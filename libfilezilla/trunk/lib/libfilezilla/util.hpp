#ifndef LIBFILEZILLA_UTIL_HEADER
#define LIBFILEZILLA_UTIL_HEADER

#include "libfilezilla.hpp"
#include "time.hpp"

#include <cstdint>

/** \file
 * \brief Various utility functions
 */

namespace fz {

/** \brief Sleep current thread for the specified \ref duration.
 *
 * Alternative to \c std::this_thread::sleep_for which unfortunately isn't implemented on
 * MinGW.
 *
 * \note May wake up early, e.g. due to a signal. You can use \ref monotonic_clock
 * to check elapsed time and sleep again if needed.
 */
void FZ_PUBLIC_SYMBOL sleep(duration const& d);

/** \brief Get a secure random integer uniformly distributed in the closed interval [min, max]
 */
int64_t FZ_PUBLIC_SYMBOL random_number(int64_t min, int64_t max);

}

#endif
