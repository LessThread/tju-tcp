# 利用UDP实现可靠传输实验代码说明

## 文件结构说明
```
tju_tcp
├─ Makefile ------------- 生成脚本
├─ README.md ------------ 文件说明
├─ src
│  ├─ client.c ---------- 客户端主函数实现
│  ├─ kernel.c ---------- 虚拟内核实现
│  ├─ log.c ------------- 日志实现
│  ├─ operation.c ------- 数据包及缓冲区操作
│  ├─ rtt.c ------------- RTT计算实现
│  ├─ server.c ---------- 服务端主函数实现
│  ├─ time_thread.c ----- 计时线程函数实现
│  ├─ tju_packet.c ------ 可靠层包封装实现
│  └─ tju_tcp.c --------- 可靠层实现
├─ inc
│  ├─ global.h
│  ├─ kernel.h
│  ├─ log.h
│  ├─ operation.h
│  ├─ rtt.h
│  ├─ time_thread.h
│  ├─ tju_packet.h
│  └─ tju_tcp.h
├─ build
│  ├─ kernel.o
│  ├─ log.o
│  ├─ operation.o
│  ├─ rtt.o
│  ├─ time_thread.o
│  ├─ tju_packet.o
│  └─ tju_tcp.o
└─ bin
   ├─ client
   └─ server
```
