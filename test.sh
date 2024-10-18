#!/bin/bash

workdir=$(cd $(dirname $0); pwd)
# 定义一个函数来处理SIGINT信号，终止所有子进程
cleanup() {
    echo "Caught SIGINT signal. Terminating all background jobs..."
    kill -- -$$  # 杀死当前脚本启动的所有进程组中的进程
}

# 在接收到SIGINT信号时调用cleanup函数
trap cleanup SIGINT

# 启动测试程序并在后台运行
$workdir/serial-test -p /dev/ttyCH9344USB1 -b 115200 &
echo "Started rs232 with PID $!"
$workdir/serial-test -p /dev/ttyCH9344USB2 -b 115200 &
echo "Started rs485-1 with PID $!"
$workdir/serial-test -p /dev/ttyCH9344USB3 -b 115200 &
echo "Started rs485-2 with PID $!"
/sbin/ip link set can0 up type can bitrate 125000 restart-ms 5000
$workdir/can-test can0 &
echo "Started can-test with PID $!"

# 等待所有后台任务完成
wait
echo "All tests have completed."
