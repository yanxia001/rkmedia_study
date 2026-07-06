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
    // SAMPLE_COMM_ISP_Init(0,RK_AIQ_WORKING_MODE_NORMAL,RK_FALSE,"/oem/etc/iqfiles");
    // SAMPLE_COMM_ISP_Run(0);
    // SAMPLE_COMM_ISP_SetFrameRate(0,30);

    //fp = fopen("./test.h265","w+");


    // vi_set();
    // rga_venc();
    // venc_set(IMAGE_TYPE_BGR888,640,480);
    
    // //vi_to_venc();
    // //vi_to_rga_to_venc();
    // vi_rga_bind_register_cb();

    // venc_reg();
    // //vi_cd();

    ai_set();
    ao_set();

    ai_to_ao();


    while(1)
    {
        usleep(5000);
    }
  
    

    return 0;
    
}