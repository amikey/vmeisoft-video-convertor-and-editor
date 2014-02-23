#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>

extern "C"
{

#define _FILE_OFFSET_BITS	64
#define _GNU_SOURCE			1
#define _ISOC99_SOURCE 		1
#define _LARGEFILE_SOURCE	1
#define _THREAD_SAFE 		1
#define HAVE_AV_CONFIG_H	1
#define PIC					1

#include "config.h"

#undef HAVE_INLINE_ASM
#undef HAVE_MMX_INLINE
#undef _MSC_VER
#define inline __inline
#define __attribute__(x)

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/avstring.h"
#include "libavutil/colorspace.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavcodec/audioconvert.h"
#include "libavcodec/avfft.h"
#include "libavformat/internal.h"

#define _MSC_VER _MSC_VER

};


