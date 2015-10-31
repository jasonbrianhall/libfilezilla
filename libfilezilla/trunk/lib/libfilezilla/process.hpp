#ifndef LIBFILEZILLA_PROCESS_HEADER
#define LIBFILEZILLA_PROCESS_HEADER

#include "libfilezilla.hpp"

/** \file
 * \brief Header for the \ref fz::process "process" class
 */

#include <vector>

namespace fz {

/** \brief The process class manages an asynchronous process with redirected IO.
 *
 * No console window is being created.

 * To use, spawn the process and, since it's blocking, call read from a different thread.
 *
 */
class FZ_PUBLIC_SYMBOL process final
{
public:
	process();
	~process();

	process(process const&) = delete;
	process& operator=(process const&) = delete;

	/** \brief Start the process
	 *
	 * This function takes care of properly quoting and escaping the the program's path and its arguments.
	 * Fails if process has already been spawned.
	 *
	 * \param cmd The path of the program to execute
	 * \param args The command-line arguments for the process.
	 *
	 * \note May return \c true even if the process cannot be started. In that case, trying to read from the process
	 * will fail with an error or EOF.
	 */
	bool spawn(native_string const& cmd, std::vector<native_string> const& args = std::vector<native_string>());

	/** \brief Stops the spawned process
	 *
	 * This function doesn't actually kill the process, it merely closes the pipes.
	 *
	 * Blocks until the process has quit.
	 */
	void kill();

	/** \brief Read data from process
	 *
	 * This function blocks
	 *
	 * \return >0 Number of octets read, can be less than requested
	 * \return 0 on EOF
	 * \return -1 on error.
	 */
	int read(char* buffer, unsigned int len);

	/** \brief Write data data process
	 *
	 * This function blocks
	 *
	 * \return true if all octets have been written.
	 * \return false on error.
	 */
	bool write(char const* buffer, unsigned int len);

	inline bool write(std::string const& s) {
		return write(s.c_str(), static_cast<unsigned int>(s.size()));
	}

private:
	class impl;
	impl* impl_;
};

}

#endif
