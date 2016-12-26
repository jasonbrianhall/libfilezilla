#ifndef LIBFILEZILLA_LOCAL_FILESYS_HEADER
#define LIBFILEZILLA_LOCAL_FILESYS_HEADER

#include "libfilezilla.hpp"
#include "time.hpp"

#ifdef FZ_WINDOWS
#include "private/windows.hpp"
#else
#include <dirent.h>
#endif

/** \file
 * \brief Declares local_filesys class to enumerate local files and query their metadata such as type, size and modification time.
 */
namespace fz {

/**
 * \brief This class can be used to enumerate the contents of local directories and to query
 * the metadata of files.
 *
 * This class is aware of symbolic links. Under Windows it can handle reparse points as well.
 */
class FZ_PUBLIC_SYMBOL local_filesys final
{
public:
	local_filesys() = default;
	~local_filesys();

	local_filesys(local_filesys const&) = delete;
	local_filesys& operator=(local_filesys const&) = delete;

	/// Types of files. While 'everything is a file', a filename can refer to a file proper, a directory or a symbolic link.
	enum type {
		unknown = -1,
		file,
		dir,
		link
	};

	/// The system's preferred path separator
	static char const path_separator;

	/// \brief Checks whether given character is a path separator.
	///
	/// On most systems, the forward slash is the only separator. The exception is Windows where both forward and backward slashes are separators, with the latter being preferred.
	static inline bool is_separator(wchar_t c) {
#ifdef FZ_WINDOWS
		return c == '/' || c == '\\';
#else
		return c == '/';
#endif
	}

	/// \brief GetFileType return the type of the passed path.
	///
	/// Can optionally follow symbolic links.
	static type get_file_type(native_string const& path, bool follow_links = false);

	/// Gets the info for the passed arguments. Follows symbolic links and stats the target, sets is_link to true if path was
	/// a link.
	static type get_file_info(native_string const& path, bool &is_link, int64_t* size, datetime* modification_time, int* mode);

	/// Gets size of file, returns -1 on error.
	static int64_t get_size(native_string const& path, bool *is_link = 0);

	/// \brief Begins enumerating a directory.
	///
	/// \param dirs_only If true, only directories are enumerated.
	bool begin_find_files(native_string path, bool dirs_only = false);

	/// Gets the next file in the directory. Call until it returns false.
	bool get_next_file(native_string& name);

	/// Gets the next file in the directory. Call until it returns false.
	///
	/// Stores the metadata in any non-null arguments.
	bool get_next_file(native_string& name, bool &is_link, bool &is_dir, int64_t* size, datetime* modification_time, int* mode);

	/// Ends enumerating files. Automatically called in the destructor.
	void end_find_files();

	static datetime get_modification_time(native_string const& path);
	static bool set_modification_time(native_string const& path, const datetime& t);

	/// Get the target path of a symbolic link
	static native_string get_link_target(native_string const& path);

private:
#ifndef FZ_WINDOWS
	void alloc_path_buffer(char const* filename); // Ensures m_raw_path is large enough to hold path and filename
#endif

	// State for directory enumeration
	bool m_dirs_only{};

#ifdef FZ_WINDOWS
	WIN32_FIND_DATA m_find_data{};
	HANDLE m_hFind{INVALID_HANDLE_VALUE};
	bool m_found{};
	native_string m_find_path;
#else
	char* m_raw_path{};
	char* m_file_part{}; // Points into m_raw_path past the trailing slash of the path part
	int m_buffer_length{};
	DIR* m_dir{};
#endif
};

}

#endif
