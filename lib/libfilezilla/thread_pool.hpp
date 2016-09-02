#ifndef LIBFILEZILLA_THREAD_POOL_HEADER
#define LIBFILEZILLA_THREAD_POOL_HEADER

#include "libfilezilla.hpp"
#include "mutex.hpp"

#include <functional>
#include <memory>
#include <vector>

/** \file
 * \brief Declares \ref fz::thread_pool "thread_pool" and \ref fz::async_task "async_task"
 */

namespace fz {

class thread_pool;
class pooled_thread_impl;

/** \brief Handle for asynchronous tasks
 */
class FZ_PUBLIC_SYMBOL async_task final {
public:
	async_task() = default;
	~async_task();

	async_task(async_task const&) = delete;
	async_task& operator=(async_task const&) = delete;

	async_task(async_task && other) noexcept;
	async_task& operator=(async_task && other) noexcept;

	/// Wait for the task to finish
	void join();

	/// Check whether it's a spawned, unjoined task.
	explicit operator bool() const { return impl_ != 0; }

private:
	friend class thread_pool;
	friend class pooled_thread_impl;

	pooled_thread_impl* impl_{};
};

/** \brief A dumb thread-pool for asynchronous tasks
 *
 * If there are no idle threads, threads are created on-demand if spawning an asynchronous task.
 * Once an asynchronous task finishes, the corresponding thread is kept idle until the pool is
 * destroyed.
 *
 * Any number of tasks can be run concurrently.
 */
class FZ_PUBLIC_SYMBOL thread_pool final
{
public:
	thread_pool();
	~thread_pool();

	thread_pool(thread_pool const&) = delete;
	thread_pool& operator=(thread_pool const&) = delete;

	/// Spawns a new asynchronous task.
	async_task spawn(std::function<void()> const& f);

private:
	friend class async_task;
	friend class pooled_thread_impl;

	std::vector<pooled_thread_impl*> threads_;
	std::vector<pooled_thread_impl*> idle_;
	mutex m_{false};
};

}

#endif
