#include <stdio.h>
#include <SDL2/SDL.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
int main()
{
 //音频输入设备打开
    avdevice_register_all();
    AVFormatContext *formatContext = avformat_alloc_context();
    AVInputFormat *input = av_find_input_format("alsa");
    avformat_open_input(&formatContext,"default",input,NULL);
 //输出设备打开
    SDL_AudioSpec want;
    want.channels = 2;//双声道
    want.format = AUDIO_S16SYS;//采样格式
    want.freq = 48000;//采样率
    want.samples = 1024;

    want.callback = NULL;
    want.userdata = NULL;
    if(SDL_OpenAudio(&want,NULL) == 0)
    {
        printf("音频输出设备打开成功\n");
    }
    SDL_PauseAudio(0);
    AVPacket *packet = av_packet_alloc();
    while(1)
    {
        //从麦克风中取音频
        av_read_frame(formatContext,packet);
        //送去播放
        SDL_QueueAudio(1,packet->data,packet->size);
    }
    return 0;
}