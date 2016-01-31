#ifndef LIBFILEZILLA_THREAD_HEADER
#define LIBFILEZILLA_THREAD_HEADER

#include "libfilezilla.hpp"

/** \file
 * \brief Declares \ref fz::thread "thread"
 */

namespace fz {

/** \brief Spawns and represents a new thread of execution
 *
 * This is a replacement of std::thread. Unfortunately std::thread isn't implemented
 * on all MinGW flavors. Most notably, MinGW as shipped by Debian Jessie does not
 * have std::thread.
 *
 * This class only supports joinable threads (see remark). You _MUST_ join threads
 * in the destructor of the outermost derived class. ~thread() calls std::abort()
 * if \c join has not previously been called.
 *
 * \remark Detached threads aren't implemented since they essentially race conditions
 * by design. You cannot use a detached thread and shutdown your program cleanly.
 */
class FZ_PUBLIC_SYMBOL thread
{
public:
	thread();

	/** \brief Calls \c std::abort if the thread has not been joined.
	 *
	 * To avoid race conditions, all threads need to be joined no later than
	 * in the destructor of the most derived class.
	 */
	virtual ~thread();

	/** \brief Start the thread.
	 *
	 * If a thread has already been started and not yet joined, this function fails.
	 */
	bool run();

	/** \brief Join the thread
	 *
	 * join blocks until the spawn thread has quit.
	 *
	 * You must call this at the latest in the destructor of the most-derived class.
	 *
	 * Must not be called from the spawned thread.
	 *
	 * After a successful join you can call \ref run again to spawn another thread.
	 */
	void join();

	/** \brief A thread is joinable after having been started and before it has been joined.
	 *
	 * Must not be called from the spawned thread.
	 */
	bool joinable() const;

protected:
	/// The thread's entry point, override in your derived class.
	virtual void entry() = 0;

private:
	class impl;
	friend class impl;
	impl* impl_{};
};

}

#endif
