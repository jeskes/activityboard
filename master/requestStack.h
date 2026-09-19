#ifndef _AB_REQUEST_STACK
#define _AB_REQUEST_STACK

#include <ablib.h>

void pushRequest(MasterRequest request);
bool isRequestAvailable();
int countRequests();
MasterRequest pullRequest();

#endif
