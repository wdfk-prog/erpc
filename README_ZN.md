# eRPC RT-Thread 矩阵乘法 UART 示例使用说明

[English](README.md) | [中文](README_ZN.md)

## 说明

本示例用于在 RT-Thread 设备端启动一个 eRPC 矩阵乘法 server，并通过 RT-Thread UART transport 与 PC 端 Python client 通信。

RT-Thread 端只编译和运行 server。PC 端 Python client 只在主机上运行，不会进入 RT-Thread 固件。


## 软件包索引位置

本示例所属 eRPC 软件包建议放在 RT-Thread `packages` 索引仓库的 IoT 分类下：

```text
packages/iot/erpc/
├── Kconfig
└── package.json
```


## RT-Thread 端使用流程

### 1. 确认串口设备

先确认 BSP 已启用目标 UART，并且 RT-Thread 可以看到对应设备名。建议不要复用正在作为 MSH/日志控制台的 UART，避免 shell 输出和 eRPC 帧混在同一串口上。

可在 MSH 中查看设备：

```sh
list_device
```

确认目标串口名，例如：

```text
uart2
```

### 2. menuconfig 启用配置

进入 RT-Thread BSP 工程后执行：

```sh
menuconfig
```

启用 eRPC 包和本示例所需配置：

```text
RT_USING_SERIAL=y
PKG_USING_ERPC=y
PKG_ERPC_USING_SERVER=y
PKG_ERPC_USING_MBF_DYNAMIC=y
PKG_ERPC_USING_TRANSPORT_RTTHREAD_UART=y
PKG_ERPC_USING_EXAMPLE_MATRIX_MULTIPLY_UART=y
```

配置 UART 设备名和波特率，必须与实际板端连接和 PC 端 client 参数一致：

```text
PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_DEV_NAME="uart2"
PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_BAUDRATE=115200
```

可按板端资源调整示例 server 线程参数：

```text
PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_THREAD_STACK_SIZE=4096
PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_THREAD_PRIORITY=20
PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_THREAD_TIMESLICE=10
```

### 3. 更新软件包并构建固件

保存 `menuconfig` 后，更新软件包：

```sh
pkgs --update
```

然后在 BSP 工程中构建：

```sh
scons -j8
```

如需生成 IDE 工程，可按 BSP 原有流程执行，例如：

```sh
scons --target=mdk5
```

### 4. 构建脚本接入关系

启用 `PKG_ERPC_USING_EXAMPLE_MATRIX_MULTIPLY_UART` 后，`rtthread/SConscript` 会引入：

```text
examples/rtthread/matrix_multiply_uart/SConscript
```

该示例 `SConscript` 会把以下源码加入 RT-Thread 固件构建：

```text
examples/rtthread/matrix_multiply_uart/src/*.cpp
examples/rtthread/matrix_multiply_uart/src/service/*.cpp
```

因此，确认 `.config` 中存在以下配置即可判断示例应进入构建：

```text
PKG_ERPC_USING_EXAMPLE_MATRIX_MULTIPLY_UART=y
```

### 5. 烧录并运行板端 server

烧录固件并复位后，本示例通过 `INIT_APP_EXPORT()` 自动创建 server 线程，不需要手动输入 MSH 命令。

启动成功时，板端日志应出现类似输出：

```text
eRPC matrix multiply UART server started: uart2, 115200
```

如果没有看到该日志，请先检查：

1. `.config` 是否启用了 `PKG_ERPC_USING_EXAMPLE_MATRIX_MULTIPLY_UART`。
2. 目标 UART 设备名是否存在。
3. 线程栈和优先级是否适合当前 BSP。
4. 当前串口是否被控制台、日志或其他业务占用。

## PC 端 client 运行

PC 端复用已有 eRPC Python 矩阵乘法 client。串口号和波特率必须与 RT-Thread 端配置一致。

### Windows PowerShell

```powershell
cd E:\CODE\RTT\erpc
$env:PYTHONPATH="E:\CODE\RTT\erpc\erpc_python;E:\CODE\RTT\erpc\examples\matrix_multiply_python"
python -u examples\matrix_multiply_python\matrix_multiply.py --client --serial COM17 --baud 115200
```

### Linux

```sh
cd /path/to/erpc
export PYTHONPATH="$PWD/erpc_python:$PWD/examples/matrix_multiply_python"
python3 -u examples/matrix_multiply_python/matrix_multiply.py --client --serial /dev/ttyUSB0 --baud 115200
```

## 常见问题

| 现象 | 检查项 |
|---|---|
| 构建后没有示例代码 | 确认 `PKG_ERPC_USING_EXAMPLE_MATRIX_MULTIPLY_UART=y`，并确认 `rtthread/SConscript` 已引入示例 `SConscript`。 |
| 板端提示 UART transport 初始化失败 | 检查 `PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_DEV_NAME` 是否与 `list_device` 中的设备名一致。 |
| PC client 无响应 | 检查波特率、串口号、TX/RX 连接、GND、串口是否被 MSH/日志占用。 |
| 数据异常或超时 | 避免在同一 UART 上同时输出日志和传输 eRPC 数据帧。 |
| 编译缺少 C++ 支持 | `PKG_USING_ERPC` 会选择 RT-Thread C++ 支持；若 BSP 有额外限制，请确认工具链支持 C++ 编译。 |

## 预期结果

PC client 发起矩阵乘法 RPC 请求后，RT-Thread 端 server 计算结果并通过 UART 返回。若串口、波特率和配置一致，client 侧应能正常完成矩阵乘法验证。
