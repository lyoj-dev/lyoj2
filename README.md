<p align="center"><img src="https://github-raw.littleyang.ml/LittleYang0531/image/main/lyoj/logo.png"></p>

<p><center>新一代跨平台在线评测系统，使用 C++ 进行构建。</center></p>

-----

(文件夹已经建好了，别催了，别催了......)

## 功能简介

1. 全程使用 C++ 进行开发，省去了 v1 版本部署 PHP8.0 和 nginx 的复杂步骤。
2. 使用线程池技术，提高系统的并发能力。
3. 使用 WebSocket 技术作为与服务端之间的沟通桥梁，杜绝了由于 Ajax 轮询导致的网站卡顿。
4. 将提交记录的代码与结果记录到文件而不是数据库里，提高了访问的速度。
5. 支持单机多线程测评，提高评测效率。
6. 更新中...

## 效率问题

### WebSever Core 效率

WebServer Core & WebSocket Server Core 原仓库地址: https://github.com/lyoj-dev/webserver

**amd64 环境**

评测系统: Windows 10

CPU 型号: Intel(R) Core(TM) i3-9100F CPU @ 3.60GHz

运行内存: 8GB

是否关闭日志输出: 是

并发数: 10500

QPS: 15168

读写速度: 2.66MB/s

下面是测试截图:

![](https://github-raw.littleyang.ml/LittleYang0531/image/main/lyoj/speedtest-screenshot2.png)

**arm64 环境**

评测系统: Ubuntu 20.04 LTS

CPU 型号: Qualcomm Snapdragon 765G @ 2.40GHz

运行内存: 8GB + 3GB(内存扩展)

是否关闭日志输出: 是

并发数: 10500

QPS: 7044+

读写速度: 1.27MB/s

(由于本次评测时 WebBench 与 WebServer Core 均在同一台机器上，实际的 QPS 可能要略大于 7044。)

下面是测试截图:

![](https://github-raw.littleyang.ml/LittleYang0531/image/main/lyoj/speedtest-screenshot.png)


### 评测机效率

经过多系统，多架构的平台测试，我们发现

1. arm64 平台效率远小于 amd64 平台效率
2. Windows 平台效率略小于 Linux 平台效率

因此我们建议在 Linux amd64 平台下进行系统搭建。

## 部署指南

### 安装数据库

lyoj 目前暂时**只支持 MySQL/MariaDB**，请根据情况自行选择。

### 安装系统依赖

lyoj 目前**只支持使用 g++** 进行编译，clang 和 MSVC 都无法编译。

对于 `Debian/Ubuntu` 系统: `sudo apt install libmysqlclient-dev libjsoncpp-dev openssl g++ -y`

对于 `CentOS` 系统: `sudo yum install jsoncpp-devel openssl-devel gcc gcc-c++ -y`

对于 `ArchLinux` 系统: `sudo pacman -S jsoncpp mysql++ openssl gcc --noconfirm`

对于 `Windows` 系统，下载 [windows-lib.zip](https://github.com/lyoj-dev/lyoj2/releases/download/winlibs/windows-lib.zip)，解压后将 `dll/*.dll` 文件放到 `C:/Windows/System32` 目录下，将 `lib/*.a` 文件放到 `MinGW目录/lib` 目录下，将 `include` 里的所有文件夹放到 `MinGW目录/include/c++/版本号` 目录下