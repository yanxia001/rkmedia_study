#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "common.h"
#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libavcodec/avcodec.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
void ai_set();
void ao_set();
void ai_to_ao();
void aenc_ai_set();
void ai_to_aenc();
void aenc_call();
void adec_set();
void adec_to_ao();
void test();
void ai_tofengzhuang_aenc();
void aac_dec_ao();
#endif // DEBUG