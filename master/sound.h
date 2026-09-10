#ifndef _AB_SOUND
#define _AB_SOUND

void initializeSound(MasterState* state);

void processSound(MasterState* state);
void handlePlaySoundRequest(MasterState* state, MasterRequest* request);
void handleStopSoundRequest(MasterState* state, MasterRequest* request);

#endif