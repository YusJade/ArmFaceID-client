# CMake 如何指定库的路径

下载并解压预编译的库文件，观察其目录结构，一般在`cmake`或`lib/cmake`中存在配置文件，这里有两种情况：

- `XXXConfig.cmake` 或 `XXX-config.cmake`
- `FindXXX.cmake`
   
**对应两种操作：**
- 在工程顶层目录中新建`path_config.cmake`，写入：
```cmake
set(XXX_DIR /path/to/file)
```
- 在工程顶层目录中新建`path_config.cmake`，写入：
```cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /path/to/file)
```