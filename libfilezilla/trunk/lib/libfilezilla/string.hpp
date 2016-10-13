#ifndef LIBFILEZILLA_STRING_HEADER
#define LIBFILEZILLA_STRING_HEADER

#include "libfilezilla.hpp"

#include <string>
#include <vector>

/** \file
 * \brief String types and assorted functions.
 *
 * Defines the \ref fz::native_string type and offers various functions to convert between
 * different string types.
 */

namespace fz {

/** \typedef native_string
 *
 * \brief A string in the system's native character type and encoding.\n Note: This typedef changes depending on platform!
 *
 * On Windows, the system's native encoding is UTF-16, so native_string is typedef'ed to std::wstring.
 *
 * On all other platform, native_string is a typedef for std::string.
 *
 * Always using native_string has the benefit that no conversion needs to be performed which is especially useful
 * if dealing with filenames.
 */

#ifdef FZ_WINDOWS
typedef std::wstring native_string;
#endif
#if defined(FZ_UNIX) || defined(FZ_MAC)
typedef std::string native_string;
#endif

/** \brief Converts std::string to native_string.
 *
 * \return the converted string on success. On failure an empty string is returned.
 */
native_string FZ_PUBLIC_SYMBOL to_native(std::string const& in);

/** \brief Convert std::wstring to native_string.
 *
 * \return the converted string on success. On failure an empty string is returned.
 */
native_string FZ_PUBLIC_SYMBOL to_native(std::wstring const& in);

/** \brief Locale-sensitive stricmp
 *
 * Like std::string::strcmp but case-insensitive, respecting locale.
 *
 * \note does not handle embedded null
 */
int FZ_PUBLIC_SYMBOL stricmp(std::string const& a, std::string const& b);
int FZ_PUBLIC_SYMBOL stricmp(std::wstring const& a, std::wstring const& b);

/** \brief Converts ASCII uppercase characters to lowercase as if C-locale is used.

 Under some locales there is a different case-relationship
 between the letters a-z and A-Z as one expects from ASCII under the C locale.
 In Turkish for example there are different variations of the letter i,
 namely dotted and dotless. What we see as 'i' is the lowercase dotted i and
 'I' is the  uppercase dotless i. Since std::tolower is locale-aware, I would
 become the dotless lowercase i.

 This is not always what we want. FTP commands for example are case-insensitive
 ASCII strings, LIST and list are the same.

 tolower_ascii instead converts all types of 'i's to the ASCII i as well.

 \return  A-Z becomes a-z.\n In addition dotless lowercase i and dotted uppercase i also become the standard i.

 */
template<typename Char>
Char tolower_ascii(Char c) {
	if (c >= 'A' && c <= 'Z') {
		return c + ('a' - 'A');
	}
	return c;
}

template<>
std::wstring::value_type FZ_PUBLIC_SYMBOL tolower_ascii(std::wstring::value_type c);

/// \brief Converts ASCII lowercase characters to uppercase as if C-locale is used.
template<typename Char>
Char toupper_ascii(Char c) {
	if (c >= 'a' && c <= 'z') {
		return c + ('A' - 'a');
	}
	return c;
}

template<>
std::wstring::value_type FZ_PUBLIC_SYMBOL toupper_ascii(std::wstring::value_type c);

/** \brief tr_tolower_ascii does for strings what tolower_ascii does for individual characters
 */
 // Note: For UTF-8 strings it works on individual octets!
template<typename String>
String str_tolower_ascii(String const& s)
{
	String ret = s;
	for (auto& c : ret) {
		c = tolower_ascii(c);
	}
	return ret;
}

template<typename String>
String str_toupper_ascii(String const& s)
{
	String ret = s;
	for (auto& c : ret) {
		c = toupper_ascii(c);
	}
	return ret;
}

/** \brief Converts from std::string in system encoding into std::wstring
 *
 * \return the converted string on success. On failure an empty string is returned.
 *
 * \note Does not handle embedded nulls
 */
std::wstring FZ_PUBLIC_SYMBOL to_wstring(std::string const& in);

/// Returns identity, that way to_wstring can be called with native_string.
inline std::wstring FZ_PUBLIC_SYMBOL to_wstring(std::wstring const& in) { return in; }

/// Converts from arithmetic type to std::wstring
template<typename Arg>
inline typename std::enable_if<std::is_arithmetic<std::decay_t<Arg>>::value, std::wstring>::type to_wstring(Arg && arg)
{
	return std::to_wstring(std::forward<Arg>(arg));
}


/** \brief Converts from std::string in UTF-8 into std::wstring
 *
 * \return the converted string on success. On failure an empty string is returned.
 */
std::wstring FZ_PUBLIC_SYMBOL to_wstring_from_utf8(std::string const& in);

/** \brief Converts from std::wstring into std::string in system encoding
 *
 * \return the converted string on success. On failure an empty string is returned.
 *
 * \note Does not handle embedded nulls
 */
std::string FZ_PUBLIC_SYMBOL to_string(std::wstring const& in);

/// Returns identity, that way to_string can be called with native_string.
inline std::string FZ_PUBLIC_SYMBOL to_string(std::string const& in) { return in; }

/// Converts from arithmetic type to std::string
template<typename Arg>
inline typename std::enable_if<std::is_arithmetic<std::decay_t<Arg>>::value, std::string>::type to_string(Arg && arg)
{
	return std::to_string(std::forward<Arg>(arg));
}


/// Returns length of 0-terminated character sequence. Works with both narrow and wide-characters.
template<typename Char>
size_t strlen(Char const* str) {
	return std::char_traits<Char>::length(str);
}


/** \brief Converts from std::string in native encoding into std::string in UTF-8
 *
 * \return the converted string on success. On failure an empty string is returned.
 *
 * \note Does not handle embedded nulls
 */
std::string FZ_PUBLIC_SYMBOL to_utf8(std::string const& in);

/** \brief Converts from std::wstring in native encoding into std::string in UTF-8
 *
 * \return the converted string on success. On failure an empty string is returned.
 *
 * \note Does not handle embedded nulls
 */
std::string FZ_PUBLIC_SYMBOL to_utf8(std::wstring const& in);


/** \brief Converts a hex digit to decimal int
 *
 * Example: '9' becomes 9, 'b' becomes 11, 'D' becomes 13
 *
 * Undefined output if input is not a valid hex digit.
 */
template<typename Char>
int hex_char_to_int(Char c)
{
	if (c >= 'a')
		return c - 'a' + 10;
	if (c >= 'A')
		return c - 'A' + 10;
	else
		return c - '0';
}

/** \brief Converts an integer to the corresponding lowercase hex digit
*
* Example: 9 becomes '9', 11 becomes 'b'
*
* Undefined output if input is less than 0 or larger than 15
*/
template<typename Char = char, bool Lowercase = true>
Char int_to_hex_char(int d)
{
	if (d > 9) {
		return (Lowercase ? 'a' : 'A') + d - 10;
	}
	else {
		return '0' + d;
	}
}

/// Calls either fz::to_string or fz::to_wstring depending on the passed template argument
template<typename String, typename Arg>
inline auto toString(Arg&& arg) -> typename std::enable_if<std::is_same<String, std::string>::value, decltype(to_string(std::forward<Arg>(arg)))>::type
{
	return to_string(std::forward<Arg>(arg));
}

template<typename String, typename Arg>
inline auto toString(Arg&& arg) -> typename std::enable_if<std::is_same<String, std::wstring>::value, decltype(to_wstring(std::forward<Arg>(arg)))>::type
{
	return to_wstring(std::forward<Arg>(arg));
}

#if !defined(fzT) || defined(DOXYGEN)
#ifdef FZ_WINDOWS
/** \brief Macro for a string literal in system-native character type.\n Note: Macro definition changes depending on platform!
 *
 * Example: \c fzT("this string is wide on Windows and narrow elsewhere")
 */
#define fzT(x) L ## x
#else
/** \brief Macro for a string literal in system-native character type.\n Note: Macro definition changes depending on platform!
 *
 * Example: \c fzT("this string is wide on Windows and narrow elsewhere")
 */
#define fzT(x) x
#endif
#endif

 /// Returns the function argument of the type matching the template argument. \sa fzS
template<typename Char>
Char const* choose_string(char const* c, wchar_t const* w);

template<> inline char const* choose_string(char const* c, wchar_t const*) { return c; }
template<> inline wchar_t const* choose_string(char const*, wchar_t const* w) { return w; }

#if !defined(fzS) || defined(DOXYGEN)
/** \brief Macro to get const pointer to a string of the corresponding type
 *
 * Useful when using string literals in templates where the type of string
 * is a template argument:
 * \code
 *   template<typename String>
 *   String append_foo(String const& s) {
 *       s += fzS(String::value_type, "foo");
 *   }
 * \endcode
 */
#define fzS(Char, s) fz::choose_string<Char>(s, L ## s)
#endif

 /// Returns \c in with all occurrences of \c find in the input string replaced with \c replacement
std::string FZ_PUBLIC_SYMBOL replaced_substrings(std::string const& in, std::string const& find, std::string const& replacement);
std::wstring FZ_PUBLIC_SYMBOL replaced_substrings(std::wstring const& in, std::wstring const& find, std::wstring const& replacement);

/// Modifies \c in, replacing all occurrences of \c find with \c replacement
void FZ_PUBLIC_SYMBOL replace_substrings(std::string& in, std::string const& find, std::string const& replacement);
void FZ_PUBLIC_SYMBOL replace_substrings(std::wstring& in, std::wstring const& find, std::wstring const& replacement);

/// Tokenizes string. Returns all non-empty substrings
template<typename String, typename Delim, typename Container = std::vector<String>>
Container strtok(String const& s, Delim const& delims)
{
	Container ret;

	typename String::size_type start{}, pos{};
	do {
		pos = s.find_first_of(delims, start);

		// Not found, we're at ends;
		if (pos == String::npos) {
			if (start + 1 < s.size()) {
				ret.emplace_back(s.substr(start));
			}
		}
		else if (pos > start + 1) {
			// Non-empty substring
			ret.emplace_back(s.substr(start, pos - start));
		}
		start = pos + 1;
	} while (pos != String::npos);

	return ret;
}


/// \brief Encodes raw input string to base64
std::string FZ_PUBLIC_SYMBOL base64_encode(std::string const& in);

/// \brief Decodes base64, ignores whitespace. Returns empty string on invalid input.
std::string FZ_PUBLIC_SYMBOL base64_decode(std::string const& in);

// Converts string to integral type T. If string is not convertible, T() is returned.
template<typename T, typename String>
T to_integral(String const& s)
{
	T ret{};

	auto it = s.cbegin();
	if (it != s.cend() && (*it == '-' || *it == '+')) {
		++it;
	}

	for (; it != s.cend(); ++it) {
		auto const& c = *it;
		if (c < '0' || c > '9') {
			return T();
		}
		ret *= 10;
		ret += c - '0';
	}

	if (!s.empty() && s.front() == '-') {
		return ret *= static_cast<T>(-1);
	}
	else {
		return ret;
	}
}

/// \brief Returns true iff the string only has characters in the 7-bit ASCII range
template<typename String>
bool str_is_ascii(String const& s) {
	for (auto const& c : s) {
		if (static_cast<std::make_unsigned_t<typename String::value_type>>(c) > 127) {
			return false;
		}
	}

	return true;
}

/// \brief Return passed string with all leading and trailing whitespace removed
template<typename String>
String trimmed(String const& s) {
	size_t const first = s.find_first_not_of(fzS(typename String::value_type, " \r\n\t"));
	if (first == String::npos) {
		return String();
	}
	else {
		// Cannot be npos
		size_t const last = s.find_last_not_of(fzS(typename String::value_type, " \r\n\t"));

		return s.substr(first, last - first + 1);
	}
}


/// \brief Remove all leading and trailing whitespace from string
template<typename String>
void trim(String & s) {
	s = trimmed(s);
}

}

#endif
