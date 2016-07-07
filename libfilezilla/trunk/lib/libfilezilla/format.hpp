#ifndef LIBFILEZILLA_FORMAT_HEADER
#define LIBFILEZILLA_FORMAT_HEADER

#include "string.hpp"

#include <cstdlib>

#include <assert.h>

/** \file
* \brief Header for the \ref fz::sprintf "sprintf" string formatting function
*/

namespace fz {

/// \cond
namespace detail {

// Get flags
enum : char {
	pad_0 = 1,
	pad_blank = 2,
	with_width = 4
};

// Converts integral type to desired string type...
// ... basic case: simple unsigned value
template<typename String, bool Unsigned, typename Arg>
typename std::enable_if_t<std::is_integral<std::decay_t<Arg>>::value && !std::is_enum<std::decay_t<Arg>>::value, String> integral_to_string(char flags, int width, Arg && arg)
{
	std::decay_t<Arg> v = arg;

	char lead{};

	assert(!Unsigned || !std::is_signed<std::decay_t<Arg>>::value || arg >= 0);

	if (std::is_signed<std::decay_t<Arg>>::value && !(arg >= 0)) {
		lead = '-';
	}
	else if (flags & pad_blank && arg >= 0) {
		lead = ' ';
	}


	// max decimal digits in b-bit integer is floor((b-1) * log_10(2)) + 1 < b * 0.5 + 1
	typename String::value_type buf[sizeof(v) * 4 + 1];
	auto *const end = buf + sizeof(v) * 4 + 1;
	auto *p = end;

	do {
		int const mod = std::abs(static_cast<int>(v % 10));
		*(--p) = '0' + mod;
		v /= 10;
	} while (v);

	if (flags & with_width) {
		if (lead && width > 0) {
			--width;
		}

		String ret;

		if (flags & pad_0) {
			if (lead) {
				ret += lead;
			}
			if (end - p < width) {
				ret.append(width - (end - p), '0');
			}
		}
		else {
			if (end - p < width) {
				ret.append(width - (end - p), ' ');
			}
			if (lead) {
				ret += lead;
			}
		}

		ret.append(p, end);
		return ret;
	}
	else {
		if (lead) {
			*(--p) = lead;
		}
		return String(p, end);
	}
}

// ... for strongly typed enums
template<typename String, bool Unsigned, typename Arg>
typename std::enable_if_t<std::is_enum<std::decay_t<Arg>>::value, String> integral_to_string(char flags, int width, Arg && arg)
{
	return integral_to_string<String, Unsigned>(flags, width, static_cast<std::underlying_type_t<std::decay_t<Arg>>>(arg));
}

// ... assert otherwise
template<typename String, bool Unsigned, typename Arg>
typename std::enable_if_t<!std::is_integral<std::decay_t<Arg>>::value && !std::is_enum<std::decay_t<Arg>>::value, String> integral_to_string(char, int, Arg &&)
{
	assert(0);
	return String();
}


// Converts argument to string...
// ... if toString(arg) is valid expression
template<typename String, typename Arg>
auto arg_to_string(Arg&& arg) -> decltype(toString<String>(std::forward<Arg>(arg)))
{
	return toString<String>(std::forward<Arg>(arg));
}

// ... assert otherwise
template<typename String>
String arg_to_string(...)
{
	assert(0);
	return String();
}


// Converts integral type to hex string with desired string type...
// ... basic case: simple unsigned value
template<typename String, bool Lowercase, typename Arg>
typename std::enable_if_t<std::is_integral<std::decay_t<Arg>>::value && !std::is_enum<std::decay_t<Arg>>::value, String> integral_to_hex_string(Arg && arg)
{
	std::decay_t<Arg> v = arg;
	typename String::value_type buf[sizeof(v) * 2];
	auto *const end = buf + sizeof(v) * 2;
	auto *p = end;

	do {
		*(--p) = fz::int_to_hex_char<typename String::value_type, Lowercase>(v & 0xf);
		v >>= 4;
	} while (v);

	return String(p, end);
}

// ... for enums
template<typename String, bool Lowercase, typename Arg>
typename std::enable_if_t<std::is_enum<std::decay_t<Arg>>::value, String> integral_to_hex_string(Arg && arg)
{
	return integral_to_hex_string<String, Lowercase>(static_cast<std::underlying_type_t<std::decay_t<Arg>>>(arg));
}

// ... assert otherwise
template<typename String, bool Lowercase, typename Arg>
typename std::enable_if_t<!std::is_integral<std::decay_t<Arg>>::value && !std::is_enum<std::decay_t<Arg>>::value, String> integral_to_hex_string(Arg &&)
{
	assert(0);
	return String();
}


// Converts to pointer to hex string
template<typename String, typename Arg>
typename std::enable_if_t<std::is_pointer<std::decay_t<Arg>>::value, String> pointer_to_string(Arg&& arg)
{
	return String({'0', 'x'}) + integral_to_hex_string<String, true>(reinterpret_cast<uintptr_t>(arg));
}


template<typename String, typename Arg>
typename std::enable_if_t<!std::is_pointer<std::decay_t<Arg>>::value, String> pointer_to_string(Arg&& arg)
{
	assert(0);
	return String();
}


template<typename String, typename... Args>
String extract_arg(char, size_t, typename String::value_type, size_t)
{
	return String();
}

template<typename String, typename Arg, typename... Args>
String extract_arg(char flags, size_t width, typename String::value_type type, size_t arg_n, Arg&& arg, Args&&...args)
{
	String ret;

	if (!arg_n) {
		if (type == 's') {
			ret = arg_to_string<String>(std::forward<Arg>(arg));
			if (flags & with_width && ret.size() < width) {
				ret = String(width - ret.size(), ' ') + ret;
			}
		}
		else if (type == 'd' || type == 'i') {
			ret = integral_to_string<String, false>(flags, width, std::forward<Arg>(arg));
		}
		else if (type == 'u') {
			ret = integral_to_string<String, true>(flags, width, std::forward<Arg>(arg));
		}
		else if (type == 'x') {
			ret = integral_to_hex_string<String, true>(std::forward<Arg>(arg));
			if (flags & with_width && ret.size() < width) {
				ret = String(width - ret.size(), ' ') + ret;
			}
		}
		else if (type == 'X') {
			ret = integral_to_hex_string<String, false>(std::forward<Arg>(arg));
			if (flags & with_width && ret.size() < width) {
				ret = String(width - ret.size(), ' ') + ret;
			}
		}
		else if (type == 'p') {
			ret = pointer_to_string<String>(std::forward<Arg>(arg));
			if (flags & with_width && ret.size() < width) {
				ret = String(width - ret.size(), ' ') + ret;
			}
		}
		else {
			assert(0);
		}
	}
	else {
		ret = extract_arg<String>(flags, width, type, arg_n - 1, std::forward<Args>(args)...);
	}

	return ret;
}

template<typename String, typename... Args>
void process_arg(String const& fmt, typename String::size_type & pos, String& ret, size_t& arg_n, Args&&... args)
{
	++pos;

	// Get literal percent out of the way
	if (fmt[pos] == '%') {
		ret += '%';
		++pos;
		return;
	}

parse_start:
	char flags{};
	while (true) {
		if (fmt[pos] == '0') {
			flags |= pad_0;
		}
		else if (fmt[pos] == ' ') {
			flags |= pad_blank;
		}
		else {
			break;
		}
		++pos;
	}

	// Field width
	size_t width{};
	while (fmt[pos] >= '0' && fmt[pos] <= '9') {
		flags |= with_width;
		width *= 10;
		width += fmt[pos] - '0';
		++pos;
	}
	if (width > 10000) {
		assert(0);
		width = 10000;
	}

	if (fmt[pos] == '$') {
		// Positional argument, start over
		arg_n = width - 1;
		++pos;
		goto parse_start;
	}

	// Ignore length modifier
	while (true) {
		auto c = fmt[pos];
		if (c == 'h' || c == 'l' || c == 'L' || c == 'j' || c == 'z' || c == 't') {
			++pos;
		}
		else {
			break;
		}
	}

	assert(arg_n < sizeof...(args));
	if (arg_n >= sizeof...(args)) {
		++pos;
		return;
	}

	auto const type = fmt[pos++];

	ret += extract_arg<String>(flags, width, type, arg_n++, std::forward<Args>(args)...);

	// Now we're ready to print!
}

}
/// \endcond

/** \brief A simple type-safe sprintf replacement
*
* Only partially implements the format specifiers for the printf family of C functions:
*
* \li Positional arguments
* \li Supported flags: 0, ' '
* \li Field widths are supported as decimal integers not exceeding 10k, longer widths are truncated
* \li precision is ignored
* \li Supported types: d, i, u, s, x, X, p
*
* For string arguments, mixing char*, wchar_t*, std::string and std::wstring is allowed.
*
* Asserts if unsupported types are passed or if the types don't match the arguments. Fails gracefully with NDEBUG.
*
* Example:
*
* \code
* std::string s = fz::printf("%2$s %1$s", "foo", std::wstring("bar");
* assert(s == "bar foo"); // This is true
* \endcode
*/
template<typename String, typename... Args>
String sprintf(String const& fmt, Args&&... args)
{
	String ret;

	// Find % characters
	typename String::size_type start = 0, pos;
	size_t arg_n{};
	while ((pos = fmt.find('%', start)) != String::npos) {
		
		// Copy segment preceeding the %
		ret += fmt.substr(start, pos - start);

		detail::process_arg(fmt, pos, ret, arg_n, std::forward<Args>(args)...);

		start = pos;
	}

	// Copy remainder of string
	ret += fmt.substr(start);

	return ret;
}

template<typename... Args>
std::string sprintf(char const* fmt, Args&&... args)
{
	return sprintf(std::string(fmt), std::forward<Args>(args)...);
}

template<typename... Args>
std::wstring sprintf(wchar_t const* fmt, Args&&... args)
{
	return sprintf(std::wstring(fmt), std::forward<Args>(args)...);
}

}

#endif
