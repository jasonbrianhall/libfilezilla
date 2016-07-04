#include "libfilezilla/iputils.hpp"

#include <stddef.h>

namespace fz {
template<typename String, typename Char = typename String::value_type>
String do_get_ipv6_long_form(String short_address)
{
	if (!short_address.empty() && short_address[0] == '[') {
		if (short_address.back() != ']') {
			return String();
		}
		short_address = short_address.substr(1, short_address.size() - 2);
	}
	short_address = str_tolower_ascii(short_address);

	Char buffer[40] = { '0', '0', '0', '0', ':',
		'0', '0', '0', '0', ':',
		'0', '0', '0', '0', ':',
		'0', '0', '0', '0', ':',
		'0', '0', '0', '0', ':',
		'0', '0', '0', '0', ':',
		'0', '0', '0', '0', ':',
		'0', '0', '0', '0', 0
	};

	Char* out = buffer;

	const size_t len = short_address.size();
	if (len > 39)
		return String();

	// First part, before possible ::
	size_t i = 0;
	size_t grouplength = 0;

	Char const* s = short_address.c_str();
	for (i = 0; i < len + 1; ++i) {
		Char const& c = s[i];
		if (c == ':' || !c) {
			if (!grouplength) {
				// Empty group length, not valid
				if (!c || s[i + 1] != ':') {
					return String();
				}
				++i;
				break;
			}

			out += 4 - grouplength;
			for (size_t j = grouplength; j > 0; --j) {
				*out++ = s[i - j];
			}
			// End of string...
			if (!c) {
				if (!*out) {
					// ...on time
					return buffer;
				}
				else {
					// ...premature
					return String();
				}
			}
			else if (!*out) {
				// Too long
				return String();
			}

			++out;

			grouplength = 0;
			if (s[i + 1] == ':') {
				++i;
				break;
			}
			continue;
		}
		else if ((c < '0' || c > '9') &&
			(c < 'a' || c > 'f'))
		{
			// Invalid character
			return String();
		}
		// Too long group
		if (++grouplength > 4)
			return String();
	}

	// Second half after ::

	Char* end_first = out;
	out = &buffer[38];
	size_t stop = i;
	for (i = len - 1; i > stop; --i) {
		if (out < end_first) {
			// Too long
			return String();
		}

		Char const& c = s[i];
		if (c == ':') {
			if (!grouplength) {
				// Empty group length, not valid
				return String();
			}

			out -= 5 - grouplength;

			grouplength = 0;
			continue;
		}
		else if ((c < '0' || c > '9') &&
			(c < 'a' || c > 'f'))
		{
			// Invalid character
			return String();
		}
		// Too long group
		if (++grouplength > 4) {
			return String();
		}
		*out-- = c;
	}
	if (!grouplength) {
		// Empty group length, not valid
		return String();
	}
	out -= 5 - grouplength;
	out += 2;

	ptrdiff_t diff = out - end_first;
	if (diff < 0 || diff % 5) {
		return String();
	}

	return buffer;
}

std::string get_ipv6_long_form(std::string const& short_address)
{
	return do_get_ipv6_long_form(short_address);
}

std::wstring get_ipv6_long_form(std::wstring const& short_address)
{
	return do_get_ipv6_long_form(short_address);
}

template<typename String, typename Char = typename String::value_type>
bool do_is_routable_address(String const& address)
{
	auto const type = get_address_type(address);

	if (type == address_type::ipv6) {
		String long_address = do_get_ipv6_long_form(address);
		if (long_address.size() != 39) {
			return false;
		}
		if (long_address[0] == '0') {
			// ::/128
			if (long_address == fzS(Char, "0000:0000:0000:0000:0000:0000:0000:0000")) {
				return false;
			}
			// ::1/128
			if (long_address == fzS(Char, "0000:0000:0000:0000:0000:0000:0000:0001")) {
				return false;
			}

			if (long_address.substr(0, 30) == fzS(Char, "0000:0000:0000:0000:0000:ffff:")) {
				Char const dot = '.';
				// IPv4 mapped
				String ipv4 =
					toString<String>(hex_char_to_int(long_address[30]) * 16 + hex_char_to_int(long_address[31])) + dot +
					toString<String>(hex_char_to_int(long_address[32]) * 16 + hex_char_to_int(long_address[33])) + dot +
					toString<String>(hex_char_to_int(long_address[35]) * 16 + hex_char_to_int(long_address[36])) + dot +
					toString<String>(hex_char_to_int(long_address[37]) * 16 + hex_char_to_int(long_address[38]));

				return do_is_routable_address(ipv4);
			}

			return true;
		}
		if (long_address[0] == 'f') {
			if (long_address[1] == 'e') {
				// fe80::/10 (link local)
				int v = hex_char_to_int(long_address[2]);
				return (v & 0xc) != 0x8;
			}
			else if (long_address[1] == 'c' || long_address[1] == 'd') {
				// fc00::/7 (site local)
				return false;
			}
		}

		return true;
	}
	else if (type == address_type::ipv4) {
		if (address.size() < 7) {
			return false;
		}

		// Assumes address is already a valid IP address
		if (address.substr(0, 3) == fzS(Char, "127") ||
			address.substr(0, 3) == fzS(Char, "10.") ||
			address.substr(0, 7) == fzS(Char, "192.168") ||
			address.substr(0, 7) == fzS(Char, "169.254"))
		{
			return false;
		}

		if (address.substr(0, 3) == fzS(Char, "172")) {
			auto middle = address.substr(4);
			auto pos = middle.find('.');
			if (pos == String::npos || pos > 3) {
				return false;
			}

			int segment = std::stoi(middle.substr(0, pos)); // Cannot throw as we have verified it to be a valid IPv4
			if (segment >= 16 && segment <= 31) {
				return false;
			}
		}

		return true;
	}

	return false;
}

bool is_routable_address(std::string const& address)
{
	return do_is_routable_address(address);
}

bool is_routable_address(std::wstring const& address)
{
	return do_is_routable_address(address);
}

template<typename String>
address_type do_get_address_type(String const& address)
{
	if (!do_get_ipv6_long_form(address).empty()) {
		return address_type::ipv6;
	}

	int segment = 0;
	int dotcount = 0;

	for (size_t i = 0; i < address.size(); ++i) {
		auto const c = address[i];
		if (c == '.') {
			if (i + 1 < address.size() && address[i + 1] == '.') {
				// Disallow multiple dots in a row
				return address_type::unknown;
			}

			if (segment > 255)
				return address_type::unknown;
			if (!dotcount && !segment)
				return address_type::unknown;
			dotcount++;
			segment = 0;
		}
		else if (c < '0' || c > '9') {
			return address_type::unknown;
		}
		else {
			segment = segment * 10 + c - '0';
		}
	}
	if (dotcount != 3) {
		return address_type::unknown;
	}

	if (segment > 255) {
		return address_type::unknown;
	}

	return address_type::ipv4;
}

address_type get_address_type(std::string const& address)
{
	return do_get_address_type(address);
}

address_type get_address_type(std::wstring const& address)
{
	return do_get_address_type(address);
}
}
