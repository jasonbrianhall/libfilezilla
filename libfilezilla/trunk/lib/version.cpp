#include "libfilezilla/version.hpp"

namespace fz {
std::string get_version_string()
{
	return LIBFILEZILLA_VERSION;
}

std::tuple<int, int, int, int, std::string> get_version()
{
	return std::make_tuple(LIBFILEZILLA_VERSION_MAJOR, LIBFILEZILLA_VERSION_MINOR, LIBFILEZILLA_VERSION_MICRO, LIBFILEZILLA_VERSION_NANO, std::string(LIBFILEZILLA_VERSION_SUFFIX));
}
}
