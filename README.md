<p align="center"><img src="https://github-raw.littleyang.ml/LittleYang0531/image/main/lyoj/logo.png"></p>

<center>新一代跨平台在线评测系统，使用 C++ 进行构建。</center>

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

WebServer Core & WebSocket Server Core 原仓库地址: https://github.com/lyoj-dev/webserver

在 **Ubuntu 20.04 LTS arm64** 的测试环境下，关闭日志输出并使用 WebBench 进行上万的并发连接，能够做到约 7044 的 QPS 以及 1.27MB 的读写速度。

由于评测时 WebBench 与 WebServer Core 均在同一台机器上，实际的 QPS 可能要略大于 7044。

下面是测试截图:

![](https://raw.fastgit.org/LittleYang0531/image/main/lyoj/speedtest-screenshot.png)