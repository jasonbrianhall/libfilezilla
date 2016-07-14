#ifndef LIBFILEZILLA_EVENT_HANDLER
#define LIBFILEZILLA_EVENT_HANDLER

#include "event_loop.hpp"

/** \file
 * \brief Declares the \ref fz::event_handler "event_handler" class.
 */
namespace fz {

/**
\brief Simple handler for asynchronous event processing.

Usage example:
\code
	struct foo_event_type{}; // Any uniquely named type that's not implicitly convertible
	typedef fz::simple_event<foo_event_type, int, std::string> foo_event;

	struct bar_event_type{};
	typedef fz::simple_event<bar_event_type> bar_event;

	class my_handler final : public fz::event_handler
	{
	public:
		my_handler(fz::event_loop& loop)
			: fz::event_handler(loop)
		{}

		virtual ~my_handler()
		{
			// It is imperative to call remove_handler
			remove_handler();
		}

		void foo(int v, std::string const& s) {
			std::cout << "foo called with:" << s << v;
		}

		void bar() {
			std::cout << "bar called";
		}

		virtual void operator()(event_base const& ev) {
			// Tip: Put in order of decreasing frequency
			fz::dispatch<foo_event, bar_event>(ev, this, &my_handler::foo, &my_handler::bar);
		}
	};

	fz::event_loop loop;
	my_handler h(loop);
	h.SendEvent<foo_event>(42, "Don't Panic");
\endcode
*/
class FZ_PUBLIC_SYMBOL event_handler
{
public:
	event_handler() = delete;

	explicit event_handler(event_loop& loop);
	virtual ~event_handler();

	event_handler(event_handler const&) = delete;
	event_handler& operator=(event_handler const&) = delete;

	/** \brief Deactivates handler, removes all pending events and stops all timers for this handler.
	 *
	 * When function returns, handler is not in its callback anymore.
	 *
	 * \warning You _MUST_ call remove_handler no later than inside the destructor of the most derived class.
	 *
	 *  This may deadlock if a handler removes itself inside its own callback.
	 */
	void remove_handler();

	/** \brief Called by the event loop in the worker thread with the event to process
	 *
	 * Override in your derived class.
	 * 
	 * Consider using \ref dispatch inside your function.
	 */
	virtual void operator()(event_base const&) = 0;

	/* \brief Sends the passed event asynchronously to the handler.
	 *
	 * Can be called from any thread.
	 *
	 * All events are processed in the order they are sent, excluding possible races between threads.
	 */
	template<typename T, typename... Args>
	void send_event(Args&&... args) {
		event_loop_.send_event(this, new T(std::forward<Args>(args)...));
	}

	template<typename T>
	void send_event(T* evt) {
		event_loop_.send_event(this, evt);
	}

	/** \brief Adds a timer, returns the timer id.
	 *
	 * Once the interval expires, you get a timer event from the event loop.
	 *
	 * One-shot timers are deleted automatically
	 *
	 * For periodic timers, the next event is scheduled right before the callback is called. If multiple
	 * intervals expire before the timer fires, e.g. under heavy load, only one event is sent.
	 *
	 * If multiple different timers have expired, the order in which the callbacks are executed is unspecified,
	 * there is no fairness guarantee.
	 *
	 * Timers take precedence over other queued events.
	 *
	 * \note High-frequency timers doing heavy processing can starve other timers and queued events.
	 */
	timer_id add_timer(duration const& interval, bool one_shot);

	/** Stops the given timer.
	 *
	 * One-shot timers that have fired stop automatically and do not need to be stopped.
	 */
	void stop_timer(timer_id id);

	event_loop & event_loop_;
private:
	friend class event_loop;
	bool removing_{};
};

/** \brief Dispatch for simple_event<> based events to simple functors
 *
 * \tparam T the event type, a simple_event<> instantiation
 *
 * \param ev the received event
 * \param f functor that should be called if the event matches the passed type.
 *
 * If the passed event is of the type passed as template argument, the passed function is called with the contents of the event
 * unpacked as arguments.
 *
 * \return true iff event matches passed type.
 *
 * \sa event_handler for complete usage example.
 */
template<typename T, typename F>
bool dispatch(event_base const& ev, F&& f)
{
	bool const same = same_type<T>(ev);
	if (same) {
		T const* e = static_cast<T const*>(&ev);
		apply(std::forward<F>(f), e->v_);
	}
	return same;
}

/** \brief Dispatch for simple_event<> based events to pointer to member
 *
 * \tparam T the event type, a simple_event<> instantiation
 *
 * \param ev the received event.
 * \param h object whose member gets called if the event matches the passed type.
 * \param f pointer to member of \c h that should be called if the event matches the passed type.
 *
 * If the passed event is of the type passed as template argument, the passed function is called with the contents of the event
 * unpacked as arguments.
 *
 * \return true iff event matches passed type.
 *
 * \sa event_handler for complete usage example.
 */
template<typename T, typename H, typename F>
bool dispatch(event_base const& ev, H* h, F&& f)
{
	bool const same = same_type<T>(ev);
	if (same) {
		T const* e = static_cast<T const*>(&ev);
		apply(h, std::forward<F>(f), e->v_);
	}
	return same;
}

/** \brief Compound dispatch for simple_event<> based events
 *
 * Calls the simple dispatch for each passed type and tries the next one if it didn't match.
 *
 * Order the passed types in decreasing usage frequency for maximum performance.
 *
 * \tparam T the event type, a simple_event<> instantiation
 * \tparam Ts additional event types
 *
 * \param ev the received event.
 * \param h object whose member gets called if the event matches the passed type.
 * \param f pointer to member of \c h that should be called if the event matches the passed type.
 * \param fs additional pairs of objects and pointers to members.
 *
 * \return true iff event matched a passed type.
 *
 * \sa event_handler for complete usage example.
 */
template<typename T, typename ... Ts, typename H, typename F, typename ... Fs>
bool dispatch(event_base const& ev, H* h, F&& f, Fs&& ... fs)
{
	if (dispatch<T>(ev, h, std::forward<F>(f))) {
		return true;
	}

	return dispatch<Ts...>(ev, h, std::forward<Fs>(fs)...);
}

}

#endif
