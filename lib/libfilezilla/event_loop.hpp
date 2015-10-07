#ifndef LIBFILEZILLA_EVENT_LOOP_HEADER
#define LIBFILEZILLA_EVENT_LOOP_HEADER

#include "apply.hpp"
#include "event.hpp"
#include "mutex.hpp"
#include "time.hpp"
#include "thread.hpp"

#include <deque>
#include <functional>
#include <vector>

namespace fz {

class event_handler;

// Timers have precedence over queued events. Too many or too frequent timers can starve processing queued events.
// If the deadline of multiple timers have expired, they get processed in an unspecified order
class FZ_PUBLIC_SYMBOL event_loop final : private thread
{
public:
	typedef std::deque<std::pair<event_handler*, event_base*>> Events;

	// Spawns a thread
	event_loop();
	virtual ~event_loop();

	event_loop(event_loop const&) = delete;
	event_loop& operator=(event_loop const&) = delete;

	// Puts all queued events through the filter function.
	// The filter function can freely change the passed events.
	// If the filter function returns true, the corresponding event
	// gets removed.
	void filter_events(std::function<bool (Events::value_type&)> const& filter);

private:
	friend class event_handler;

	void FZ_PRIVATE_SYMBOL remove_handler(event_handler* handler);

	timer_id FZ_PRIVATE_SYMBOL add_timer(event_handler* handler, duration const& interval, bool one_shot);
	void FZ_PRIVATE_SYMBOL stop_timer(timer_id id);

	void send_event(event_handler* handler, event_base* evt);

	// Process the next (if any) event. Returns true if an event has been processed
	bool FZ_PRIVATE_SYMBOL process_event(scoped_lock & l);

	// Process timers. Returns true if a timer has been triggered
	bool FZ_PRIVATE_SYMBOL process_timers(scoped_lock & l, monotonic_clock const& now);

	virtual void FZ_PRIVATE_SYMBOL entry();

	struct timer_data final
	{
		event_handler* handler_{};
		timer_id id_{};
		monotonic_clock deadline_;
		duration interval_{};
	};

	typedef std::vector<timer_data> Timers;

	Events pending_events_;
	Timers timers_;

	mutex sync_;
	condition cond_;

	bool quit_{};

	event_handler * active_handler_{};


	monotonic_clock deadline_;

	timer_id next_timer_id_{};
};


/// Dispatch for simple_event<> based events.
/// \see event_handler for additional information.
template<typename T, typename H, typename F>
bool dispatch(event_base const& ev, F&& f)
{
	bool const same = same_type<T>(ev);
	if (same) {
		T const* e = static_cast<T const*>(&ev);
		apply(std::forward<F>(f), e->v_);
	}
	return same;
}


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
