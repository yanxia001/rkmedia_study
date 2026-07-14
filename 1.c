#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libavcodec/avcodec.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
//上下文 ---结构体


int main ()
{
    //客户端设计
    int kehu = socket(AF_INET,SOCK_STREAM,0);
    if(kehu <0)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in seraddr;
    seraddr.sin_family = AF_INET;
    //端口 80 是小端序,成员 sin_port 要的是大端序,
    //需要借助 htons 将小端序转为大端序
    seraddr.sin_port = htons(6556);
    //"116.62.81.138"是点分十进制
    //sin_addr 需要大端序整数 IP 借助 inet_aton 转换
    inet_aton("192.168.100.75",&seraddr.sin_addr);
    int ret = connect(kehu,(struct sockaddr *)&seraddr,sizeof(seraddr));
    if(ret == -1)
    {
        perror("connect");
        return -1;
    }
    printf("连接服务器成功\n");




    //注册音频设备
    avdevice_register_all();
    //音频的采集
    AVFormatContext * a = avformat_alloc_context();

    AVDictionary *options = NULL;
    av_dict_set(&options, "sample_rate", "48000", 0);
    av_dict_set(&options, "channels", "2", 0);
    av_dict_set(&options, "sample_fmt", "s16", 0);

    AVInputFormat * a_input = av_find_input_format("alsa");
    if(avformat_open_input(&a,"default",a_input,&options))//打开一个输入设备
    {
        printf("打开失败\n");
    }
    else
    {
        printf("打开成功\n");
    }

    //音频编码
    //打开音频编码器
    AVCodecContext * c =avcodec_alloc_context3(NULL);//上下文
    c->bit_rate = 64000;//比特率
    c->sample_fmt = AV_SAMPLE_FMT_S16;//采样格式
    c->sample_rate = 48000;//采样率
    c->channels = 2 ;
    c->channel_layout = AV_CH_LAYOUT_STEREO; //省道布局
    c->frame_size = 1024; //每帧采样点那个数

    AVCodec *  c1 = avcodec_find_encoder_by_name("libfdk_aac");
    if( c1 == NULL)
    {
        printf("未找到编码器\n");
        return 0;
    }
    else{
        printf("已找到编码器\n");
    }

    if(avcodec_open2(c,c1,NULL) == 0)//打开编码器
    {
        printf("打开编码器成功~~~\n");
    }


    //采集音频送到编码器

    AVPacket *packet = av_packet_alloc(); //原始数据包
    AVPacket *encPacket = av_packet_alloc(); //编码后的数据包

//AVFrame = 编码器和解码器之间的"中间格式"，是你可以直接触碰的裸音频数据。编码器吃 AVFrame，吐出 AVPacket。
    AVFrame *frame = av_frame_alloc();//开辟frame空间，也是原始数据
    frame->channel_layout = AV_CH_LAYOUT_STEREO; //声道布局
    frame->nb_samples = 1024; //每帧采样点数
    frame->format = AV_SAMPLE_FMT_S16; //采样格式

    av_frame_get_buffer(frame,0);//frame 开辟音频或视频的数据缓冲区
    int cnt = 0;


    // int fd = open("test.aac",O_WRONLY|O_CREAT|O_TRUNC,0644);

    int n = 0;
    while(1)
    {
        //从麦克风读数据
        av_read_frame(a,packet);//对着麦克风录了一小段，放进 packet 里
        //把读到的数据，拼接到 frame 的缓冲区里
        memcpy(frame->data[0]+cnt*packet->size, // 目标：frame 缓冲区从第 cnt 块开始的位置
                packet->data,                   // 来源：刚读到的数据
                packet->size);                  // 长度：这一包的大小
        cnt++; // 计数器 +1，表示又攒了一块

        // 如果攒的数据还不够 4096 字节，继续录
        if(cnt*packet->size < 4096)
            continue;
        cnt = 0;// 攒够了，重置计数器

        //送到编码器
        avcodec_send_frame(c,frame);
        //从编码器中获取数据
        while(1)
        {
            // 从编码器 c 里取一包编码好的数据（AAC），放入 encPacket
            if(avcodec_receive_packet(c,encPacket))
                break;
            // 把编码后的数据写入文件
            n++;
            int a = write(kehu,encPacket->data,encPacket->size);
            printf("写入%d     %d\n",a,n);
            printf("encPacket %d \n",encPacket->size);
        }
    }
    return 0;
}