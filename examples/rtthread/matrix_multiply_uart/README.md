# eRPC RT-Thread matrix multiply UART example

This example starts an eRPC matrix multiply server on an RT-Thread UART device.
It is intended to validate the native RT-Thread UART transport with the existing
eRPC matrix multiply Python client.

## Enable

Enable the package and the example in `menuconfig`:

```text
PKG_USING_ERPC=y
PKG_ERPC_USING_SERVER=y
PKG_ERPC_USING_MBF_DYNAMIC=y
PKG_ERPC_USING_TRANSPORT_RTTHREAD_UART=y
PKG_ERPC_USING_EXAMPLE_MATRIX_MULTIPLY_UART=y
```

Set the UART device and baud rate to match the board connection:

```text
PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_DEV_NAME="uart2"
PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_BAUDRATE=115200
```

## Run

After the board boots, the example uses `INIT_APP_EXPORT()` to create a server
thread automatically. No MSH command is required.

The host side can reuse the existing eRPC Python matrix multiply client. Run it
on the PC and point it at the board UART port.

Windows example:

```sh
cd E:\CODE\RTT\erpc
$env:PYTHONPATH="E:\CODE\RTT\erpc\erpc_python;E:\CODE\RTT\erpc\examples\matrix_multiply_python"
python -u examples\matrix_multiply_python\matrix_multiply.py --client --serial COM17 --baud 115200
```

Linux example:

```sh
cd /path/to/erpc
export PYTHONPATH="$PWD/erpc_python:$PWD/examples/matrix_multiply_python"
python3 -u examples/matrix_multiply_python/matrix_multiply.py --client --serial /dev/ttyUSB0 --baud 115200
```

The Python client runs on the PC only. It is not part of the RT-Thread firmware.
