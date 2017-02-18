#ifndef LIBFILEZILLA_FILE_HEADER
#define LIBFILEZILLA_FILE_HEADER

#include "libfilezilla.hpp"

#ifdef FZ_WINDOWS
#include "private/windows.hpp"
#endif

/** \file
 * \brief File handling
 */

#include <stdint.h>

namespace fz {

/** \brief Lean class for file access
 *
 * This class uses the system's native file access functions. It is a less convoluted and much faster alternative
 * to the almost useless std::fstream.
 *
 * Supports large files exceeding the 32bit limits.
 */
class FZ_PUBLIC_SYMBOL file final
{
public:
	/// Files can be opened for reading or writing, but not both
	enum mode {
		reading,
		writing
	};

	/** \brief Creation flags when opening file for writing.
	 *
	 * Only evaluated when opening existing files for writing
	 * Non-existing files will always be created when writing.
	 * Opening for reading never creates files.
	 */
	enum creation_flags {
		/// Keep existing data if file exists, otherwise create new
		existing,

		/// Truncate file if already existing, otherwise create new
		empty
	};

	file();
	file(native_string const& f, mode m, creation_flags d = existing);

	~file();

	file(file const&) = delete;
	file& operator=(file const&) = delete;

	bool opened() const;

	bool open(native_string const& f, mode m, creation_flags d = existing);

	void close();

	/// Used by \ref seek
	enum seek_mode {
		/// Seek from beginning of file
		begin,

		/// Seek from current position in the file
		current,

		/// Seek from end of file
		end
	};

	/** \brief Gets size of file
	 * \return Size of file or -1 on error
	 */
	int64_t size() const;

	/** \brief Relative seek based on seek mode
	 *
	 * It is possible to seek past the end of the file. Doing so does
	 * not change the size of the file. It will only change on subsequent
	 * writes.
	 *
	 * You can get the current position int the file by passing \c current
	 * as \ref seek_mode with a 0 offset.
	 *
	 * \return -1 on error, otherwise new absolute offset in file
	 * \note On failure, the new position in the file is undefined.
	 */
	int64_t seek(int64_t offset, seek_mode m);

	/** \brief Get Current position in file */
	int64_t position() { return seek(0, current); }

	/** \brief Truncate the file to the current position of the file pointer.
	 *
	 * Despite its name, this function can extend the size of the file
	 * if the current file pointer is past the end of the file.
	 */
	bool truncate();

	/** \brief Read data from file
	 *
	 * Reading from file advances the file pointer with the number of octets read.
	 *
	 * \param buf The buffer that should receive the data. Must be large enough to hold at least \c count octets
	 * \param count The number of octets to read
	 *
	 * \return >0 The number of octets read and placed into \c buf. It may be less than \c count.
	 * \return 0 at EOF
	 * \return -1 on error
	 *
	 * \note Reading less than \c count octets can happen at any time, it does not indicate EOF.
	 */
	int64_t read(void *buf, int64_t count);

	/** \brief Write data to file
	 *
	 * Writing to file advances the file pointer with the number of octets written
	 *
	 * \param buf The buffer that holds the data to be written. Must hold at least \c count octets
	 * \param count The number of octets to write
	 *
	 * \return >=0 The number of octets written to the file. It may be less than \c count.
	 * \return -1 on error
	 */
	int64_t write(void const* buf, int64_t count);

private:
#ifdef FZ_WINDOWS
	HANDLE hFile_{INVALID_HANDLE_VALUE};
#else
	int fd_{-1};
#endif
};

bool FZ_PUBLIC_SYMBOL remove_file(native_string const& name);

}
#endif
