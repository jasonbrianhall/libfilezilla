#include "libfilezilla/string.hpp"

#ifdef FZ_WINDOWS
#include <string.h>

#include "libfilezilla/private/windows.hpp"
#else
#include <iconv.h>
#include <strings.h>

#include <type_traits>
#endif

#include <cstdlib>

static_assert('a' + 25 == 'z', "We only support systems running with an ASCII-based character set. Sorry, no EBCDIC.");

// char may be unsigned, yielding stange results if subtracting characters. To work around it, expect a particular order of characters.
static_assert('A' < 'a', "We only support systems running with an ASCII-based character set. Sorry, no EBCDIC.");

namespace fz {

#ifdef FZ_WINDOWS
native_string to_native(std::string const& in)
{
	return to_wstring(in);
}

native_string to_native(std::wstring const& in)
{
	return in;
}
#else
native_string to_native(std::string const& in)
{
	return in;
}

native_string to_native(std::wstring const& in)
{
	return to_string(in);
}
#endif

int stricmp(std::string const& a, std::string const& b)
{
#ifdef FZ_WINDOWS
	return _stricmp(a.c_str(), b.c_str());
#else
	return strcasecmp(a.c_str(), b.c_str());
#endif
}

int stricmp(std::wstring const& a, std::wstring const& b)
{
#ifdef FZ_WINDOWS
	return _wcsicmp(a.c_str(), b.c_str());
#else
	return wcscasecmp(a.c_str(), b.c_str());
#endif
}

template<>
std::wstring::value_type tolower_ascii(std::wstring::value_type c)
{
	if (c >= 'A' && c <= 'Z') {
		return c + ('a' - 'A');
	}
	else if (c == 0x130 || c == 0x131) {
		c = 'i';
	}
	return c;
}

std::wstring to_wstring(std::string const& in)
{
	std::mbstate_t ps{};
	std::wstring ret;

	char const* in_p = in.c_str();
	size_t len = std::mbsrtowcs(0, &in_p, 0, &ps);
	if (len != static_cast<size_t>(-1)) {
		ret.resize(len);
		wchar_t* out_p = &ret[0];

		in_p = in.c_str(); // Some implementations of mbsrtowcs change src even on null dst
		len = std::mbsrtowcs(out_p, &in_p, len + 1, &ps);
	}

	return ret;
}

#ifndef FZ_WINDOWS
// On some platforms, e.g. NetBSD, the second argument to iconv is const.
// Depending which one is used, declare iconv_second_arg_type as either char* or char const*
extern "C" typedef size_t (*iconv_prototype_with_const)(iconv_t, char const**, size_t *, char**, size_t *);
typedef std::conditional<std::is_same<decltype(&iconv), iconv_prototype_with_const>::value, char const*, char*>::type iconv_second_arg_type;

namespace {
// On some platforms, e.g. those derived from SunOS, iconv does not understand "WCHAR_T", so we
// need to guess an encoding.
// On other platforms, WCHAR_T results in iconv() doing endless loops, such as OS X.
char const* calc_wchar_t_encoding()
{
	auto try_encoding = [](char const* const encoding) -> bool {
		iconv_t cd = iconv_open(encoding, "UTF-8");
		if (cd == reinterpret_cast<iconv_t>(-1)) {
			return false;
		}
		iconv_close(cd);
		return true;
	};

	// Explicitly specify endianess, otherwise we'll get a BOM prefixed to everything
	int const i = 1;
	char const* p = reinterpret_cast<char const*>(&i);
	bool little_endian = p[0] == 1;

	if (sizeof(wchar_t) == 4) {
		if (little_endian && try_encoding("UTF-32LE")) {
			return "UTF-32LE";
		}
		if (!little_endian && try_encoding("UTF-32BE")) {
			return "UTF-32BE";
		}
	}
	else if (sizeof(wchar_t) == 2) {
		if (little_endian && try_encoding("UTF-16LE")) {
			return "UTF-16LE";
		}
		if (!little_endian && try_encoding("UTF-16BE")) {
			return "UTF-16BE";
		}
	}

	// Oh dear...
	// WCHAR_T is our last, best hope.
	return "WCHAR_T";
}

char const* wchar_t_encoding()
{
	static char const* const encoding = calc_wchar_t_encoding();
	return encoding;
}
}
#endif

std::wstring to_wstring_from_utf8(std::string const& in)
{
	std::wstring ret;

	if (!in.empty()) {
#if FZ_WINDOWS
		char const* const in_p = in.c_str();
		int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, in_p, static_cast<int>(in.size()), 0, 0);
		if (len > 0) {
			ret.resize(len);
			wchar_t* out_p = &ret[0];
			MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, in_p, static_cast<int>(in.size()), out_p, len);
		}
#else
		iconv_t cd = iconv_open(wchar_t_encoding(), "UTF-8");
		if (cd != reinterpret_cast<iconv_t>(-1)) {
			auto in_p = const_cast<iconv_second_arg_type>(in.c_str());
			size_t in_len = in.size();

			size_t out_len = in_len * sizeof(wchar_t) * 2;
			char* out_buf = new char[out_len];
			char* out_p = out_buf;

			size_t r = iconv(cd, &in_p, &in_len, &out_p, &out_len);

			if (r != static_cast<size_t>(-1)) {
				ret.assign(reinterpret_cast<wchar_t*>(out_buf), reinterpret_cast<wchar_t*>(out_p));
			}

			// Our buffer should big enough as well, so we can ignore errors such as E2BIG.

			delete [] out_buf;

			iconv_close(cd);
		}
#endif
	}

	return ret;
}

std::string to_string(std::wstring const& in)
{
	std::mbstate_t ps{};
	std::string ret;

	wchar_t const* in_p = in.c_str();
	size_t len = std::wcsrtombs(0, &in_p, 0, &ps);
	if (len != static_cast<size_t>(-1)) {
		ret.resize(len);
		char* out_p = &ret[0];

		in_p = in.c_str(); // Some implementations of wcsrtombs change src even on null dst
		std::wcsrtombs(out_p, &in_p, len + 1, &ps);
	}

	return ret;
}

std::string FZ_PUBLIC_SYMBOL to_utf8(std::string const& in)
{
	return to_utf8(to_wstring(in));
}

std::string FZ_PUBLIC_SYMBOL to_utf8(std::wstring const& in)
{
	std::string ret;

	if (!in.empty()) {
#if FZ_WINDOWS
		wchar_t const* const in_p = in.c_str();
		int len = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, in_p, static_cast<int>(in.size()), 0, 0, 0, 0);
		if (len > 0) {
			ret.resize(len);
			char* out_p = &ret[0];
			WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, in_p, static_cast<int>(in.size()), out_p, len, 0, 0);
		}
#else
		iconv_t cd = iconv_open("UTF-8", wchar_t_encoding());
		if (cd != reinterpret_cast<iconv_t>(-1)) {
			auto in_p = reinterpret_cast<iconv_second_arg_type>(const_cast<wchar_t*>(in.c_str()));
			size_t in_len = in.size() * sizeof(wchar_t);

			size_t out_len = in.size() * 4;
			char* out_buf = new char[out_len];
			char* out_p = out_buf;

			size_t r = iconv(cd, &in_p, &in_len, &out_p, &out_len);

			if (r != static_cast<size_t>(-1)) {
				ret.assign(out_buf, out_p);
			}

			// Our buffer should big enough as well, so we can ignore errors such as E2BIG.

			delete[] out_buf;

			iconv_close(cd);
		}
#endif
	}

	return ret;
}

namespace {
template<typename String>
inline void do_replace_substrings(String& in, String const& find, String const& replacement)
{
	size_t pos = in.find(find);
	while (pos != std::string::npos) {
		in.replace(pos, find.size(), replacement);
		pos = in.find(find, pos + replacement.size());
	}
}
}

std::string replaced_substrings(std::string const& in, std::string const& find, std::string const& replacement)
{
	std::string ret = in;
	do_replace_substrings(ret, find, replacement);
	return ret;
}

std::wstring replaced_substrings(std::wstring const& in, std::wstring const& find, std::wstring const& replacement)
{
	std::wstring ret = in;
	do_replace_substrings(ret, find, replacement);
	return ret;
}

void replace_substrings(std::string& in, std::string const& find, std::string const& replacement)
{
	do_replace_substrings(in, find, replacement);
}

void replace_substrings(std::wstring& in, std::wstring const& find, std::wstring const& replacement)
{
	do_replace_substrings(in, find, replacement);
}

std::string base64_encode(std::string const& in)
{
	std::string::value_type const base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	std::string ret;

	size_t len = in.size();
	size_t pos{};

	ret.reserve(((len + 2) / 3) * 4);

	while (len >= 3) {
		len -= 3;
		auto const c1 = static_cast<unsigned char>(in[pos++]);
		auto const c2 = static_cast<unsigned char>(in[pos++]);
		auto const c3 = static_cast<unsigned char>(in[pos++]);

		ret += base64_chars[(c1 >> 2) & 0x3fu];
		ret += base64_chars[((c1 & 0x3u) << 4) | ((c2 >> 4) & 0xfu)];
		ret += base64_chars[((c2 & 0xfu) << 2) | ((c3 >> 6) & 0x3u)];
		ret += base64_chars[(c3 & 0x3fu)];
	}
	if (len) {
		auto const c1 = static_cast<unsigned char>(in[pos++]);
		ret += base64_chars[(c1 >> 2) & 0x3fu];
		if (len == 2) {
			auto const c2 = static_cast<unsigned char>(in[pos++]);
			ret += base64_chars[((c1 & 0x3u) << 4) | ((c2 >> 4) & 0xfu)];
			ret += base64_chars[(c2 & 0xfu) << 2];
		}
		else {
			ret += base64_chars[(c1 & 0x3u) << 4];
			ret += '=';
		}
		ret += '=';
	}

	return ret;
}

std::string base64_decode(std::string const& in)
{
	unsigned char const chars[256] =
	{
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x80, 0xff, 0x80, 0x80, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3e, 0xff, 0xff, 0xff, 0x3f,
		0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0xff, 0xff, 0xff, 0x40, 0xff, 0xff,
		0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
		0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
		0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
	};

	std::string ret;
	ret.reserve((in.size() / 4) * 3);

	size_t pos{};
	size_t len = in.size();

	// Trim trailing whitespace
	while (len && chars[static_cast<unsigned char>(in[len - 1])] == 0x80) {
		--len;
	}

	auto next = [&]() {
		while (pos < len) {
			auto c = chars[static_cast<unsigned char>(in[pos++])];
			if (c != 0x80u) {
				return c;
			}
		}
		return static_cast<unsigned char>(0xffu);
	};

	while (pos < len) {
		auto c1 = next();
		auto c2 = next();
		auto c3 = next();
		auto c4 = next();

		if (c1 == 0xff || c2 == 0xff || c3 == 0xff || c4 == 0xff || c1 == 0x40 || c2 == 0x40) {
			// Bad input
			return std::string();
		}

		if (c4 == 0x40) {
			// Pad
			if (pos < len) {
				// Not at end of string
				return std::string();
			}
			ret += (c1 << 2) | ((c2 >> 4) & 0x3);

			if (c3 != 0x40) {
				ret += ((c2 & 0xf) << 4) | ((c3 >> 2) & 0xf);
			}
		}
		else {
			if (c3 == 0x40) {
				// Bad input
				return std::string();
			}

			ret += (c1 << 2) | ((c2 >> 4) & 0x3);
			ret += ((c2 & 0xf) << 4) | ((c3 >> 2) & 0xf);
			ret += ((c3 & 0x3) << 6) | c4;
		}
	}

	return ret;
}


}
