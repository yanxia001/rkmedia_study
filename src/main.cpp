#include "common.h"
#include "hxy_media.h"  
#include "hxy_audio.h"

#include "fengzhuang.h"
FILE *fp ;
void sig_fun(int)
{
    RK_MPI_VI_DisableChn(0,0);
    SAMPLE_COMM_ISP_Stop(0);
    exit(0);
}
  void delete_oldest_file_if_full()
  {
      DIR *dir = opendir("/mnt/sdcard");
      if (!dir) return;

      char oldest_name[256] = {0};
      time_t oldest_time = 0;
      struct dirent *entry;
      struct stat st;

      while ((entry = readdir(dir)) != NULL)
      {
          if (entry->d_type != DT_REG) continue;

          char fullpath[512];
          snprintf(fullpath, sizeof(fullpath), "/mnt/sdcard/%s", entry->d_name);
          stat(fullpath, &st);

          if (oldest_time == 0 || st.st_mtime < oldest_time)
          {
              oldest_time = st.st_mtime;
              strncpy(oldest_name, entry->d_name, sizeof(oldest_name) - 1);
          }
      }
      closedir(dir);

      if (oldest_name[0] != '\0')
      {
          char fullpath[512];
          snprintf(fullpath, sizeof(fullpath), "/mnt/sdcard/%s", oldest_name);
          remove(fullpath);
          printf("删除最旧文件: %s\n", fullpath);
      }
  }
void* save_fun(void* arg)
{
    // SD 卡检测 + 封装逻辑
    bool save_started = false;
    while (1)
    {
        FILE *fd = NULL;
        FILE *fd1 = NULL;
        char buf[10] = {0};
        char buf1[10] = {0};
        int a = 100;
        bool sd_ready = false;

        fd = popen("cat /sys/block/mmcblk*/device/type", "r");
        if(fd)
        {
            if(fgets(buf, sizeof(buf), fd))
            {
                sd_ready = (buf[0] == 'S' && buf[1] == 'D');
            }
            pclose(fd);
        }

        if(sd_ready)
        {
            
            fd1 = popen("df -h /mnt/sdcard | awk 'NR==2 {print $5}'", "r");
            if(fd1)
            {
                if(fgets(buf1, sizeof(buf1), fd1))
                {
                    a = atoi(buf1);
                }
                pclose(fd1);
            }

            if(a <= 1 && !save_started)
            {

                vi_todengzhuang_venc();
                ai_tofengzhuang_aenc();
                muxer_set();
                save_started = true;
            }
            else if (a >1)
            {
                delete_oldest_file_if_full();
            }

          }
          else
          {
            printf("sd卡丢失");
            save_started = false;
          }

          sleep(1);
    }
    return NULL;
}

void* talk_fun(void* arg)
{
    tonghua();   // 远程对讲
    return NULL;
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


    // init_rtsp();//推流
    
    // vi_set();
    // venc_set(IMAGE_TYPE_NV12,1920,1080);
    // vi_to_venc();
    // // vi_todengzhuang_venc();
    // // //rga_venc();
    // // //vi_to_rga_to_venc(); //VI → RGA → VENC 系统自动转发
    // // //vi_rga_bind_register_cb();//VI → RGA → callback 拦截 → 手动发 VENC 
    // venc_reg();
    
    


    // ai_ff_aenc();
    // aac_dec_ao();



    son_init_rtsp();
    main_init_rtsp();
    vi_set();
    venc_set(IMAGE_TYPE_NV12,1920,1080);
    vi_to_venc();
    venc_main_reg();
    rga_venc();
    venc_son_set(IMAGE_TYPE_BGR888,640,480);
    vi_to_rga_to_venc();
    venc_son_reg();

    ai_set();
    aenc_ai_set();
    ai_to_aenc();
    aenc_call();
    // // ===== 远程对讲 =====
    //ai_set();              // 麦克风采集 (AI ch0, 8000Hz/1ch/S16)
    // ao_set();              // I2S 扬声器输出 (AO ch1)
    //adec_set();            // G711A 解码器 (ADEC ch0)
    // //adec_to_ao();          // ADEC ch0 → AO ch1 绑定
    //ai_ff_aenc();

    pthread_t tid_talk,tid_save;

    pthread_create(&tid_talk,NULL,talk_fun,NULL);

    //pthread_create(&tid_save,NULL,save_fun,NULL);
    // pid_t talk;
    // talk = fork();
    // if(talk == 0)
    // {
    //     tonghua ();
    // }

    

    // pid_t save = fork();

    // if(save == 0)
    // {
    //   bool save_started = false;

    //   while(1)
    //   {
    //       FILE *fd = NULL;
    //       FILE *fd1 = NULL;
    //       char buf[10] = {0};
    //       char buf1[10] = {0};
    //       int a = 100;
    //       bool sd_ready = false;

    //       fd = popen("cat /sys/block/mmcblk*/device/type", "r");
    //       if(fd)
    //       {
    //           if(fgets(buf, sizeof(buf), fd))
    //           {
    //               sd_ready = (buf[0] == 'S' && buf[1] == 'D');
    //           }
    //           pclose(fd);
    //       }

    //       if(sd_ready)
    //       {
    //           fd1 = popen("df -h /mnt/sdcard | awk 'NR==2 {print $5}'", "r");
    //           if(fd1)
    //           {
    //               if(fgets(buf1, sizeof(buf1), fd1))
    //               {
    //                   a = atoi(buf1);
    //               }
    //               pclose(fd1);
    //           }

    //           if(a <= 80 && !save_started)
    //           {
    //               ai_set();
    //               aenc_ai_set();
    //               ai_tofengzhuang_aenc();
    //               muxer_set();
    //               save_started = true;
    //           }
    //       }
    //       else
    //       {
    //           save_started = false;
    //       }

    //       sleep(1);
    //   }
    // }

    while(1)
    {
      sleep(60);
    }


    return 0;
    
}