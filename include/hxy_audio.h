#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "common.h"
extern "C"
{
#include <stdio.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 6556
#define IP "0.0.0.0" //表示当前设备所有网卡 IP
}
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
void ai_ff_aenc();
void  tonghua() ;
#endif // DEBUG