#define TRACEPOINT_CREATE_PROBES
#define TRACEPOINT_DEFINE
#include "tp.h"
int main() { tracepoint(providername, tracepointname, 0); }
