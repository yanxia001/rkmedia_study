# 瑞芯微 RV1126/RV1109 音视频采集与编码 demo

基于 **Rockchip RV1126/RV1109** 平台的嵌入式音视频多媒体 demo 项目，演示了摄像头视频采集、H.265 硬编码、RGA 硬件加速图像转换、OpenCV 文字叠加以及音频采集与播放等功能。

## 功能特性

### 音频
- **音频采集 (AI)**：通过 `default:CARD=Device` 节点采集麦克风输入，48kHz / 立体声 / S16 格式
- **音频播放 (AO)**：通过 `rockchipi2s0sou` I2S 数字音频接口输出
- **音频直通**：将 AI 通道直接绑定到 AO 通道，实现麦克风 -> 扬声器的实时音频环回

### 视频
- **视频采集 (VI)**：从 ISP `rkispp_scale0` 节点获取 1920×1080 NV12 图像
- **H.265 硬编码 (VENC)**：VBR 码率控制，30fps，GOP=30，输出 H.265 码流
- **RGA 硬件加速**：NV12 → RGB888 色彩空间转换 + 1920×1080 → 640×480 缩放
- **OpenCV 文字叠加**：在 RGA 处理后的帧上绘制文字标注（"hxy"），再送入编码器
- **码流保存**：将编码后的 H.265 帧写入 `test.h265` 文件（前 300 帧 / 约 10 秒）

## 硬件平台

- **芯片**：Rockchip RV1126 / RV1109
- **系统**：Buildroot Linux（来自 RV1126_RV1109_LINUX_SDK_V2.2.5.1）
- **架构**：ARM (arm-linux-gnueabihf)

## 项目结构

```
.
├── CMakeLists.txt          # CMake 构建配置（交叉编译）
├── build/                  # 构建输出目录
│   └── mymain              # 编译生成的可执行文件
├── include/                # 头文件
│   ├── common.h            # 公共头文件（RKMedia SDK、OpenCV 等）
│   ├── hxy_audio.h         # 音频功能声明
│   └── hxy_media.h         # 视频/媒体功能声明
└── src/                    # 源文件
    ├── main.cpp            # 程序入口
    ├── hxy_adio.cpp        # 音频采集/播放实现
    └── hxy_media.cpp       # 视频采集/编码/RGA 实现
```

## 依赖库

### Rockchip 专有库
| 库名 | 说明 |
|------|------|
| `libeasymedia` | EasyMedia 多媒体框架 |
| `librga` | RGA 2D 硬件图形加速 |
| `librkaiq` | AI 图像质量 (ISP) 调优 |
| `librockx` | RockX AI 推理 SDK |
| `libthird_media` | 第三方媒体集成 |
| `librtsp` | RTSP 流媒体（静态链接） |

### 第三方库
| 库名 | 版本 | 说明 |
|------|------|------|
| OpenCV | 3.4.12 | 计算机视觉（文字叠加） |
| FFmpeg | 4.1.3 | 音视频编解码 |

### 系统库
`pthread`、`dl`

## 构建

### 前置条件

1. 安装 **RV1126_RV1109_LINUX_SDK_V2.2.5.1**（或兼容版本）
2. 确保 SDK 中包含 Buildroot 交叉编译工具链及所有依赖库

### 配置 SDK 路径

修改 `CMakeLists.txt` 第 7 行的 `CMAKE_HOME` 为你的 SDK 实际路径：

```cmake
set(CMAKE_HOME /home/hxy/)   # ← 改为你的路径
```

### 编译

```bash
mkdir build && cd build
cmake ..
make
```

编译成功后在 `build/` 目录下生成 `mymain` 可执行文件。

## 运行

将 `mymain` 部署到 RV1126/RV1109 开发板：

```bash
# 上传可执行文件
scp build/mymain root@<板子IP>:/tmp/

# SSH 登录开发板并运行
ssh root@<板子IP>
cd /tmp
chmod +x mymain
./mymain
```

按 `Ctrl+C` 停止程序（会触发信号处理，停止 ISP 并关闭 VI 通道）。

## 主要 API 调用流程

### 当前激活流程（音频直通）

```
RK_MPI_SYS_Init()           # 系统初始化
    ├── ai_set()            # 配置 AI 通道 0（麦克风）
    ├── ao_set()            # 配置 AO 通道 1（扬声器）
    └── ai_to_ao()          # 绑定 AI → AO（音频环回）
```

### 视频流水线（已注释，可按需启用）

```
RK_MPI_SYS_Init()
    ├── SAMPLE_COMM_ISP_Init()    # ISP 初始化
    ├── vi_set()                  # 配置 VI 通道（1920×1080 NV12）
    ├── rga_venc()                # 配置 RGA 通道（NV12→RGB888, 1080p→480p）
    ├── venc_set()                # 配置 VENC 通道（H.265 VBR）
    ├── vi_rga_bind_register_cb() # 绑定 VI→RGA，注册 RGA 回调
    │     └── rga_OutCbFunc()     #   RGA 回调：OpenCV 画文字 → 送编码器
    └── venc_reg()                # 注册 VENC 回调
          └── vi_venc_fun()       #   VENC 回调：写 H.265 文件（300 帧）
```

## 说明

- 这是一个学习/测试性质的 demo 项目，用于熟悉 RKMedia API 的音频、视频、RGA 和编码器模块
- `main.cpp` 中视频相关代码已注释，当前仅启用音频直通功能；如需测试视频流水线，取消对应注释即可
- 视频流水线中 RGA 回调用 OpenCV 在每帧上绘制 "hxy" 文字，可用于验证 RGA → OpenCV → VENC 的数据通路
- 编译产物 `build/mymain` 已交叉编译为 ARM 架构，无法在 x86 主机上直接运行
- SDK 路径中的 `RV1126_RV1109_LINUX_SDK_V2.2.5.1_20231011` 是示例版本号，其他版本可能需要调整头文件和库的路径

## 函数功能说明

### `src/main.cpp` — 程序入口

| 函数 | 说明 |
|------|------|
| `main()` | 程序入口。初始化系统→ISP→RTSP→VI→VENC→RGA→AI→AENC，绑定各通道并注册回调，最后进入死循环等待 |
| `sig_fun(int)` | 信号处理函数，捕获 Ctrl+C（SIGINT），停止 ISP 并关闭 VI 通道后退出 |

### `src/hxy_media.cpp` — 视频采集 / 编码 / RGA / RTSP

| 函数 | 说明 |
|------|------|
| `vi_set()` | 配置 VI 通道 0：从 `rkispp_scale0` 节点获取 1920×1080 NV12 图像，设置缓存帧数为 2，使能并启动流 |
| `vi_get_frame()` | 从 VI 通道获取一帧原始数据，写入文件（调试用，当前未使用） |
| `vi_Outcb_fun(MEDIA_BUFFER ch)` | VI 回调函数（测试用），打印获取到的帧数据大小 |
| `vi_cd()` | 注册 VI 通道的输出回调 `vi_Outcb_fun`（调试用，当前未使用） |
| `venc_set(IMAGE_TYPE_E image, int w, int h)` | 创建 VENC 通道 0：H.265 编码，VBR 码率控制，30fps，GOP=30。参数指定输入图像格式和宽高 |
| `vi_venc_fun(MEDIA_BUFFER ch)` | VENC 编码回调函数。将编码后的 H.265 码流通过 RTSP 推流出去（`rtsp_tx_video`），同时驱动 `rtsp_do_event` |
| `venc_reg()` | 注册 VENC 通道 0 的输出回调 `vi_venc_fun`，每编码完一帧触发一次 |
| `vi_to_venc()` | 绑定 VI→VENC，系统自动将 VI 采集的帧送入编码器（跳过 RGA，当前已注释） |
| `rga_venc()` | 创建 RGA 通道 0：NV12→BGR888 色彩转换 + 1920×1080→640×480 缩放，启用缓存池（3 帧） |
| `rga_OutCbFunc(MEDIA_BUFFER mb)` | RGA 回调函数。RGA 处理完成后触发，用 OpenCV 在帧上画 "hxy" 文字标注，再手动 `SendMediaBuffer` 送入 VENC |
| `vi_rga_bind_register_cb()` | 绑定 VI→RGA 并注册 RGA 回调。走 **拦截路径**：VI 帧经 RGA 转换后回到回调，由回调手动送入 VENC（可插入 OpenCV 处理） |
| `vi_to_rga_to_venc()` | 绑定 VI→RGA→VENC **系统自动转发路径**。帧自动流转，无回调拦截，无法插入 OpenCV 处理（当前已注释） |
| `init_rtsp()` | 初始化 RTSP 服务：创建端口 554 的 RTSP 服务端，新建 `/9203` 会话，设置 H.265 视频流 + G711A 音频流并同步时间戳 |

### `src/hxy_adio.cpp` — 音频采集 / 编码 / 解码 / 播放

| 函数 | 说明 |
|------|------|
| `ai_set()` | 配置 AI 通道 0：从 `default:CARD=Device` 采集音频，8000Hz / 单声道 / S16 / 每帧 1024 采样点，使能并启动流 |
| `aenc_ai_set()` | 创建 AENC 通道 0：G711A 编码，64000bps 码率，8000Hz / 单声道 / 1024 采样点 |
| `ai_to_aenc()` | 绑定 AI→AENC，将采集的原始 PCM 音频自动送入 G711A 编码器 |
| `aenc_call()` | 注册 AENC 通道 0 的输出回调 `aenc_call_fun`，每编码完一帧音频触发一次 |
| `aenc_call_fun(MEDIA_BUFFER mb)` | AENC 编码回调函数。将编码后的 G711A 音频帧通过 RTSP 推流出去（`rtsp_tx_audio`），同时驱动 `rtsp_do_event` |
| `ao_set()` | 配置 AO 通道 1：通过 `rockchipi2s0sou` I2S 接口输出音频，8000Hz / 单声道 / S16（当前已注释） |
| `ai_to_ao()` | 绑定 AI→AO，实现麦克风→扬声器的音频直通环回（当前已注释） |
| `adec_set()` | 创建 ADEC 通道 0：G711A 解码，8000Hz / 单声道（当前已注释） |
| `adec_to_ao()` | 绑定 ADEC→AO，将解码后的音频输出到扬声器（当前已注释） |
| `test()` | 从文件 `./9203` 循环读取 G711A 编码数据，封装为 `MEDIA_BUFFER` 送入 ADEC 解码后播放（当前已注释） |

#  0706 新添加

- 音频编码格式：mp3 mp2 g711a
- 需要使用编码器是我adio中的 `aenc_ai_set`函数
- 使用的具体接口和参数见代码
- 注意编码格式 使用的是g711a
- 然后是将ai和aenc进行连接
- 回调函数
- 注意解码的采样率 = 8000 采样点个数 = 1024
- 



# 0706 个人认为的重点

## 推流
1.常见的推流协议：
- RTMP（实时消息传输协议）：最主流的推流协议。OBS、ffmpeg 等工具推流到直播平台（如斗鱼、虎牙、Twitch）
几乎都使用 RTMP。
- SRT（安全可靠传输）：远程制作、跨地域传输、信号回传、在弱网环境下需要高质量稳定传输的场景。像跨
国直播、专业广电推流
- WebRTC（网页实时通信）：超低延迟互动直播，如电商直播、在线教育、视频会议、秀场连麦
- RTSP/RTP：在安防监控、IP 摄像头领域是绝对的主流协议。
  
2.首先就是需要有一个初始化函数 是media的init_rtsp函数 .因为本项目是监控 所以选择了rtsp

3.具体的函数寻找要看官方的例程 在/home/hxy/RV1126_RV1109_LINUX_SDK_V2.2.5.1_20231011/external/rkmedia/examples/ 找带有rtsp的文件

4.根据例程写函数 此时就需要写到回调函数中了 在进行完编码后的回调函数中去存

5.回调函数类似中断 

6.再rga后再推流 可以做到 但是我要过一边逻辑
  - 首先是正常的ai ，venc ，rga
  - ai 和 venc之间多了一道 rga工序
  - 把经过rga转变后的图像给venc
  - 在此项目中有多个连接函数
  - vi_to_rga_to_venc(); //VI → RGA → VENC 系统自动转发
  - vi_rga_bind_register_cb();//VI → RGA → callback 拦截 → 手动发 VENC 

# 7月7日更新
## 感觉有点烧脑，但是不是很难
1. 首先今天整了一个qt界面，简单整一下qt界面的函数
    - `libvlc_new` 创建一个vlc实例
    - `libvlc_media_new_location` 创建vlc媒体 rtsp://192.168.100.75/9203
    - `libvlc_media_player_new_from_media`//创建媒体播放器
    
    - ` 播放函数libvlc_media_player_play(player);`
    - `//指定播放窗口libvlc_media_player_set_xwindow(player,ui->label->winId());`

2. 然后是popen函数可以在代码中使用命令行命令 
3. 套接字，从网上的服务器获取服务 要复习了
4. 同时进行推流和封装操作。
   - vi到venc（推流） vi到venc（封装）要用不同的通道号。
   - 封装主要还是函数结构体的参数
5. 对于板子上wifi的设置
   - 首先找到8188eu.ko的位置 把它送到板子上 然后挂载
   - 使用`ifconfig`然后查看是否会有新的网卡
   - 若是没有，使用`ifconfig wlan0 up`
   - `vi /etc/wpa_supplicant.conf` 去修改文档内容，修改名字密码
   - `wpa_supplicant -D wext -c /etc/wpa_supplicant.conf -i wlan0 &`连接热点
   - `udhcpc -i wlan0`通过 wlan0 这个无线网卡接口，向网络中的 DHCP 服务器发送请求，自动获取 IP 地址。 也就是让 Wi-Fi
  网卡自动获取 IP（IP 地址、子网掩码、默认网关、DNS 等）。这是让ifconfig的ip显示出来
6. 还有vm的的网络设置，先在虚拟网络编辑器中修改桥接模式的网卡信息
7. 在linux中修改网络连接。

# 对于项目到现在的一点总结（会更新）

## 对于代码的编写
1. 我觉得音视频的开发有相似性 都会有一个设置通道属性，使能，启动的函数，然后就是编码，连接。
2. 音视频开发中会用到回调函数，对于回调函数要了解。
3. 然后就是在设置编码格式时结构体中所要填写的数据要懂。
4. 添加了套接字


## 好像没了 想起来再写
