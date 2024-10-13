# ArmFaceID-client

原 ArmFaceID 的客户端转为多人协作开发，该客户端运行在`aarch64`架构的嵌入式设备上，集成`SeetaFace6`的各个模块，以增强客户端的可靠性。

<a href="https://github.com/YusJade/ArmFaceID?tab=readme-ov-file">
  <img src="https://github-readme-stats.vercel.app/api/pin/?username=YusJade&repo=ArmFaceID&theme=default" alt="YusJade/ArmFaceID" />
</a>

## 依赖库以及交叉编译清单

以下是开发中所需的库，可自行在主机编译`x86`以及`aarch64`版本，下文也会给出库编译后的下载链接。

**通过 apt 安装的库**
- [ ] `Qt6.5`： 跨平台`C++`应用开发框架。

**编译安装**
- [ ] `SeetaFace6`：人脸算法。
- [x] `OpenCV 4.10`：用于调取摄像头，进行图像处理。
- [x] `gRPC 1.67.0-dev`：用于RPC 网络通信。
- [x] `Protobuf 5.27.2`：数据的序列化和反序列化。（作为第三方库与`gRPC 1.67.0-dev`一同编译）

完成交叉编译的库文件 [下载](https://github.com/YusJade/ArmFaceID)

## 开发目标

- 集成 `SeetaFace6` 的人脸检测模块、活体检测模块、人脸质量评估模块，提高客户端的可靠性。
- 使用 `gRPC` 框架进行网络通信，将人脸识别请求发送至服务端，并处理服务端回传的结果。
- 有清晰易用的用户图形界面`GUI`，向用户展示识别结果、提示用户完成人脸识别流程。
- 使用某种优化方法，降低向服务端发送人脸识别请求的频率。

<img src="./assets/frame.png" width=70%>


### 目标 1：使用 `gRPC` 框架进行网络通信


### 目标 2：使用 `Qt` 搭建用户图形界面

### 目标 3：集成人脸检测模块

### 目标 4：活体检测模块

### 目标 5：人脸质量评估模块

