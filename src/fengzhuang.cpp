#include "fengzhuang.h"

void muxer_set()
{
    MUXER_CHN_ATTR_S ch;
    ch.enType = MUXER_TYPE_FLV;//封装类型
    ch.enMode = MUXER_MODE_AUTOSPLIT;//封装格式

    ch.stSplitAttr.enSplitType = MUXER_SPLIT_TYPE_TIME;//按时间分割
    ch.stSplitAttr.u32TimeLenSec = 10 ;//5s分割一次 5s一个视频

    ch.stSplitAttr.enSplitNameType = MUXER_SPLIT_NAME_TYPE_AUTO;//自动命名分割
    ch.stSplitAttr.stNameAutoAttr.bTimeStampEnable = RK_TRUE; //文件命名带时间辍
    ch.stSplitAttr.stNameAutoAttr.pcBaseDir = "/mnt/sdcard";//封装视频村
    ch.stSplitAttr.stNameAutoAttr.pcPrefix ="hxy"; //文件前缀
    ch.stSplitAttr.stNameAutoAttr.u16StartIdx = 1;//索引值

    //视频流参数
    ch.stVideoStreamParam.enCodecType = RK_CODEC_TYPE_H264;//编码格式
    ch.stVideoStreamParam.enImageType = IMAGE_TYPE_NV21;//图像格式
    ch.stVideoStreamParam.u16Fps = 30;
    ch.stVideoStreamParam.u16Level = 41;
    ch.stVideoStreamParam.u16Profile = 77 ;//编码等级
    ch.stVideoStreamParam.u32BitRate =1920*1080; //比特率
    ch.stVideoStreamParam.u32Width =1920;
    ch.stVideoStreamParam.u32Height =1080;


    //音频流参数
    ch.stAudioStreamParam.enCodecType =RK_CODEC_TYPE_G711A;//编码格式
    ch.stAudioStreamParam.enSampFmt = RK_SAMPLE_FMT_S16;//采样格式
    ch.stAudioStreamParam.u32Channels = 1 ;
    ch.stAudioStreamParam.u32NbSamples = 1024;
    ch.stAudioStreamParam.u32SampleRate =8000 ;//采样率
    
    ch.u32MuxerId = 0;

    RK_MPI_MUXER_EnableChn(0,&ch);

    //绑定封装器和流信息
    MUXER_CHN_S src;
    MPP_CHN_S  vch,ach;
    vch.enModId = RK_ID_VENC;
    vch.s32ChnId = 2;
    vch.s32DevId = 0;

    src.enModId = RK_ID_MUXER;
    src.enChnType = MUXER_CHN_TYPE_VIDEO ;
    src.s32ChnId = 0;

    RK_MPI_MUXER_Bind(&vch,&src);

    ach.enModId = RK_ID_AENC;
    ach.s32ChnId = 1;
    src.enChnType = MUXER_CHN_TYPE_AUDIO;
    RK_MPI_MUXER_Bind(&ach,&src);
    
    RK_MPI_MUXER_StreamStart(0);

}