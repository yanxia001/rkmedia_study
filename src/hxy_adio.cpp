#include "hxy_audio.h"
extern FILE *fp;
extern rtsp_demo_handle g_rtsplive;
extern rtsp_session_handle g_rtsp_session;

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
    ch.u32Channels = 1;//双声道
    ch.u32SampleRate = 8000 ; //采样率
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
    int ret = RK_MPI_AENC_CreateChn(0,&ch);//编码通道 结构体
    if(ret)
    {
        printf("创建音频编码通道错误！！！！");
    }
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
    ch.stAdecG711A.u32Channels =1; //声道数
    ch.stAdecG711A.u32SampleRate = 8000;//采样率
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