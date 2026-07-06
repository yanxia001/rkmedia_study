#include "common.h"
#include "hxy_media.h"  
#include "hxy_audio.h"


FILE *fp ;
void sig_fun(int)
{
    RK_MPI_VI_DisableChn(0,0);
    SAMPLE_COMM_ISP_Stop(0);
    exit(0);
}
int main()
{

    signal(2,sig_fun);
    //初始化
    RK_MPI_SYS_Init();
    //isp设置
    SAMPLE_COMM_ISP_Init(0,RK_AIQ_WORKING_MODE_NORMAL,RK_FALSE,"/oem/etc/iqfiles");
    SAMPLE_COMM_ISP_Run(0);
    SAMPLE_COMM_ISP_SetFrameRate(0,30);

    //fp = fopen("./test.h265","w+");


    // vi_set();
    // rga_venc();
    // venc_set(IMAGE_TYPE_BGR888,640,480);
    
    // //vi_to_venc();
    // //vi_to_rga_to_venc();
    // vi_rga_bind_register_cb();

    // venc_reg();
    // //vi_cd();

    // ai_set();
    // aenc_ai_set();

    // ai_to_aenc();
    // aenc_call();
    // fp = fopen("./9203","w+");
    // ao_set();

    // // ai_to_ao();
    // adec_set();
    // adec_to_ao();

    // test();


    init_rtsp();
    
    vi_set();
    venc_set(IMAGE_TYPE_RGB888,640,480);
    //vi_to_venc();
    rga_venc();
    //vi_to_rga_to_venc(); //VI → RGA → VENC 系统自动转发
    vi_rga_bind_register_cb();//VI → RGA → callback 拦截 → 手动发 VENC 
    venc_reg();

    ai_set();
    aenc_ai_set();
    ai_to_aenc();
    aenc_call();
  
    while(1)
    {}

    return 0;
    
}