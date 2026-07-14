# RV1126/RV1109 音视频监控 Demo

这是一个基于 Rockchip RKMedia 的音视频监控测试工程，主要实现摄像头采集、H.264 编码、双 RTSP 推流、RGA 缩放转码、G711A 音频推流，以及 TCP 远程对讲。

## 当前功能

### 音频
- **音频采集 (AI)**：从 `default:CARD=Device` 采集 8000Hz / 单声道 / S16 音频。
- **音频编码 (AENC)**：创建 G711A 编码通道，当前 `AENC ch0` 用于 RTSP 音频推流，`AENC ch1` 预留给 FLV 封装。
- **双路 RTSP 音频推送**：`aenc_call_fun()` 会把 G711A 音频同时推到主码流和子码流 RTSP 会话。
- **远程对讲（网络通话）**：板端启动 TCP Server，监听 `6556` 端口，接收客户端发来的 AAC 音频流，使用 FFmpeg `libfdk_aac` 解码后送到 AO 播放。
- **音频直通 / 文件解码测试**：`ai_to_ao()`、`test()`、`aac_dec_ao()` 等函数保留为调试代码，当前主流程未启用。

### 视频
- **视频采集 (VI)**：从 ISP `rkispp_scale0` 节点采集 1920×1080 NV12 图像。
- **主码流编码**：`VENC ch0` 编码 1920×1080 H.264，用于主 RTSP。
- **子码流编码**：`RGA ch0` 将 1920×1080 NV12 转成 640×480 BGR888，再送入 `VENC ch1` 编码 H.264，用于子 RTSP。
- **封装预留通道**：`venc_set()` 同时创建 `VENC ch2`，供 FLV 封装使用。
- **OpenCV 叠字调试路径**：`rga_OutCbFunc()` 可以在 RGA 回调里写入 `"hxy"`，但当前主流程走 `vi_to_rga_to_venc()` 自动转发路径，不会经过该回调。

### 推流与封装
- **主 RTSP**：端口 `554`，路径 `/live/main_stream`，推送 1080p H.264 + G711A 音频。
- **子 RTSP**：端口 `553`，路径 `/live/son_stream`，推送 640×480 H.264 + G711A 音频。
- **FLV 封装存储**：`muxer_set()` 已实现，保存到 `/mnt/sdcard`，按 10 秒切片；但 `main.cpp` 里 `pthread_create(&tid_save, ...)` 当前被注释，默认不启动封装线程。

## 硬件平台

- **芯片**：Rockchip RV1126 / RV1109。
- **系统**：Buildroot Linux（RV1126_RV1109_LINUX_SDK_V2.2.5.1）。
- **架构**：ARM (arm-linux-gnueabihf)。

## 项目结构

```
.
├── CMakeLists.txt          # CMake 构建配置（交叉编译）
├── build/                  # 构建输出目录
│   └── mymain              # 编译生成的可执行文件
├── 1.c                     # PC/Ubuntu 端 TCP 对讲客户端测试程序
├── jiance.c                # 板端 WiFi 初始化 + mymain 守护启动程序
├── include/                # 头文件
│   ├── common.h            # 公共头文件（RKMedia SDK、OpenCV、RTSP、FFmpeg 等）
│   ├── hxy_audio.h         # 音频功能声明
│   ├── hxy_media.h         # 视频/媒体功能声明
│   └── fengzhuang.h        # 封装模块声明
├── src/                    # 源文件
│   ├── main.cpp            # 程序入口（双 RTSP + RGA + 远程对讲线程）
│   ├── hxy_adio.cpp        # 音频采集/编码/解码/网络对讲实现
│   ├── hxy_media.cpp       # 视频采集/编码/RGA/RTSP 实现
│   └── fengzhuang.cpp      # FLV 封装存储实现
└── qt/                     # Qt 桌面客户端（VLC 拉流播放）
    └── untitled/           # Qt 项目源码
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

RTSP 拉流地址：

```text
rtsp://<板子IP>:554/live/main_stream
rtsp://<板子IP>:553/live/son_stream
```

## 主要 API 调用流程

### 当前主流程（双 RTSP 推流 + RGA 子码流 + 远程对讲）

```
RK_MPI_SYS_Init()                       # 系统初始化
    ├── SAMPLE_COMM_ISP_Init()          # ISP 初始化
    ├── SAMPLE_COMM_ISP_Run()           # ISP 启动
    ├── SAMPLE_COMM_ISP_SetFrameRate()  # 设置帧率 30fps
    │
    ├── son_init_rtsp()                 # 子 RTSP：553/live/son_stream
    ├── main_init_rtsp()                # 主 RTSP：554/live/main_stream
    │
    ├── vi_set()                        # 配置 VI 通道 0（1920×1080 NV12）
    ├── venc_set()                      # 创建 VENC ch0 + ch2（主推流 + 封装预留）
    ├── vi_to_venc()                    # 绑定 VI → VENC ch0
    ├── venc_main_reg()                 # 注册 VENC ch0 回调 → 主 RTSP 推视频流
    │
    ├── rga_venc()                      # 创建 RGA 通道 0（NV12→BGR888, 1080p→480p）
    ├── venc_son_set()                  # 创建 VENC ch1（640×480 BGR888 → 子 RTSP）
    ├── vi_to_rga_to_venc()             # 绑定 VI → RGA → VENC ch1（系统自动转发）
    ├── venc_son_reg()                  # 注册 VENC ch1 回调 → 子 RTSP 推视频流
    │
    ├── ai_set()                        # 配置 AI 通道 0（8000Hz / 单声道）
    ├── aenc_ai_set()                   # 创建 AENC ch0 + ch1（G711A）
    ├── ai_to_aenc()                    # 绑定 AI → AENC ch0
    ├── aenc_call()                     # 注册 AENC ch0 回调 → 双 RTSP 推音频流
    │
    └── pthread_create(talk_fun)        # 启动远程对讲线程（TCP Server :6556）
```

`save_fun()` 封装线程当前没有启动，如果需要 SD 卡 FLV 存储，需要恢复 `pthread_create(&tid_save, NULL, save_fun, NULL);`。

### 架构说明

项目当前采用 **多通道并发架构**：

| 通道 | 视频路径 | 音频路径 | 用途 |
|------|---------|---------|------|
| ch0 | VI → VENC ch0 → 主 RTSP | AI → AENC ch0 → 主/子 RTSP | 主 RTSP（1080p） |
| ch1 | VI → RGA → VENC ch1 → 子 RTSP | 同上 | 子 RTSP（640×480） |
| ch2 | VI → VENC ch2 → MUXER（预留） | AI → AENC ch1 → MUXER（预留） | FLV 封装，当前默认未启动 |

### RGA 路径（当前已启用，系统自动转发）

```
vi_set() → rga_venc() → vi_to_rga_to_venc()    # 自动路径：VI → RGA → VENC ch1 全自动转发
vi_set() → rga_venc() → vi_rga_bind_register_cb()  # 拦截路径：VI → RGA → 回调 → 手动送 VENC（可插入 OpenCV 处理）
```

当前使用 `vi_to_rga_to_venc()` 自动转发路径。

### 远程对讲（网络通话）

TCP Server 监听端口 6556，接收客户端发送的 AAC 编码音频流：
```
socket() → bind() → listen() → accept()
    → FFmpeg libfdk_aac 解码 → AO (default:CARD=Camera) 扬声器播放
```

### SD 卡封装存储（线程检测）

`save_fun` 已实现 SD 卡检测和封装逻辑，但当前线程创建代码被注释。启用后会循环检测 SD 卡状态：

```
检测 /sys/block/mmcblk*/device/type → 确认 SD 卡在位
    → df -h 检查磁盘使用率
    → 当前代码中使用率 <= 1 时启动 FLV 封装（10 秒分割）
    → 使用率 > 1 时删除最旧文件
```

## 函数功能说明

### `src/main.cpp` — 程序入口

| 函数 | 说明 |
|------|------|
| `main()` | 程序入口。依次初始化系统、ISP、双 RTSP、VI、VENC、RGA、AI、AENC，绑定主/子码流，启动远程对讲线程，最后进入循环等待 |
| `sig_fun(int)` | 信号处理函数，捕获 Ctrl+C（SIGINT），停止 ISP 并关闭 VI 通道 0 后退出 |
| `delete_oldest_file_if_full()` | SD 卡空间不足时自动删除 `/mnt/sdcard` 下最旧的文件 |
| `save_fun(void*)` | SD 卡检测与封装线程。函数已实现，但当前主流程没有创建该线程 |
| `talk_fun(void*)` | 远程对讲线程入口，调用 `tonghua()` 启动 TCP 对讲服务 |

### `src/hxy_media.cpp` — 视频采集 / 编码 / RGA / RTSP

| 函数 | 说明 |
|------|------|
| `vi_set()` | 配置 VI 通道 0：从 `rkispp_scale0` 节点获取 1920×1080 NV12 图像，缓存帧数 2，MMAP 模式，使能并启动流 |
| `vi_get_frame()` | 从 VI 通道获取一帧原始数据并写入文件（调试用，当前未使用） |
| `vi_Outcb_fun(MEDIA_BUFFER ch)` | VI 通道输出回调（调试用），打印获取到的帧数据大小 |
| `vi_cd()` | 注册 VI 通道 0 的输出回调 `vi_Outcb_fun`（调试用，当前未使用） |
| `venc_set(IMAGE_TYPE_E image, int w, int h)` | 创建 VENC 通道 **2 和 0**：H.264 编码，VBR 码率控制，30fps，GOP=30，Profile=77。ch0 供主 RTSP 使用，ch2 供封装预留 |
| `venc_son_set(IMAGE_TYPE_E image, int w, int h)` | 创建 VENC 通道 **1**：H.264 编码，参数同上。接收 RGA 处理后的 640×480 BGR888 图像，供子 RTSP 推流使用 |
| `vi_venc_main_fun(MEDIA_BUFFER ch)` | 主码流 VENC 回调，将 `VENC ch0` 的 H.264 数据推到 `/live/main_stream` |
| `vi_venc_son_fun(MEDIA_BUFFER ch)` | 子码流 VENC 回调，将 `VENC ch1` 的 H.264 数据推到 `/live/son_stream` |
| `venc_main_reg()` | 注册 VENC 通道 **0** 的输出回调 `vi_venc_main_fun` |
| `venc_son_reg()` | 注册 VENC 通道 **1** 的输出回调 `vi_venc_son_fun` |
| `venc_fengzhaung_reg()` | 头文件中保留声明，源码中对应实现当前被注释 |
| `vi_to_venc()` | 绑定 VI → VENC 通道 **0**，系统自动将 VI 采集的帧送入编码器 ch0（供主 RTSP 推流使用） |
| `vi_todengzhuang_venc()` | 绑定 VI → VENC 通道（供 FLV 封装使用，由 SD 卡检测线程调用） |
| `rga_venc()` | 创建 RGA 通道 0：NV12→BGR888 色彩转换 + 1920×1080→640×480 缩放，启用缓存池（3 帧） |
| `rga_OutCbFunc(MEDIA_BUFFER mb)` | RGA 回调函数。RGA 处理完成后触发，用 OpenCV 在帧上绘制 "hxy" 文字标注，再手动 `SendMediaBuffer` 送入 VENC |
| `vi_rga_bind_register_cb()` | 绑定 VI→RGA 并注册 RGA 回调。走 **拦截路径**：VI 帧经 RGA 转换后回到回调，由回调手动送入 VENC（可插入 OpenCV 处理） |
| `vi_to_rga_to_venc()` | 绑定 VI→RGA→VENC **系统自动转发路径**。帧自动流转，无回调拦截，无法插入 OpenCV 处理。**当前已启用**，将 RGA 处理后图像送入 VENC ch1 供子 RTSP 推流 |
| `main_init_rtsp()` | 初始化主 RTSP 服务：端口 `554`，路径 `/live/main_stream`，传输 1920×1080 H.264 视频 + G711A 音频 |
| `son_init_rtsp()` | 初始化子 RTSP 服务：端口 `553`，路径 `/live/son_stream`，传输 640×480 H.264 视频 + G711A 音频 |

### `src/hxy_adio.cpp` — 音频采集 / 编码 / 解码 / 播放

| 函数 | 说明 |
|------|------|
| `ai_set()` | 配置 AI 通道 0：从 `default:CARD=Device` 采集音频，8000Hz / 单声道 / S16 / 每帧 1024 采样点，使能并启动流 |
| `ao_set()` | 配置 AO 通道 1：通过 `default:CARD=rockchipi2s0sou` 输出 48000Hz / 双声道 / S16 音频（当前主流程未启用） |
| `ai_to_ao()` | 绑定 AI→AO，实现麦克风→扬声器的音频直通环回（当前已注释） |
| `aenc_ai_set()` | 创建 AENC 通道 **0 和 1**：G711A 编码，64000bps 码率，8000Hz / 单声道 / 1024 采样点。ch0 供 RTSP 推流用，ch1 供封装用 |
| `ai_to_aenc()` | 绑定 AI → AENC 通道 **0**，将 PCM 音频送入编码器 ch0（供 RTSP 推流使用） |
| `ai_tofengzhuang_aenc()` | 绑定 AI → AENC 通道 **1**，将 PCM 音频送入编码器 ch1（供 FLV 封装使用） |
| `aenc_call_fun(MEDIA_BUFFER mb)` | AENC 编码回调函数。将编码后的 G711A 音频帧通过 **双 RTSP** 推流（同时向 `g_rtsplive` 和 `g_rtsplive1` 推送），并驱动 RTSP 事件 |
| `aenc_call()` | 注册 AENC 通道 **0** 的输出回调 `aenc_call_fun`，每编码完一帧音频触发一次双 RTSP 推流 |
| `adec_set()` | 创建 ADEC 通道 0：G711A 解码，48000Hz / 双声道（当前已注释） |
| `adec_to_ao()` | 绑定 ADEC→AO，将解码后的音频输出到扬声器（当前已注释） |
| `test()` | 从文件 `./9203` 循环读取 G711A 编码数据，封装为 `MEDIA_BUFFER` 送入 ADEC 解码后播放（当前已注释） |
| `tonghua()` | **远程对讲服务端**。创建 TCP Server（端口 6556），接收客户端 AAC 音频流 → FFmpeg libfdk_aac 解码 → AO（`default:CARD=Camera`）扬声器播放。支持循环接受多个客户端连接 |
| `aac_dec_ao()` | 从文件 `/201/0708ffmpeg/test.aac` 读取 AAC 音频 → FFmpeg libfdk_aac 解码 → AO 播放（调试/测试用，当前主流程未启用） |
| `ai_ff_aenc()` | 配置 AI 通道 + FFmpeg libfdk_aac 软件编码器，替代 G711A 硬编码。注册 AI 回调 `ai_ff_fun`，采集的 PCM 经 FFmpeg 编码为 AAC 后推流到双 RTSP |
| `ai_ff_fun(MEDIA_BUFFER mb)` | AI 回调函数（配合 `ai_ff_aenc` 使用）。将采集的 PCM 帧送 FFmpeg AAC 编码，编码后推流到双 RTSP |

### `src/fengzhuang.cpp` — FLV 封装存储

| 函数 | 说明 |
|------|------|
| `muxer_set()` | 配置 FLV 封装器（MUXER）。FLV 格式，按时间分割（每 10 秒一个文件），自动命名带时间戳，文件前缀 `hxy`，起始索引 1，存储路径 `/mnt/sdcard`。绑定 VENC ch2（H.264 视频）+ AENC ch1（G711A 音频），启动封装流 |

### `qt/untitled/` — Qt 桌面拉流客户端

| 函数/接口 | 说明 |
|------|------|
| `libvlc_new()` | 创建 VLC 实例 |
| `libvlc_media_new_location()` | 创建 VLC 媒体，当前 Qt 代码里写死的 RTSP 地址需要按板子 IP 和实际路径修改 |
| `libvlc_media_player_new_from_media()` | 从媒体创建播放器 |
| `libvlc_media_player_play()` | 开始播放 RTSP 流 |
| `libvlc_media_player_set_xwindow()` | 指定播放窗口（绑定到 Qt 的 `QLabel` 控件） |

## 说明

- 这是一个学习/测试性质的 demo 项目，用于熟悉 RKMedia API 的音频、视频、RGA、编码器和封装器模块
- `main.cpp` 当前启用 **双 RTSP 推流 + RGA 子码流 + TCP 远程对讲**。
- 视频编码使用 **H.264**（非 H.265），`VENC ch0` 用于主码流，`VENC ch1` 用于 RGA 子码流，`VENC ch2` 预留给封装。
- RGA 当前使用 **系统自动转发路径**（`vi_to_rga_to_venc`），NV12→BGR888 色彩转换 + 1080p→480p 缩放后通过子 RTSP 推流
- 音频支持两种编码路径：G711A 硬编码（`aenc_ai_set`）和 FFmpeg libfdk_aac 软件编码（`ai_ff_aenc`），当前主流程使用 G711A。
- 远程对讲使用 TCP + FFmpeg libfdk_aac 解码 → AO 扬声器输出。
- FLV 封装相关函数已实现，但保存线程当前在 `main.cpp` 中被注释，默认不启用。
- 编译产物 `build/mymain` 已交叉编译为 ARM 架构，无法在 x86 主机上直接运行
- SDK 路径中的 `RV1126_RV1109_LINUX_SDK_V2.2.5.1_20231011` 是示例版本号，其他版本可能需要调整头文件和库的路径

## 根目录独立测试程序

这一部分说明根目录 `1.c` 和 `jiance.c`，它们和主工程 `src/`、`include/` 里的 CMake 项目分开看。

### `1.c`

`1.c` 是远程对讲客户端测试程序，主要作用是从本机麦克风采集音频，使用 FFmpeg `libfdk_aac` 编码成 AAC，然后通过 TCP 发送到板端服务。

主要流程：

1. 创建 TCP 客户端 socket。
2. 连接板端服务器：`192.168.100.75:6556`。
3. 使用 FFmpeg 注册音频设备，并通过 ALSA 打开默认音频输入设备 `default`。
4. 设置采集参数：采样率 `48000`、声道数 `2`、采样格式 `s16`。
5. 创建 AAC 编码器 `libfdk_aac`。
6. 循环读取麦克风数据，拼成 `AVFrame`。
7. 调用 `avcodec_send_frame()` / `avcodec_receive_packet()` 编码。
8. 将编码后的 AAC 数据通过 `write()` 写入 TCP 套接字。

用途：

- 配合板端 `tonghua()` TCP 对讲服务测试远程语音输入。
- 验证 Ubuntu/PC 端采集、AAC 编码、网络发送链路。

注意：

- 运行前要保证板端服务已经监听 `6556` 端口。
- `192.168.100.75` 需要改成实际板子的 IP。
- 编译时需要链接 FFmpeg 相关库和 `libfdk_aac`。

参考编译方式：

```bash
gcc 1.c -o talk_client \
  -lavformat -lavdevice -lavcodec -lavutil -lfdk-aac
```

### `jiance.c`

`jiance.c` 是板端启动和检测守护程序，主要作用是先初始化 WiFi，再启动主程序 `mymain`，并根据有线网口状态控制主程序暂停、继续或异常重启。

主要流程：

1. fork 子进程初始化 WiFi：加载 `/201/external/8188eu.ko`，启动 `wlan0`，使用 `wpa_supplicant` 连接热点，通过 `udhcpc` 获取 IP。
2. 使用 `ping www.baidu.com` 判断 WiFi 是否联网成功。
3. 如果 WiFi 初始化成功，运行 `/201/external/a.out` 用于同步时间。
4. 循环启动主程序：`/201/0701rkmidea/build/mymain`。
5. 子进程持续检测 `/sys/class/net/eth0/carrier`。
6. 网线连接正常时向 `mymain` 发送 `SIGCONT`，网线断开时发送 `SIGSTOP`。
7. 如果 `mymain` 异常退出，等待 1 秒后自动重启；如果正常返回 0，则不再重启。

注意：

- 文件里的路径是板端实际部署路径，换目录后需要同步修改：
  - `/201/external/8188eu.ko`
  - `/201/external/a.out`
  - `/201/0701rkmidea/build/mymain`
- `jiance.c` 更像是部署脚本/守护进程，不属于当前 CMake 主工程。

参考编译方式：

```bash
gcc jiance.c -o jiance
```

---

# 0706 新添加

- 音频编码格式：mp3 mp2 g711a
- 需要使用编码器是我adio中的 `aenc_ai_set`函数
- 使用的具体接口和参数见代码
- 注意编码格式 使用的是g711a
- 然后是将ai和aenc进行连接
- 回调函数
- 注意解码的采样率 = 8000 采样点个数 = 1024



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

# 对于ffmpeg在ubuntu的一个移植
1. 首先下载ffmpeg的库
2. 解压tar -xvf ffmpeg-4.3.6.tar.xz
3. 在解压后的文件夹下./configure --target-os=linux --prefix=/home/jinli/ffmpeg_install --arch=x86_64 --disable-doc --enable-libx264 --enable-libmp3lame --enable-libopus --enable-debug=3 --extra-cflags="" --extra-ldflags="" --enable-alsa   --enable-gpl --enable-opengl --enable-sdl2 --enable-avdevice --enable-indev=v4l2  --enable-shared --disable-static --enable-nonfree --enable-libfdk-aac 进行配置
4. make -j16 && make install -j16进行编译
   

# 对于项目到现在的一点总结（会更新）

## 对于代码的编写
1. 我觉得音视频的开发有相似性 都会有一个设置通道属性，使能，启动的函数，然后就是编码，连接。
2. 音视频开发中会用到回调函数，对于回调函数要了解。
3. 然后就是在设置编码格式时结构体中所要填写的数据要懂。
4. 添加了套接字

## 对于项目的一些看法
最近在整合项目，遇到了一些问题整理一下
1. 封装，推流，初始的ai设置 这些功能都要用到ai通道，但是ai只有一个通道。所以只能迁就封装，让封装能跑起来，这就需要改变ai的设置和aenc的设置。一个管道对应一个设置。要记好对应管道号所对应的设置
2. 使用ctrl+c结束一个进程时，如果该进程有子进程。那么子进程就会变成僵尸进程。可以使用线程。
3. 对于网络编程，主要是实现一个远程对话的功能，因为我们可以拉流，而且我选择将tcp服务器部署在板端。板端只需要从tcp的服务器端的套接字中拿到音频文件，然后送给解码器，再给ao（喇叭.），同样的，客户端只需要将读取到的麦克风的音频文件传到套接字就好
4. popen很好用


## 好像没了 想起来再写
