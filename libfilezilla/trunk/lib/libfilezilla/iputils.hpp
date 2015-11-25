#ifndef LIBFILEZILLA_IPUTILS_HEADER
#define LIBFILEZILLA_IPUTILS_HEADER

#include "libfilezilla.hpp"

/** \file
 * \brief Various functions to deal with IP address strings
 */

namespace fz {

/** \brief Given a shortened IPv6 address, returns the full, unshortened address
 *
 * If passed address is encloded in square brackes, they are stripped.
 *
 * Returns an empty string if the passed string isn't a valid IPv6 address.
 */
std::string FZ_PUBLIC_SYMBOL get_ipv6_long_form(std::string const& short_address);
std::wstring FZ_PUBLIC_SYMBOL get_ipv6_long_form(std::wstring const& short_address);

enum class address_type
{
	unknown,
	ipv4,
	ipv6
};

bool FZ_PUBLIC_SYMBOL is_routable_address(std::string const& address);
bool FZ_PUBLIC_SYMBOL is_routable_address(std::wstring const& address);

address_type FZ_PUBLIC_SYMBOL get_address_type(std::string const& address);
address_type FZ_PUBLIC_SYMBOL get_address_type(std::wstring const& address);

}

#endif
