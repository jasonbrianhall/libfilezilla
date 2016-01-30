#include "libfilezilla/event.hpp"

namespace fz {

/// \private
/// This instantiation must be a public symbol
template class simple_event<timer_event_type, timer_id>;

}

