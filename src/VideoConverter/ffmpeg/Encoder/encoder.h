#pragma once
#include <stdint.h>
#include "../../VideoConverter.h"

class CEncoder;
extern CEncoder* CreateEncoder(wchar_t *filename, volatile int64_t *time, volatile long *abort_addr, VideoStream* vStream, struct AudioStream *aStream);
extern void DestroyEncoder(CEncoder *enc);

