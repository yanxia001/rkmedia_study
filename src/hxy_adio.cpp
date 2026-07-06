#include "hxy_audio.h"


void ai_set()
{
    
    //设置ai通道属性
    AI_CHN_ATTR_S ch;
    ch.pcAudioNode = "default:CARD=Device";//音频设备节点路径
    ch.enSampleFormat = RK_SAMPLE_FMT_S16 ;//采样格式
    ch.u32Channels = 2 ;
    ch.u32SampleRate = 48000 ; //采样率
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

