#ifndef LIBFILEZILLA_GLUE_WX_HEADER
#define LIBFILEZILLA_GLUE_WX_HEADER

#include <wx/string.h>

#include "../format.hpp"
#include "../string.hpp"

inline std::wstring to_wstring(wxString const& s) { return s.ToStdWstring(); }

namespace fz {
template<>
inline wxString str_tolower_ascii(wxString const& s)
{
	wxString ret = s;
	// wxString is just broken, can't even use range-based for loops with it.
	for (auto it = ret.begin(); it != ret.end(); ++it) {
		*it = tolower_ascii(static_cast<wxChar>(*it));
	}
	return ret;
}

inline native_string to_native(wxString const& in)
{
	return to_native(in.ToStdWstring());
}

inline std::string to_utf8(wxString const& s)
{
	return to_utf8(s.ToStdWstring());
}

template<typename... Args>
std::wstring sprintf(wxString const& fmt, Args&&... args)
{
	return sprintf(fmt.ToStdWstring(), std::forward<Args>(args)...);
}

}

#endif
