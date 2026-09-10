#ifndef _AB_HANDLER
#define _AB_HANDLER

#include <ablib.h>

struct MasterState {
  struct {
    bool mute;
    int volume;
    int moduleId;
    int requestId;
    long requestContext;
    long started;
  } sound;
};

typedef void (*Processor)(MasterState* state);
typedef void (*RequestHandler)(MasterState* state, MasterRequest* request);

#endif
