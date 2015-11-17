#ifndef LIBFILEZILLA_RECURSIVE_REMOVE_HEADER
#define LIBFILEZILLA_RECURSIVE_REMOVE_HEADER

#include "libfilezilla.hpp"

#ifdef FZ_WINDOWS
#include "private/windows.hpp"
#endif

#include <list>

namespace fz {

class recursive_remove
{
public:
	recursive_remove() = default;
	virtual ~recursive_remove() = default;

	recursive_remove(recursive_remove const&) = delete;
	recursive_remove& operator=(recursive_remove const&) = delete;

	// If parent window is given, display confirmation dialog
	// Returns false iff there's an encoding error, e.g. program
	// started without UTF-8 locale.
	bool remove(native_string const& path);
	bool remove(std::list<native_string> dirsToVisit);

protected:
	virtual bool confirm() const { return true; }

#ifdef FZ_WINDOWS
	virtual void adjust_shfileop(SHFILEOPSTRUCT & op);
#endif
};

}

#endif
