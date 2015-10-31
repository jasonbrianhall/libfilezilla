#include <libfilezilla/event_handler.hpp>

#include <iostream>
#include <string>
#include <vector>

// A condition that we'll use in this example to signal the main
// thread that the worker has processed the event.
fz::condition c;
fz::mutex m;


// Define a new event.
// The event is uniquely identified via the incomplete my_event_type struct and
// has two arguments: A string and a vector of ints.
struct my_event_type;
typedef fz::simple_event<my_event_type, std::string, std::vector<int>> my_event;

// A simple event handler
class handler final : public fz::event_handler
{
public:
	handler(fz::event_loop& l)
		: fz::event_handler(l)
	{}

	virtual ~handler()
	{
		// This _MUST_ be called to avoid a race so that operator()(fz::event_base const&) is not called on a partially destructed object.
		remove_handler();
	}

private:
	// The event loop calls this function for every event sent to this handler.
	virtual void operator()(fz::event_base const& ev)
	{
		// Dispatch the event to the correct function.
		fz::dispatch<my_event>(ev, this, &handler::on_my_event);
	}

	void on_my_event(std::string const& s, std::vector<int> const& v)
	{
		std::cout << "Received event with text \"" << s << "\" and a vector with " << v.size() << " elements" << std::endl;

		// Signal the condition
		fz::scoped_lock lock(m);
		c.signal(lock);
	}
};

int main()
{
	// Start an event loop
	fz::event_loop l;

	// Create a handler
	handler h(l);

	// Send an event to the handler
	h.send_event<my_event>("Hello World!", std::vector<int>{23, 42, 666});

	// Wait until a signal from the worker thread
	fz::scoped_lock lock(m);
	c.wait(lock);

	// All done.
	return 0;
}
