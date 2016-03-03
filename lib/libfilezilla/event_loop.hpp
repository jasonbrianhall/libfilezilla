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

/** \file
 * \brief A simple threaded event loop for the typesafe event system
 */

namespace fz {

class event_handler;

/** \brief A threaded event loop that supports sending events and timers
 *
 * Timers have precedence over queued events. Too many or too frequent timers can starve processing queued events.
 *
 * If the deadlines of multiple timers have expired, they get processed in an unspecified order.
 *
 * \sa event_handler for a complete usage example.
 */
class FZ_PUBLIC_SYMBOL event_loop final : private thread
{
public:
	typedef std::deque<std::pair<event_handler*, event_base*>> Events;

	/// Spawns a thread and starts the loop
	event_loop();

	/// Stops the thread
	virtual ~event_loop();

	event_loop(event_loop const&) = delete;
	event_loop& operator=(event_loop const&) = delete;

	/** \brief Allows filtering of queued events
	 *
	 * Puts all queued events through the filter function.
	 * The filter function can freely change the passed events.
	 * If the filter function returns true, the corresponding event
	 * gets removed.
	 *
	 * The filter function must not call any function of event_loop.
	 *
	 * Filtering events is a blocking operation and temporarily pauses the loop.
	 */
	void filter_events(std::function<bool (Events::value_type&)> const& filter);

	/** \brief Stops the loop
	 *
	 * Stops the event loop. It is automatically called by the destructor.
	 */
	void stop();

private:
	friend class event_handler;

	void FZ_PRIVATE_SYMBOL remove_handler(event_handler* handler);

	timer_id FZ_PRIVATE_SYMBOL add_timer(event_handler* handler, duration const& interval, bool one_shot);
	void FZ_PRIVATE_SYMBOL stop_timer(timer_id id);

	void send_event(event_handler* handler, event_base* evt);

	// Process the next (if any) event. Returns true if an event has been processed
	bool FZ_PRIVATE_SYMBOL process_event(scoped_lock & l);

	// Process timers. Returns true if a timer has been triggered
	bool FZ_PRIVATE_SYMBOL process_timers(scoped_lock & l, monotonic_clock& now);

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

}
#endif
