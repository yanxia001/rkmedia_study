#include "hxy_audio.h"
extern FILE *fp;
extern rtsp_demo_handle g_rtsplive;
extern rtsp_session_handle g_rtsp_session;
extern rtsp_demo_handle g_rtsplive1;
extern rtsp_session_handle g_rtsp_session1;
void ai_set()
{
    
    //设置ai通道属性
    AI_CHN_ATTR_S ch;
    ch.pcAudioNode = "default:CARD=Device";//音频设备节点路径
    ch.enSampleFormat = RK_SAMPLE_FMT_S16 ;//采样格式
    ch.u32Channels = 1 ;
    ch.u32SampleRate = 8000 ; //采样率
    ch.u32NbSamples = 1024 ;//每一帧的采样点个数
    ch.enAiLayout = AI_LAYOUT_NORMAL ;//布局类型
    if(RK_MPI_AI_SetChnAttr(0,&ch))
    {
        printf("失败\n");
    }
    printf("设置ai通道属性!!!!!!!!!!!!!!成功!!!!!!!!\n");

    RK_MPI_AI_EnableChn(0);

    //shineng

    if(RK_MPI_AI_StartStream(0))
    {
        printf("AI启动失败");
    }
    printf("使能ai通道成功");

    
}



void ao_set()
{
    AO_CHN_ATTR_S ch ;
    ch.pcAudioNode = "default:CARD=rockchipi2s0sou";//输出设备
    ch.enSampleFormat = RK_SAMPLE_FMT_S16 ;//采样格式
    ch.u32Channels = 2;//双声道
    ch.u32SampleRate = 48000 ; //采样率
    ch.u32NbSamples = 1024 ;//每一帧的采样点个数
    if(RK_MPI_AO_SetChnAttr(1,&ch))
    {
        printf("ao失败\n");
    }


    RK_MPI_AO_EnableChn(1);
}

void ai_to_ao()
{
    MPP_CHN_S ai,ao;
    ai.enModId =RK_ID_AI ;
    ai.s32ChnId = 0 ;

    ao.enModId = RK_ID_AO;
    ao.s32ChnId = 1 ;
    RK_MPI_SYS_Bind(&ai,&ao);
}



void aenc_ai_set()
{
    AENC_CHN_ATTR_S ch;
    ch.u32Quality = 1;//编码质量
    ch.u32Bitrate = 64000;//比特率+
    ch.enCodecType = RK_CODEC_TYPE_G711A;//个编码格式
    ch.stAencG711A.u32Channels = 1;
    ch.stAencG711A.u32NbSample = 1024;
    ch.stAencG711A.u32SampleRate = 8000;
    RK_MPI_AENC_CreateChn(1,&ch);
    int ret = RK_MPI_AENC_CreateChn(0,&ch);//编码通道 结构体
    if(ret)
    {
        printf("创建音频编码通道错误！！！！");
    }
}




void ai_tofengzhuang_aenc()
{
    MPP_CHN_S ai,aenc;
    ai.enModId = RK_ID_AI;
    ai.s32ChnId = 0 ;
    aenc.enModId = RK_ID_AENC;
    aenc.s32ChnId = 1;
    RK_MPI_SYS_Bind(&ai,&aenc);
}



void ai_to_aenc()
{
    MPP_CHN_S ai,aenc;
    ai.enModId = RK_ID_AI;
    ai.s32ChnId = 0 ;
    aenc.enModId = RK_ID_AENC;
    aenc.s32ChnId = 0;
    RK_MPI_SYS_Bind(&ai,&aenc);
}

extern FILE *fp;
void aenc_call_fun(MEDIA_BUFFER mb)
{
    // fwrite(RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetSize(mb), 1, fp);
    // RK_MPI_MB_ReleaseBuffer(mb);
    if (g_rtsplive && g_rtsp_session) {
        rtsp_tx_audio(g_rtsp_session,(const uint8_t *)RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetSize(mb),
                  RK_MPI_MB_GetTimestamp(mb));
        rtsp_do_event(g_rtsplive);
    
  }
      if (g_rtsplive1 && g_rtsp_session1) {
        rtsp_tx_audio(g_rtsp_session1,(const uint8_t *)RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetSize(mb),
                  RK_MPI_MB_GetTimestamp(mb));
        rtsp_do_event(g_rtsplive);
    
  }
    RK_MPI_MB_ReleaseBuffer(mb);
}

void aenc_call()
{
    MPP_CHN_S aenc;
    aenc.enModId = RK_ID_AENC;
    aenc.s32ChnId = 0;
    RK_MPI_SYS_RegisterOutCb(&aenc,aenc_call_fun);
}

void adec_set()
{
    //创建解码通道
    ADEC_CHN_ATTR_S ch ;
    ch.enCodecType = RK_CODEC_TYPE_G711A;
    ch.stAdecG711A.u32Channels =2; //声道数
    ch.stAdecG711A.u32SampleRate = 48000;//采样率
    RK_MPI_ADEC_CreateChn(0,&ch);
}


void adec_to_ao()
{
    MPP_CHN_S ao,adec;
    ao.enModId = RK_ID_AO;
    ao.s32ChnId = 1;
    adec.enModId = RK_ID_ADEC;
    adec.s32ChnId = 0;
    RK_MPI_SYS_Bind(&adec,&ao);
}

void test()
{
    int fd2 = open("./9203",O_RDONLY);
    MB_AUDIO_INFO_S ch;
    
    ch.u32Channels = 1;
    ch.u32SampleRate = 8000;
    ch.u32NBSamples  = 1024;
    ch.enSampleFmt  =RK_SAMPLE_FMT_S16;
    MEDIA_BUFFER mb;
    while(1)
    {
        mb = RK_MPI_MB_CreateAudioBufferExt(&ch,RK_FALSE,0);
        if(::read(fd2,RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetSize(mb))==0)
        {
            printf("读完");
            break;
        }
        RK_MPI_SYS_SendMediaBuffer(RK_ID_ADEC,0,mb);
        RK_MPI_MB_ReleaseBuffer(mb);
        usleep(5000);
    }
}



void tonghua () 
{
    int client = socket(AF_INET,SOCK_STREAM,0);
    if(client <0 )
    {
        printf("client 创建失败");
        return ;
    }
    //2.绑定 服务器有明确的 IP 和端口 
    struct sockaddr_in self;
    self.sin_family = AF_INET;
    self.sin_port = htons(PORT);//大端序端口
    //self.sin_addr.s_addr = inet_addr(IP);//大端序的整数 IP
    self.sin_addr.s_addr = INADDR_ANY;//表示当前设备所有网卡 IP

    int num = bind(client,(struct sockaddr *)&self,sizeof(self));
    if(num == -1)
    {
        perror("bind");
        return ;
    }
    printf("绑定成功\n");
    //3.监听 允许同一时刻 连接服务器的最大数量
    listen(client,3);
    //等待客户端连接
    

while(1)
{
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    int cfd = accept(client,(struct sockaddr *)&cliaddr,&len);
    if(cfd == -1)
    {
        perror("accept");
        return ;
    }

    

//首先打开解码器
    AVCodecContext *ct = avcodec_alloc_context3(NULL);
    ct->channels = 2 ;//声道数
    ct->channel_layout = AV_CH_LAYOUT_STEREO;//声道布局
    ct->sample_fmt = AV_SAMPLE_FMT_S16; //采样格式
    ct->sample_rate = 48000;//采样率
    ct->frame_size = 1024;//每帧采样点数
    ct->bit_rate = 64000;//比特率


    AVCodec *decoder = avcodec_find_decoder_by_name("libfdk_aac");
    if(avcodec_open2(ct,decoder,NULL) == 0)
    {
        printf("打开解码器成功~~~\n");
    }
     else
    {
        printf("打开解码器失败!!!!!\n");
        return ;
    }


    //ao
    AO_CHN_ATTR_S ch ;
    ch.pcAudioNode = "default:CARD=Camera";
    ch.enSampleFormat = RK_SAMPLE_FMT_S16;
    ch.u32Channels = 2;
    ch.u32NbSamples = 1024;
    ch.u32SampleRate = 48000;
    RK_MPI_AO_SetChnAttr(0,&ch);
    RK_MPI_AO_EnableChn(0);

    /*循环从文件中读取 送去解码器解码, 解码后的数据给 ao*/

    // int fd = open("/201/0708ffmpeg/test.aac",O_RDONLY);
    // if(fd == -1){
    //     printf("音频文件打不开!!!!!!\n");
    //     return ;
    // }
    //ao_set();
    uint8_t buf[4096];

    AVPacket *packet = av_packet_alloc();
    
    AVFrame *frame = av_frame_alloc();
    int wcnt,ret;
    MB_AUDIO_INFO_S infor = {2,48000,1024,RK_SAMPLE_FMT_S16};
    MEDIA_BUFFER mb;
    int n =0 ;
    while(1){
        wcnt = read(cfd,buf,sizeof(buf));
        packet->data = buf;
        n ++ ;
        printf("受到的大小为 %d   %d",wcnt,n);
        if(wcnt == 0)
            break;

        packet->size = wcnt;
      

        ret = avcodec_send_packet(ct,packet);//送去解码
        
        while(1)
        {
        //从解码器获取数据
            if(avcodec_receive_frame(ct,frame))
                break;
            
            printf("已取到解码音频!!\n");
        //送到 ao
            mb = RK_MPI_MB_CreateAudioBufferExt(&infor,RK_FALSE,0);
            memcpy(RK_MPI_MB_GetPtr(mb),frame->data[0],frame->linesize[0]);
            RK_MPI_SYS_SendMediaBuffer(RK_ID_AO,0,mb);
            RK_MPI_MB_ReleaseBuffer(mb);
        }
    }
 }


    return ;
}

void aac_dec_ao()
{
    //首先打开解码器
    AVCodecContext *ct = avcodec_alloc_context3(NULL);
    ct->channels = 2 ;//声道数
    ct->channel_layout = AV_CH_LAYOUT_STEREO;//声道布局
    ct->sample_fmt = AV_SAMPLE_FMT_S16; //采样格式
    ct->sample_rate = 48000;//采样率
    ct->frame_size = 1024;//每帧采样点数
    ct->bit_rate = 64000;//比特率

    AVCodec *decoder = avcodec_find_decoder_by_name("libfdk_aac");
    if(avcodec_open2(ct,decoder,NULL) == 0)
    {
        printf("打开解码器成功~~~\n");
    }
     else
    {
        printf("打开解码器失败!!!!!\n");
        return ;
    }


    //ao
    AO_CHN_ATTR_S ch ;
    ch.pcAudioNode = "default";
    ch.enSampleFormat = RK_SAMPLE_FMT_S16;
    ch.u32Channels = 2;
    ch.u32NbSamples = 1024;
    ch.u32SampleRate = 48000;
    RK_MPI_AO_SetChnAttr(0,&ch);
    RK_MPI_AO_EnableChn(0);

    /*循环从文件中读取 送去解码器解码, 解码后的数据给 ao*/

    int fd = open("/201/0708ffmpeg/test.aac",O_RDONLY);
    if(fd == -1){
        printf("音频文件打不开!!!!!!\n");
        return ;
    }
    ao_set();
    uint8_t buf[4096];

    AVPacket *packet = av_packet_alloc();
    packet->data = buf;
    AVFrame *frame = av_frame_alloc();
    int wcnt,ret;
    MB_AUDIO_INFO_S infor = {2,48000,1024,RK_SAMPLE_FMT_S16};
    MEDIA_BUFFER mb;
    
    while(1){
        wcnt = read(fd,buf,sizeof(buf));
        
        if(wcnt == 0)
            break;

        packet->size = wcnt;
        ret = avcodec_send_packet(ct,packet);//送去解码
        
        while(1)
        {
        //从解码器获取数据
            if(avcodec_receive_frame(ct,frame))
                break;
            printf("已取到解码音频!!\n");
            
        //送到 ao
            mb = RK_MPI_MB_CreateAudioBufferExt(&infor,RK_FALSE,0);
            memcpy(RK_MPI_MB_GetPtr(mb),frame->data[0],frame->linesize[0]);
            RK_MPI_SYS_SendMediaBuffer(RK_ID_AO,1,mb);
            RK_MPI_MB_ReleaseBuffer(mb);
        }
 }
}

AVCodecContext * ct;
AVPacket * packet;
AVFrame * frame;



void ai_ff_fun(MEDIA_BUFFER mb)
{
    memcpy(frame->data[0],RK_MPI_MB_GetPtr(mb),RK_MPI_MB_GetSize(mb));
    //送到编码器
    avcodec_send_frame(ct,frame);

    //从编码器中区数据
    while(1)
    {
        if(avcodec_receive_packet(ct ,packet))
            break;
        //取到编码后推流
        if (g_rtsplive1 && g_rtsp_session1) 
        {
            rtsp_tx_audio(g_rtsp_session1,(const uint8_t *)packet->data, packet->size,
                  RK_MPI_MB_GetTimestamp(mb));//时间戳
            rtsp_do_event(g_rtsplive1);
        }
        if (g_rtsplive && g_rtsp_session) 
        {
            rtsp_tx_audio(g_rtsp_session,(const uint8_t *)packet->data, packet->size,
                  RK_MPI_MB_GetTimestamp(mb));//时间戳
            rtsp_do_event(g_rtsplive);
        }
    
    }
    RK_MPI_MB_ReleaseBuffer(mb);
}


void ai_ff_aenc()
{

    //设置ai通道属性
    AI_CHN_ATTR_S ch;
    ch.pcAudioNode = "default:CARD=Device";//音频设备节点路径
    ch.enSampleFormat = RK_SAMPLE_FMT_S16 ;//采样格式
    ch.u32Channels = 1 ;
    ch.u32NbSamples = 1024 ;//每一帧的采样点个数
    ch.enAiLayout = AI_LAYOUT_NORMAL ;//布局类型
    ch.u32SampleRate = 8000;
    if(RK_MPI_AI_SetChnAttr(0,&ch))
    {
        printf("失败\n");
    }
    printf("设置ai通道属性!!!!!!!!!!!!!!成功!!!!!!!!\n");

    RK_MPI_AI_EnableChn(0);
    RK_MPI_AI_StartStream(0);

    ct = avcodec_alloc_context3(NULL);
    ct->bit_rate = 64000;
    ct->frame_size =1024;
    ct->sample_rate = 8000;
    ct->channels = 1 ;
    ct->channel_layout = AV_CH_LAYOUT_MONO;
    ct->sample_fmt = AV_SAMPLE_FMT_S16;

    //使用ffmepg编码音频
    AVCodec * encode = avcodec_find_encoder_by_name("libfdk_aac");
    avcodec_open2(ct,encode,NULL);

    frame=av_frame_alloc();
    frame->channel_layout =AV_CH_LAYOUT_MONO;
    frame->nb_samples = 1024;
    frame->format = AV_SAMPLE_FMT_S16;

    av_frame_get_buffer(frame,0);

    //音频采集回调函数
    packet = av_packet_alloc();
    MPP_CHN_S chn;
    chn.enModId = RK_ID_AI;
    chn.s32ChnId = 0;
    RK_MPI_SYS_RegisterOutCb(&chn,ai_ff_fun);


}