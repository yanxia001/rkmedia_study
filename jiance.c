#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
pid_t pid1 ,pid2 ,pid3 = 0;


int main()
{
          // ====== WiFi 初始化：只跑一次 ======
    printf("@@@@@@@@@@@@@@@@@@@@@@@@%s is called\n",__FILE__);
    pid_t pid_wifi = fork();
    if (pid_wifi < 0) return -1;

    printf("1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    if (pid_wifi == 0) 
    {
        system("insmod /201/external/8188eu.ko");
        system("ifconfig wlan0 up");
        system("wpa_supplicant -D wext -c /etc/wpa_supplicant.conf -i wlan0 -B");
        system("udhcpc -i wlan0");
        printf("2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        int ret = system("ping -c 3 -W 2 www.baidu.com > /dev/null 2>&1");
        if (ret == 0) 
        {
            printf("WiFi 联网成功\n");
            _exit(0);   // ← 只告诉父进程结果，不变成别的程序
        } 
        else
        {
            printf("WiFi 联网失败\n");
             _exit(1);
        }
    }

    // 父进程等 WiFi 初始化完成
    int wifi_status;
    waitpid(pid_wifi, &wifi_status, 0);
    int wifi_ok = (WIFEXITED(wifi_status) && WEXITSTATUS(wifi_status) == 0);

    if (WIFEXITED(wifi_status) && WEXITSTATUS(wifi_status) != 0) {
          printf("WiFi 初始化失败，但不影响有线网络，继续运行...\n");
    }

    pid_t pid_aout = -1;
    if (wifi_ok) 
    {
        pid_aout = fork();
        if (pid_aout < 0) return -1;

        if (pid_aout == 0) 
        {
            execl("/201/external/a.out", "a.out", NULL);
            perror("execl a.out failed");
             _exit(1);
        }

        // ↓↓↓ 改这里：等 a.out 同步完时间，别急着杀 ↓↓↓
        waitpid(pid_aout, NULL, 0);
        printf("时间同步完成\n");
    }



    while(1)
    {
        pid_t pid;

        pid = fork();//pid 在父进程中返回子进程的 PID 号，在子进程中返回 0；
        if(pid<0)
            return -1;
        if(pid  == 0)
        {
            /* 子进程1：运行 mymain */
            
            execl("/201/0701rkmidea/build/mymain", "mymain", NULL);
            perror("execl failed");
            _exit(1);
        }
        pid1 = pid;

        pid = fork();
        pid2 = pid;
        if (pid2 == 0) {
              while (1) {
                  FILE *fp = fopen("/sys/class/net/eth0/carrier", "r");
                  char val = '0';

                  if (fp) {
                      val = fgetc(fp);
                      fclose(fp);
                  }

                  if (val == '1') {         // ← 修复：== 且跟 '1' 比
                      kill(pid1, SIGCONT);   // 网线插着 → 继续跑
                  } else {
                      kill(pid1, SIGSTOP);   // 网线拔了 → 暂停
                  }

                  sleep(1);
              }
              _exit(0);  // 防御性
        }


        
        
        int status;
        waitpid(pid1, &status, 0);

        kill(pid2, SIGKILL);
        waitpid(pid2, NULL, 0);   // 收尸

        
        if (WIFEXITED(status)) 
        {
            int code = WEXITSTATUS(status);
            printf("mymain 正常退出，退出码: %d\n", code);
            if (code == 0) 
            {
                // 正常退出码为 0，说明是预期内的关闭，不重启
                printf("mymain 正常结束，不再重启\n");
                break;
            }
        } 
        else if (WIFSIGNALED(status)) 
        {
            printf("mymain 被信号 %d 杀死\n", WTERMSIG(status));
        }

        // 异常退出 → 重启
        printf("正在重启 mymain...\n");
        sleep(1);  // 避免疯狂重启
    }
}