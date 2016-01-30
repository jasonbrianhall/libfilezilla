#ifndef LIBFILEZILLA_EVENT_HEADER
#define LIBFILEZILLA_EVENT_HEADER

#include "libfilezilla.hpp"

#include <tuple>

/** \file
 * \brief Declares event_base and simple_event<>
 */

namespace fz {

/**
\brief Common base class for all events.

If possible, use simple_event<> below instead of deriving from event_base directly.

Keep events as simple as possible. Avoid mutexes in your events.
*/
class FZ_PUBLIC_SYMBOL event_base
{
public:
	event_base() = default;
	virtual ~event_base() {}

	event_base(event_base const&) = delete;
	event_base& operator=(event_base const&) = delete;

	/**
	The returned pointer must be unique for the derived type such that:
		event_base& a = ...
		event_base& b = ...
		assert((a.derived_type() == b.derived_type()) == (typeid(a) == typeid(b)));

	\warning Using &typeid is tempting, but unspecifined (sic)

	\warning According to the C++ standard, the address of a static member function is unique
		 for each type. Unfortunately this does not prevent optimizing compilers to pool identical
		 functions.

	Best solution is to have your derived type return the address of a static data member of it, as
	done in \ref fz::simple_event "simple_event".
	*/
	virtual void const* derived_type() const = 0;
};

/**
\brief This is the recommended event class.

Instanciate the template with a unique type to identify the type of the event and a number of types for the values.

Keep the values simple, in particular avoid mutexes in your values.

\see event_handler for usage example.
*/
template<typename UniqueType, typename...Values>
class simple_event final : public event_base
{
public:
	typedef UniqueType unique_type;
	typedef std::tuple<Values...> tuple_type;

	simple_event() = default;

	template<typename First_Value, typename...Remaining_Values>
	explicit simple_event(First_Value&& value, Remaining_Values&& ...values)
		: v_(std::forward<First_Value>(value), std::forward<Remaining_Values>(values)...)
	{
	}

	simple_event(simple_event const& op) = default;
	simple_event& operator=(simple_event const& op) = default;

	/// \brief Returns a unique pointer for the type such that can be used directly in derived_type.
	static void const* type() {
		static const char* f = 0;
		return &f;
	}

	/// \brief Simply returns \ref type()
	virtual void const* derived_type() const {
		return type();
	}

	tuple_type v_;
};

/// Used as lightweight RTTI alternative during \ref dispatch
/// \return true iff T& t = ...; t.derived_type() == ev.derived_type()
template<typename T>
bool same_type(event_base const& ev)
{
	return ev.derived_type() == T::type();
}

typedef unsigned long long timer_id;

/// \private
struct timer_event_type{};

/** \brief All timer events have this type.
 *
 * All timer events have one arguments of type \c timer_id which is the id of the timer that triggered.
 */
typedef simple_event<timer_event_type, timer_id> timer_event;

/// \private
/// This instantiation must be a public symbol
extern template class FZ_PUBLIC_SYMBOL simple_event<timer_event_type, timer_id>;

}

#endif
