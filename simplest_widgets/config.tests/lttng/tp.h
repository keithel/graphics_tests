#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER providername

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./tp.h"

#if !defined(_TP_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _TP_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(providername, tracepointname
                 ,TP_ARGS(int, intarg)
                 ,TP_FIELDS(ctf_integer(int, intfield, intarg)))

#endif /* _TP_H */

#include <lttng/tracepoint-event.h>

