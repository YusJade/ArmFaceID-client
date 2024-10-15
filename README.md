# ArmFaceID-client

原 `ArmFaceID` 的客户端转为多人协作开发，该客户端运行在`aarch64`架构的嵌入式设备上，集成`SeetaFace6`的各个模块，以增强客户端的可靠性。

<a href="https://github.com/YusJade/ArmFaceID?tab=readme-ov-file">
  <img src="https://github-readme-stats.vercel.app/api/pin/?username=YusJade&repo=ArmFaceID&theme=default" alt="YusJade/ArmFaceID" />
</a>

## 协同开发约定

- 使用 `GitHub` + `git` 进行版本管理。
  - [👑Git 关卡式学习](https://learngitbranching.js.org/?locale=zh_CN) 
- 使用 `Clion` 作为集成开发环境 `IDE`。
- 采用 `Google Cpp` 作为编码规范（会采用 `clang-format` 作为代码自动格式化工具）。
  - [📖Google 开源项目风格指南 - Cpp](https://zh-google-styleguide.readthedocs.io/en/latest/google-cpp-styleguide/contents.html)
- 使用 `CMake` 构建工程。
  - [📖CMake 保姆级教程（上）](https://subingwen.cn/cmake/CMake-primer/)
  - [📖CMake 保姆级教程（下）](https://subingwen.cn/cmake/CMake-advanced/?highlight=cmake)

## 依赖库以及交叉编译清单

以下是开发中所需的库，可自行在主机编译`x86`以及`aarch64`版本，下文也会给出库编译后的下载链接。

**通过 apt 安装的库**
- [ ] `Qt6.5`： 跨平台`C++`应用开发框架。

**编译安装**
- [x] `SeetaFace6`：人脸算法。
- [x] `OpenCV 4.10`：用于调取摄像头，进行图像处理。
- [x] `gRPC 1.67.0-dev`：用于RPC 网络通信。
- [x] `Protobuf 5.27.2`：数据的序列化和反序列化。（作为第三方库与`gRPC 1.67.0-dev`一同编译）

完成交叉编译的库文件 👉 [下载](https://github.com/YusJade/ArmFaceID-client/releases)

## 开发目标

- 集成 `SeetaFace6` 的人脸检测模块、活体检测模块、人脸质量评估模块，提高客户端的可靠性。
- 使用 `gRPC` 框架进行网络通信，将人脸识别请求发送至服务端，并处理服务端回传的结果。
- 有清晰易用的用户图形界面`GUI`，向用户展示识别结果、提示用户完成人脸识别流程。
- 使用某种优化方法，降低向服务端发送人脸识别请求的频率。

<img src="./assets/frame.png" width=70%>


### 目标 1：使用 `gRPC` 框架进行网络通信

**负责开发：**
<a href="https://github.com/YusJade" ><img src="https://github.com/yusjade.png" width="50" height="50" style="border-radius: 50%; border: 2px solid #333;"/></a>


**何为 `gRPC` ？：**
`RPC` 即远程过程调用（Remote Procedure Call），可以使得客户端程序调用服务端功能变得像调用本地的方法一样， `gRPC`是谷歌开发的一套实现`RPC`的框架。

**如何开始开发`gRPC`：**
获得编译后的`gRPC`库后，需要为客户端和服务端定义一套“协议”。它用于约定二者通讯时的消息格式以及可用的服务，这个“协议”需要写在一个`.proto`文件中，随后就可以使用`gRPC`库中提供的编译器将其编译为我们需要的`.cpp/.h`文件。

**如何开发`gRPC`：**

> `gRPC` 文档 🚅 [Quick start](https://grpc.io/docs/languages/cpp/quickstart/)

**具体的开发目标：**
- 实现 `RPC` 异步客户端，能够异步地处理响应。
- 调用服务器的人脸识别服务，获取识别结果和关联的用户信息。

### 目标 2：使用 `Qt` 搭建用户图形界面

**负责开发：**
<a href="https://github.com/BorderArea01" ><img src="https://github.com/BorderArea01.png" width="50" height="50" style="border-radius: 50%; border: 2px solid #333;"/></a><a href="https://github.com/YusJade" ><img src="https://github.com/yusjade.png" width="50" height="50" style="border-radius: 50%; border: 2px solid #333;"/></a>

<img src="./assets/ui_desc.png" width=70%>

- 界面简洁明了，功能指示明确。
- 使用纯代码搭建界面，方便维护。
- 合理使用设计模式与`Qt`槽/信号机制，保持界面逻辑与业务逻辑的低耦合。

### 目标 3：集成人脸检测模块（前置）

> `SeetaFace6` [官方教程文档](https://github.com/seetafaceengine/SeetaFaceTutorial)

输入图像，调用`FaceDetector`提取其中人脸信息`SeetaFaceInfoArray`。

> 人脸检测的功能实现起来比较简单，而且是后两个目标的前置，故不作为一个独立任务来分配。

### 目标 4：集成活体检测模块

**负责开发：**
<a href="https://github.com/idealoong" ><img src="https://github.com/idealoong.png" width="50" height="50" style="border-radius: 50%; border: 2px solid #333;"/></a>

> `SeetaFace6` [官方教程文档](https://github.com/seetafaceengine/SeetaFaceTutorial)

输入图像和其中的人脸信息，调用`FaceAntiSpoofing`判别其中人脸是否为二次呈像，以及图像中是否出现平板、手机、照片等攻击介质。

### 目标 5：集成人脸质量评估模块

**负责开发：**
<a href="https://github.com/Serendipity-hjn" ><img src="https://github.com/Serendipity-hjn.png" width="50" height="50" style="border-radius: 50%; border: 2px solid #333;"/></a>

> `SeetaFace6` [官方教程文档](https://github.com/seetafaceengine/SeetaFaceTutorial)

 判断输入图像的人脸是否完整、是否为正脸，以及检查脸部图像的分辨率是否符合要求。

