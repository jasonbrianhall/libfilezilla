#ifndef LIBFILEZILLA_TIME_HEADER
#define LIBFILEZILLA_TIME_HEADER

#include "libfilezilla.hpp"

#include <chrono>
#include <ctime>

#include <limits>

#ifdef FZ_WINDOWS
#include "private/windows.hpp"
#endif

/** \file
 * \brief Assorted classes dealing with time.
 */

namespace fz {

class FZ_PUBLIC_SYMBOL duration;

/**
\brief Represents a point of time in wallclock, tracking the timestamps accuracy/precision.

As time may come from different sources that have different accuracy/precision,
this class keeps track of accuracy information.

For example, your local filesystem might store timestamps with milliseconds, whereas a given network protocol
might only offer seconds. You can use \ref compare to easily check whether the timestamp of a local file is
equivalent to a timestamp received via the protocol.

The internal representation of \c datetime is in milliseconds since 1970-01-01 00:00:00.000 UTC and can handle a
range of several million years. While datetime supports negative times (i.e. earlier than 1970-01-01 00:00:00.000 UTC),
the underlying plaform may not support it.

\remark Some *nix systems base their time on TAI instead of UTC, though we pretend there
is no difference, as the latter is the default on every modern distribution.
*/
class FZ_PUBLIC_SYMBOL datetime final
{
public:
	/**
	 * \brief The datetime's accuracy.
	 */
	enum accuracy : char {
		days,
		hours,
		minutes,
		seconds,
		milliseconds
	};

	/**
	 * \brief When importing or exporting a timestamp, zone is used to explicitly specify whether the
	 * conversion is to/from localtime or UTC.
	 */
	enum zone {
		utc,
		local
	};

	/// A default-constructed timestamp is \ref empty()
	datetime() = default;

	datetime(zone z, int year, int month, int day, int hour = -1, int minute = -1, int second = -1, int millisecond = -1);

	explicit datetime(time_t, accuracy a);

	/**
	 * \brief Construct from string, looks for \c YYYYmmDD[[[[HH]MM]SS]sss]
	 * \see bool set(std::string const& str, zone z)
	 */
	explicit datetime(std::string const& s, zone z);
	explicit datetime(std::wstring const& s, zone z);

#ifdef FZ_WINDOWS
	/// Windows-only: Construct from FILETIME
	explicit datetime(FILETIME const& ft, accuracy a);
#endif

	datetime(datetime const& op) = default;
	datetime(datetime && op) noexcept = default;
	datetime& operator=(datetime const& op) = default;
	datetime& operator=(datetime && op) noexcept = default;

	/// \return \c true if no timestamp has been set
	bool empty() const;

	/// Resulting timestamp is empty()
	void clear();

	accuracy get_accuracy() const { return a_; }

	/// Returns the current date/time
	static datetime now();

	/**
	 * \name Naive comparison operators
	 * Naive operators compare the timestamp's internal representation first, with accuracy as secondary criterion.
	 * \sa compare
	 * \{
	 */
	bool operator==(datetime const& op) const;
	bool operator!=(datetime const& op) const { return !(*this == op); }
	bool operator<(datetime const& op) const;
	bool operator<=(datetime const& op) const;
	bool operator>(datetime const& op) const { return op < *this; }
	/// \}

	/**
	 * \brief Accuracy-aware comparison against another timestamp.
	 *
	 * Conceptually it works as if naively comparing both timestamps after
	 * truncating/clamping them to the least common accuracy.
	 *
	 * \return 0 if timestamps are equivalent,\n <0 if own timestamp is earlier than
	 * the argument,\n >0 if own timestamp is late than the argument.
	 */
	int compare(datetime const& op) const;

	/// Equivalent to compare(op) < 0
	bool earlier_than(datetime const& op) const { return compare(op) < 0; };

	/// Equivalent to compare(op) > 0
	bool later_than(datetime const& op) const { return compare(op) > 0; };

	/** \name Adding/subtracting duration intervals
	 *
	 * Adding or subracting a \ref duration interval is accuracy-aware, e.g. adding a single second to a datetime with
	 * minute-accuracy does not change the timestamp.
	 */
	datetime& operator+=(duration const& op);
	datetime operator+(duration const& op) const { datetime t(*this); t += op; return t; }

	datetime& operator-=(duration const& op);
	datetime operator-(duration const& op) const { datetime t(*this); t -= op; return t; }
	/// \}

	friend duration FZ_PUBLIC_SYMBOL operator-(datetime const& a, datetime const& b);

	/** \brief Sets the timestamp
	 * \param month 1-indexed as on a calender, e.g. February is 2
	 * \param day 1-indexed as on a calender
	 * \param hour 0-indexed as on a 24h clock
	 *
	 * \return whether setting the timestamp succeeded. On failure object gets cleared
	 */
	bool set(zone z, int year, int month, int day, int hour = -1, int minute = -1, int second = -1, int millisecond = -1);

	/**
	 * \brief Set from string, looks for \c YYYYmmDD[[[[HH]MM]SS]sss]
	 *
	 * Ignores all non-digit characters between fields. For example 20140720T165432 is the same as 2014-07-20 16:54:32
	 *
	 * \ref accuracy is set according to the fields present in the string.
	 *
	 * \return whether setting the timestamp succeeded. On failure object gets cleared
	 */
	bool set(std::string const& str, zone z);
	bool set(std::wstring const& str, zone z);

#ifdef FZ_WINDOWS
	/// Windows-only: Set timestamp from FILETIME
	bool set(FILETIME const& ft, accuracy a);
	/// Windows-only: Set timestamp from SYSTEMTIME
	bool set(SYSTEMTIME const& ft, accuracy a, zone z);
#endif

#if defined(FZ_UNIX) || defined(FZ_MAC)
	/** Sets timestamp from struct tm.
	 * \note Not available on Windows.
	 * \warning modifies passed structure
	 * \returns \c true on success, \c false on failure. The object is empty after failure.
	 */
	bool set(tm & t, accuracy a, zone z);
#endif

	/** \brief Adds time to timestamps that only have a day-accuracy.
	 *
	 * Changes accuracy based on given parameters. Use -1 in parameters that aren't used.
	 *
	 * \return Whether the function succeeded. On failure object remains unchanged.
	 */
	bool imbue_time(int hour, int minute, int second = -1, int millisecond = -1);

	/** Format time as string
	 *
	 * \param format is a format string as understood by ::strftime
	 */
	std::string format(std::string const& format, zone z) const;
	std::wstring format(std::wstring const& format, zone z) const;

	/** Verify format strings
	 *
	 * \param fmt the string that is to be tested
	 * \return whether the passed string is a format string understood by ::strftime
	 */
	static bool verify_format(std::string const& fmt);
	static bool verify_format(std::wstring const& fmt);

	/// Get millisecond part of timestamp
	int get_milliseconds() const { return t_ % 1000; }

	/// Get timestamp as time_t, seconds since 1970-01-01 00:00:00
	time_t get_time_t() const;

	/// Get timestamp as struct tm
	tm get_tm(zone z) const;

#ifdef FZ_WINDOWS
	/// Windows-only: Get timestamp as FILETIME
	FILETIME get_filetime() const;
#endif

private:
	int FZ_PRIVATE_SYMBOL compare_slow(datetime const& op) const;

	bool FZ_PRIVATE_SYMBOL clamped();

	enum invalid_t : int64_t {
		invalid = std::numeric_limits<int64_t>::min()
	};

	int64_t t_{invalid};
	accuracy a_{days};
};

/** \brief The \c duration class represents a time interval in milliseconds.
 *
 * Constructing a non-empty duration is only possible using the static setters which
 * have the time unit as part of the function name.
 *
 * In contract to \ref datetime, \c duration does not track accuracy.
 */
class FZ_PUBLIC_SYMBOL duration final
{
public:
	duration() = default;

	/** \name Getters
	 * All getters return the total time of the duration, rounded down to the requested granularity.
	 * \{
	 */
	int64_t get_days() const { return ms_ / 1000 / 3600 / 24; }
	int64_t get_hours() const { return ms_ / 1000 / 3600; }
	int64_t get_minutes() const { return ms_ / 1000 / 60; }
	int64_t get_seconds() const { return ms_ / 1000; }
	int64_t get_milliseconds() const { return ms_; }
	/// \}

	static duration from_days(int64_t m) {
		return duration(m * 1000 * 60 * 60 * 24);
	}
	static duration from_hours(int64_t m) {
		return duration(m * 1000 * 60 * 60);
	}
	static duration from_minutes(int64_t m) {
		return duration(m * 1000 * 60);
	}
	static duration from_seconds(int64_t m) {
		return duration(m * 1000);
	}
	static duration from_milliseconds(int64_t m) {
		return duration(m);
	}
	/// \}

	duration& operator-=(duration const& op) {
		ms_ -= op.ms_;
		return *this;
	}

	duration operator-() const {
		return duration(-ms_);
	}

	explicit operator bool() const {
		return ms_ != 0;
	}

	bool operator<(duration const& op) const { return ms_ < op.ms_; }
	bool operator<=(duration const& op) const { return ms_ <= op.ms_; }
	bool operator>(duration const& op) const { return ms_ > op.ms_; }
	bool operator>=(duration const& op) const { return ms_ >= op.ms_; }

	friend duration FZ_PUBLIC_SYMBOL operator-(duration const& a, duration const& b);
private:
	explicit duration(int64_t ms) : ms_(ms) {}

	int64_t ms_{};
};

inline duration operator-(duration const& a, duration const& b)
{
	return duration(a) -= b;
}


/** \relates datetime
 * \relatesalso duration
 * \brief Gets the difference between two timestamps as \ref duration
 *
 * This function ignores accuracy, it treats both timestamps as if they had millisecond-accuracy.
 */
duration FZ_PUBLIC_SYMBOL operator-(datetime const& a, datetime const& b);




/** \brief A monotonic clock (aka steady clock) is independent from walltime.
 *
 * In particular, while wallclock might jump forward and backward (e.g. due to DST),
 * the monotonic clock ticks steadily forward at always the same pace.
 *
 * \c monotonic_clock is a convenience wrapper around std::chrono::steady_clock.
 */
class FZ_PUBLIC_SYMBOL monotonic_clock final
{
public:
	/** \brief Constructs empty clock.
	 *
	 * Comparisons with empty clocks and adding durations to an empty clocks are undefined.
	 */
	monotonic_clock() = default;

	monotonic_clock(monotonic_clock const&) = default;
	monotonic_clock(monotonic_clock &&) noexcept = default;
	monotonic_clock& operator=(monotonic_clock const&) = default;
	monotonic_clock& operator=(monotonic_clock &&) noexcept = default;

	monotonic_clock const operator+(duration const& d) const
	{
		return monotonic_clock(*this) += d;
	}

private:
	typedef std::chrono::steady_clock clock_type;
	static_assert(std::chrono::steady_clock::is_steady, "Nonconforming stdlib, your steady_clock isn't steady");

public:
	/// Gets the current point in time time
	static monotonic_clock now() {
		return monotonic_clock(clock_type::now());
	}

	explicit operator bool() const {
		return t_ != clock_type::time_point();
	}

	monotonic_clock& operator+=(duration const& d)
	{
		t_ += std::chrono::milliseconds(d.get_milliseconds());
		return *this;
	}

	monotonic_clock& operator-=(duration const& d)
	{
		t_ -= std::chrono::milliseconds(d.get_milliseconds());
		return *this;
	}

private:
	explicit monotonic_clock(clock_type::time_point const& t)
		: t_(t)
	{}

	clock_type::time_point t_;

	friend duration operator-(monotonic_clock const& a, monotonic_clock const& b);
	friend bool operator==(monotonic_clock const& a, monotonic_clock const& b);
	friend bool operator<(monotonic_clock const& a, monotonic_clock const& b);
	friend bool operator<=(monotonic_clock const& a, monotonic_clock const& b);
	friend bool operator>(monotonic_clock const& a, monotonic_clock const& b);
	friend bool operator>=(monotonic_clock const& a, monotonic_clock const& b);
};

/** \relates monotonic_clock
 * \relatesalso duration
 * Gets the difference between two clocks as \ref duration
 */
inline duration operator-(monotonic_clock const& a, monotonic_clock const& b)
{
	return duration::from_milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(a.t_ - b.t_).count());
}

/// \relates monotonic_clock
inline bool operator==(monotonic_clock const& a, monotonic_clock const& b)
{
	return a.t_ == b.t_;
}

/// \relates monotonic_clock
inline bool operator<(monotonic_clock const& a, monotonic_clock const& b)
{
	return a.t_ < b.t_;
}

/// \relates monotonic_clock
inline bool operator<=(monotonic_clock const& a, monotonic_clock const& b)
{
	return a.t_ <= b.t_;
}

/// \relates monotonic_clock
inline bool operator>(monotonic_clock const& a, monotonic_clock const& b)
{
	return a.t_ > b.t_;
}

/// \relates monotonic_clock
inline bool operator>=(monotonic_clock const& a, monotonic_clock const& b)
{
	return a.t_ >= b.t_;
}

}

#endif
