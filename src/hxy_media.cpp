#include "common.h"
rtsp_demo_handle g_rtsplive = NULL;
rtsp_session_handle g_rtsp_session;
void vi_set()
{
    VI_CHN_ATTR_S pstChnAttr;
    pstChnAttr.pcVideoNode ="rkispp_scale0";//摄像头节点
    pstChnAttr.u32Width = 1920;
    pstChnAttr.u32Height = 1080;
    pstChnAttr.enPixFmt = IMAGE_TYPE_NV12;
    pstChnAttr.u32BufCnt = 2;//设置缓存帧
    pstChnAttr.enBufType = VI_CHN_BUF_TYPE_MMAP;
    pstChnAttr.enWorkMode = VI_WORK_MODE_NORMAL;//工作模式
    int ret = RK_MPI_VI_SetChnAttr(0,0,&pstChnAttr);
    if (ret)
    {
        printf("设置 vi 失败\n");
        return ;
    }
    printf("设置 vi 通道成功\n");
    //使能vi通道
    ret = RK_MPI_VI_EnableChn(0,0);
    if (ret)
    {
        printf("使能 vi 通道失败\n");
        /* code */
        return ;
    }
    printf("使能 vi 通道成功\n");
    //启动vi
    ret = RK_MPI_VI_StartStream(0,0);
    if(ret)
    {
        printf("启动 vi 失败\n");
        return ;
    }
    printf("启动 vi 成功!!!!\n");
}


extern FILE *fp;
//获取一帧画面
void vi_get_frame()
{
    MEDIA_BUFFER buf = RK_MPI_SYS_GetMediaBuffer(RK_ID_VI,0,-1);
    
    fwrite(RK_MPI_MB_GetPtr(buf),RK_MPI_MB_GetSize(buf),1,fp);
 
    printf("获取到的画面的大小:%d\n",RK_MPI_MB_GetSize(buf));
}

//回调函数
void vi_Outcb_fun (MEDIA_BUFFER ch)
{
    printf("11获取到的画面的大小:%d\n",RK_MPI_MB_GetSize(ch));
}
void vi_cd ()
{
    MPP_CHN_S ch;
    ch.enModId = RK_ID_VI;
    ch.s32DevId = 0;
    ch.s32ChnId = 0;
    RK_MPI_SYS_RegisterOutCb(&ch,vi_Outcb_fun);
}

//设置venc的值
void venc_set(IMAGE_TYPE_E image,int w,int h)
{
    int ret;
    VENC_CHN_ATTR_S ch;
    ch.stVencAttr.enType = RK_CODEC_TYPE_H265;//编码格式
    ch.stVencAttr.imageType = image;//采集的图片的格式
    ch.stVencAttr.u32VirWidth =w;
    ch.stVencAttr.u32VirHeight = h ;
    ch.stVencAttr.u32Profile = 77 ; //主流编码等级    
    ch.stVencAttr.u32PicWidth = w;//编码图像宽度
    ch.stVencAttr.u32PicHeight = h;
    
    ch.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
    ch.stRcAttr.stH265Vbr.u32Gop =30;//i帧间隔
    ch.stRcAttr.stH265Vbr.u32SrcFrameRateDen =1;
    ch.stRcAttr.stH265Vbr.u32SrcFrameRateNum =30;
    ch.stRcAttr.stH265Vbr.fr32DstFrameRateDen =1;
    ch.stRcAttr.stH265Vbr.fr32DstFrameRateNum =30;
    ch.stRcAttr.stH265Vbr.u32MaxBitRate =w*h;//最大码率
    ret = RK_MPI_VENC_CreateChn(0,&ch);
    if(ret)
    {
        printf("shibai");
        return ;
    }
    printf("success");


}
int num  = 0;
//设置标志位 10s 每秒30fps 10秒就是300帧
//什么时候进 一帧调一次

// 回调函数
void vi_venc_fun(MEDIA_BUFFER ch)
{
    // 记录计数
    // if (num < 300)
    // {
    //     // 只有前 300 帧才写入文件
    //     fwrite(RK_MPI_MB_GetPtr(ch), RK_MPI_MB_GetSize(ch), 1, fp);
    //     printf("venc 获取到的画面的大小:%d  num= %d\n", RK_MPI_MB_GetSize(ch), num);
    //     num++;
    // }
    // else
    // {
    //    exit(0);
    // }
        if (g_rtsplive && g_rtsp_session) {
        rtsp_tx_video(g_rtsp_session,(const uint8_t *)RK_MPI_MB_GetPtr(ch), RK_MPI_MB_GetSize(ch),
                  RK_MPI_MB_GetTimestamp(ch));
        rtsp_do_event(g_rtsplive);
  }

    RK_MPI_MB_ReleaseBuffer(ch);
    

 
}

void venc_reg()
{
    MPP_CHN_S ch;
    ch.enModId = RK_ID_VENC;
    ch.s32DevId = 0;
    ch.s32ChnId = 0;
    RK_MPI_SYS_RegisterOutCb(&ch,vi_venc_fun);
}



void vi_to_venc()
{
    MPP_CHN_S src,dest;
    src.enModId = RK_ID_VI ;
    src.s32ChnId = 0;
    src.s32DevId = 0 ;

    dest.enModId = RK_ID_VENC;
    dest.s32ChnId =0;
    dest.s32DevId = 0;
    RK_MPI_SYS_Bind(&src,&dest);
}


void rga_venc()
{
    RGA_ATTR_S ch;
    ch.stImgIn.imgType = IMAGE_TYPE_NV12 ; //输入图像格式
    ch.stImgIn.u32X = 0;
    ch.stImgIn.u32Y =0;
    ch.stImgIn.u32Width = 1920 ; //输入的宽高
    ch.stImgIn.u32Height = 1080 ;
    ch.stImgIn.u32HorStride =1920;
    ch.stImgIn.u32VirStride =1080;//虚宽


    ch.stImgOut.imgType = IMAGE_TYPE_BGR888; //输出图像类型
    ch.stImgOut.u32X =0;
    ch.stImgOut.u32Y = 0;
    ch.stImgOut.u32Width =  640;
    ch.stImgOut.u32Height = 480;
    ch.stImgOut.u32HorStride =640;
    ch.stImgOut.u32VirStride =480;//虚宽

    ch.bEnBufPool =RK_TRUE ;//启用缓存池
    ch.u16BufPoolCnt = 3; //缓存帧
    ch.enFlip = RGA_FLIP_NULL;
    ch.u16Rotaion =  0 ;
    if(RK_MPI_RGA_CreateChn(0,&ch))
    {
        printf("创建失败\n");
    }
}


void vi_to_rga_to_venc()
{
    MPP_CHN_S vi_ch,rga_ch,venc_ch;
    vi_ch.enModId = RK_ID_VI;
    vi_ch.s32ChnId = 0;
    vi_ch.s32DevId =0;

    rga_ch.enModId = RK_ID_RGA ;
    rga_ch.s32ChnId = 0;
    rga_ch.s32DevId = 0 ;

    venc_ch.enModId = RK_ID_VENC ;
    venc_ch.s32ChnId = 0;
    venc_ch.s32DevId = 0 ;

    RK_MPI_SYS_Bind(&vi_ch,&rga_ch);//连接

    RK_MPI_SYS_Bind(&rga_ch,&venc_ch);


}

void rga_OutCbFunc(MEDIA_BUFFER mb)
{
    RK_MPI_MB_BeginCPUAccess(mb,RK_FALSE);

    Mat src(480,640,CV_8UC3,RK_MPI_MB_GetPtr(mb));

    putText(src,"hxy",Point(10,200),FONT_HERSHEY_PLAIN,2,Scalar(255,0,0),3);
    RK_MPI_MB_EndCPUAccess(mb,RK_FALSE);
    //把数据送到编码器
    RK_MPI_SYS_SendMediaBuffer(RK_ID_VENC,0,mb);
    RK_MPI_MB_ReleaseBuffer(mb);
}

void vi_rga_bind_register_cb()
{
    MPP_CHN_S vi_chn,rga_chn;
    vi_chn.enModId = RK_ID_VI;
    vi_chn.s32DevId = 0;
    vi_chn.s32ChnId = 0;
    rga_chn.enModId = RK_ID_RGA;
    rga_chn.s32DevId = 0;
    rga_chn.s32ChnId = 0;
    RK_MPI_SYS_Bind(&vi_chn,&rga_chn);
    RK_MPI_SYS_RegisterOutCb(&rga_chn,rga_OutCbFunc);
}

 


void init_rtsp()
{
    g_rtsplive = create_rtsp_demo(554);
    g_rtsp_session = rtsp_new_session(g_rtsplive, "/9203");// rtsp://ip/9203
    //设置视频流
    rtsp_set_video(g_rtsp_session, RTSP_CODEC_ID_VIDEO_H265, NULL, 0);
    rtsp_sync_video_ts(g_rtsp_session, rtsp_get_reltime(), rtsp_get_ntptime());
    //设置音频流
    rtsp_set_audio(g_rtsp_session,RTSP_CODEC_ID_AUDIO_G711A,NULL, 0);
    rtsp_sync_audio_ts(g_rtsp_session, rtsp_get_reltime(), rtsp_get_ntptime());

}//推音视频流