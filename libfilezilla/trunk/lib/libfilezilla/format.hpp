#ifndef LIBFILEZILLA_FORMAT_HEADER
#define LIBFILEZILLA_FORMAT_HEADER

#include "string.hpp"

#include <assert.h>

/** \file
* \brief Header for the \ref fz::sprintf "sprintf" string formatting function
*/

namespace fz {

/// \cond
namespace detail {

// Converts integral type to desired string type...
// ... basic case: simple unsigned value
template<typename String, bool Unsigned, typename Arg>
typename std::enable_if_t<std::is_integral<std::decay_t<Arg>>::value && !(Unsigned && std::is_signed<std::decay_t<Arg>>::value), String> integral_to_string(Arg && arg)
{
	return toString<String>(arg);
}

// ... for signed types, assert if an unsigned types is expected and a negative value is passed
template<typename String, bool Unsigned, typename Arg>
typename std::enable_if_t<std::is_integral<std::decay_t<Arg>>::value && Unsigned && std::is_signed<std::decay_t<Arg>>::value, String> integral_to_string(Arg && arg)
{
	assert(arg >= 0);
	return toString<String>(arg);
}

// ... assert if not a integral type
template<typename String, bool Unsigned, typename Arg>
typename std::enable_if_t<!std::is_integral<std::decay_t<Arg>>::value, String> integral_to_string(Arg && arg)
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
template<typename String, typename Arg>
String arg_to_string(...)
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
	if (!arg_n) {
		if (type == 's') {
			return arg_to_string<String>(std::forward<Arg>(arg));
		}
		else if (type == 'd' || type == 'i') {
			return integral_to_string<String, false>(std::forward<Arg>(arg));
		}
		else if (type == 'u') {
			return integral_to_string<String, true>(std::forward<Arg>(arg));
		}
		else {
			assert(0);
			return String();
		}
	}
	else {
		return extract_arg<String>(flags, width, type, arg_n - 1, std::forward<Args>(args)...);
	}
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
	// Get flags
	enum : char {
		pad_0 = 1,
		pad_blank = 2,
		with_width = 4
	};
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

	String arg = extract_arg<String>(flags, width, type, arg_n++, std::forward<Args>(args)...);

	if (flags & pad_blank) {
		ret += ' ';
		if (width) {
			--width;
		}
	}

	if (flags & with_width) {
		if (arg.size() < width) {
			if (flags & pad_0) {
				ret += String(width - arg.size(), '0');
			}
			else {
				ret += String(width - arg.size(), ' ');
			}
		}
	}

	ret += arg;


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
* \li Supported types: d, i, u, s
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
