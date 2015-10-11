#ifndef LIBFILEZILLA_MUTEX_HEADER
#define LIBFILEZILLA_MUTEX_HEADER

/** \file
 * \brief Thread synchronization primitives: mutex, scoped_lock and condition
 */
#include "libfilezilla.hpp"
#include "time.hpp"

#ifdef FZ_WINDOWS
#include "private/windows.hpp"
#else
#include <pthread.h>
#endif

namespace fz {

/**
 * \brief Lean replacement for std::(recursive_)mutex
 *
 * Unfortunately we can't use std::mutex and std::condition_variable as MinGW doesn't implement
 * C++11 threading yet. Even in those variants that at least mutexes and condition variables, they doen't
 * make use of Vista+'s CONDITION_VARIABLE, so it's too slow for our needs.
 *
 * \note Once all platforms have C++11 threading implemented _in the fastest way possible_, this class will be removed.
 */
class FZ_PUBLIC_SYMBOL mutex final
{
public:
	explicit mutex(bool recursive = true);
	~mutex();

	mutex(mutex const&) = delete;
	mutex& operator=(mutex const&) = delete;

	/// Beware, manual locking isn't exception safe, use scoped_lock
	void lock();

	/// Beware, manual locking isn't exception safe, use scoped_lock
	void unlock();

private:
	friend class condition;
	friend class scoped_lock;

#ifdef FZ_WINDOWS
	CRITICAL_SECTION m_;
#else
	pthread_mutex_t m_;
#endif
};

/** \brief A simple scoped lock.
 *
 * The lock is aquired on construction and, if still locked, released on destruction.
 * You can manually unlock and re-lock if needed.
 *
 * \note While this can be used with recursive mutexes, scoped_lock does not implement reference
 * counting.
 */
class FZ_PUBLIC_SYMBOL scoped_lock final
{
public:
	explicit scoped_lock(mutex& m)
		: m_(&m.m_)
	{
#ifdef FZ_WINDOWS
		EnterCriticalSection(m_);
#else
		pthread_mutex_lock(m_);
#endif
	}

	~scoped_lock()
	{
		if (locked_) {
#ifdef FZ_WINDOWS
			LeaveCriticalSection(m_);
#else
			pthread_mutex_unlock(m_);
#endif
		}

	}

	scoped_lock(scoped_lock const&) = delete;
	scoped_lock& operator=(scoped_lock const&) = delete;

	/** \brief Obtains the mutex.
	 *
	 * Locking an already locked scoped_lock results in undefined behavior.
	 */
	void lock()
	{
		locked_ = true;
#ifdef FZ_WINDOWS
		EnterCriticalSection(m_);
#else
		pthread_mutex_lock(m_);
#endif
	}

	/** \brief Releases the mutex.
	 *
	 * Releasing a scoped_lock that isn't locked results in undefined behavior.
	 */
	void unlock()
	{
		locked_ = false;
#ifdef FZ_WINDOWS
		LeaveCriticalSection(m_);
#else
		pthread_mutex_unlock(m_);
#endif
	}

private:
	friend class condition;

#ifdef FZ_WINDOWS
	CRITICAL_SECTION * const m_;
#else
	pthread_mutex_t * const m_;
#endif
	bool locked_{true};
};

/** \brief Waitable condition variable
 *
 * Allows one thread to wait for the condition variable to become signalled by another thread.
 */
class FZ_PUBLIC_SYMBOL condition final
{
public:
	condition();
	~condition();

	condition(condition const&) = delete;
	condition& operator=(condition const&) = delete;

	/** \brief Wait indefinitely for condition to become signalled.
	 *
	 * Atomically unlocks the mutex and waits for the condition. Atomically consumes the signal and re-locks the mutex.
	 *
	 * \note The lock must have be on the same mutex that is used for both signalling and for waiting.
	 */
	void wait(scoped_lock& l);

	/** \brief Wait until timeout for condition to become signalled.
	 *
	 * Atomically unlocks the mutex and waits for the condition. Atomically consumes the signal and re-locks the mutex.
	 *
	 * \return true if the condition has been signalled.
	 * \return false if the condition could not be obtained before the timeout has elapsed.
	 *
	 * \note Spurious signals are a rare possibility.
	 *
	 * \note The lock must have be on the same mutex that is used for both signalling and for waiting.
	 */
	bool wait(scoped_lock& l, duration const& timeout);

	/** \brief Signal condition variable
	 *
	 * To avoid race conditions leading to lost signals, you must pass
	 * a locked mutex.
	 *
	 * \note Spurious signals are a rare possibility.
	 *
	 * \note The lock must have be on the same mutex that is used for both signalling and for waiting.
	 */
	void signal(scoped_lock& l);

	/** \brief Check if condition is already signalled
	 *
	 * To avoid race conditions leading to lost signals, you must pass
	 * a locked mutex.
	 *
	 * \note The lock must have be on the same mutex that is used for both signalling and for waiting.
	 */
	bool signalled(scoped_lock const&) const { return signalled_; }
private:
#ifdef FZ_WINDOWS
	CONDITION_VARIABLE cond_;
#else
	pthread_cond_t cond_;
#endif
	bool signalled_{};
};

}
#endif
