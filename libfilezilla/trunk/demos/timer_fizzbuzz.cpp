#include <libfilezilla/event_handler.hpp>
#include <libfilezilla/time.hpp>

#include <iostream>
#include <string>

/// \file
/// \brief A simple demonstration of using timers
///
/// This example creates and event loop and starts two timers to
/// print fizz and buzz every 3 respective 5 seconds.
///
/// The user can also configure a third timer via stdin to print woof.

struct interval_change_type;
typedef fz::simple_event<interval_change_type, fz::duration> interval_change;

class fizzbuzz final : public fz::event_handler
{
public:
	fizzbuzz(fz::event_loop& loop)
		: fz::event_handler(loop)
	{
		// Start two periodic timers
		fizz_ = add_timer(fz::duration::from_seconds(3), false);
		buzz_ = add_timer(fz::duration::from_seconds(5), false);
	}

	virtual ~fizzbuzz()
	{
		// This _MUST_ be called to avoid a race so that operator()(fz::event_base const&) is not called on a partially destructed object.
		remove_handler();
	}

private:
	virtual void operator()(fz::event_base const& ev)
	{
		// Dispatch the event to the correct function.
		fz::dispatch<fz::timer_event, interval_change>(ev, this, &fizzbuzz::on_timer, &fizzbuzz::on_interval_change);
	}

	void on_interval_change(fz::duration interval)
	{
		stop_timer(woof_);
		woof_ = add_timer(interval, false);
	}

	void on_timer(fz::timer_id const& id)
	{
		if (id == fizz_) {
			std::cout << "fizz" << std::endl;
		}
		else if (id == buzz_) {
			std::cout << "buzz" << std::endl;
		}
		else if (id == woof_) {
			std::cout << "woof" << std::endl;
		}
	}

	fz::timer_id fizz_{};
	fz::timer_id buzz_{};
	fz::timer_id woof_{};
};

int main()
{
	// Start an event loop and add a handler.
	fz::event_loop loop;
	fizzbuzz fb(loop);

	std::cout << "Enter interval in seconds for the woof timer or 0 to stop program." << std::endl;

	int seconds;
	do {
		std::cin >> seconds;
		if (std::cin.good() && seconds > 0) {
			fb.send_event<interval_change>(fz::duration::from_seconds(seconds));
		}
	}
	while (std::cin.good() && seconds != 0);

	return 0;
}
