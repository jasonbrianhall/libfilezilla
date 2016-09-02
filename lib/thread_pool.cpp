#include "libfilezilla/thread_pool.hpp"
#include "libfilezilla/thread.hpp"

#include <assert.h>

namespace fz {

class pooled_thread_impl final : public thread
{
public:
	pooled_thread_impl(thread_pool & pool)
		: m_(pool.m_)
		, pool_(pool)
	{}

	virtual ~pooled_thread_impl()
	{
		join();
	}

	virtual void entry() {
		scoped_lock l(m_);
		while (!quit_) {
			thread_cond_.wait(l);

			if (f_) {
				l.unlock();
				f_();
				l.lock();
				task_cond_.signal(l);
			}
		}
	}

	void quit(scoped_lock & l)
	{
		quit_ = true;
		thread_cond_.signal(l);
	}

	std::function<void()> f_{};
	mutex & m_;
	condition thread_cond_;
	condition task_cond_;
	thread_pool& pool_;

private:
	bool quit_{};
};


async_task::async_task(async_task && other) noexcept
{
	std::swap(impl_, other.impl_);
}

async_task& async_task::operator=(async_task && other) noexcept
{
	std::swap(impl_, other.impl_);
	return *this;
}

async_task::~async_task()
{
	join();
}


void async_task::join()
{
	if (impl_) {
		scoped_lock l(impl_->m_);
		impl_->task_cond_.wait(l);
		impl_->f_ = std::function<void()>();
		impl_->pool_.idle_.push_back(impl_);
		impl_ = 0;
	}
}



thread_pool::thread_pool()
{
}

thread_pool::~thread_pool()
{
	std::vector<pooled_thread_impl*> threads;
	{
		scoped_lock l(m_);
		for (auto thread : threads_) {
			thread->quit(l);
		}
		threads.swap(threads_);
	}

	for (auto thread : threads) {
		delete thread;
	}
}

async_task thread_pool::spawn(std::function<void()> const& f)
{
	async_task ret;

	scoped_lock l(m_);

	pooled_thread_impl *t{};
	if (idle_.empty()) {
		t = new pooled_thread_impl(*this);
		if (!t->run()) {
			delete t;
			return ret;
		}
		threads_.push_back(t);
	}
	else {
		t = idle_.back();
		idle_.pop_back();
	}

	ret.impl_ = t;
	t->f_ = f;
	t->thread_cond_.signal(l);

	return ret;
}

}
