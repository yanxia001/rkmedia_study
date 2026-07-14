#ifndef _COMMON_H_
#define _COMMON_H_

extern "C"
{
#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "common/sample_common.h"
#include "rkmedia_api.h" 
#include "librtsp/rtsp_demo.h"


#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libavcodec/avcodec.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



  #include <dirent.h>
  #include <sys/stat.h>
  #include <string.h>
}

#include "opencv2/opencv.hpp"
using namespace cv;
#endif // DEBUG