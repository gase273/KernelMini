# KernelMini
Project for the course Operating Systems 1 (OS1), School of Electrical Engineering, Belgrade
Designed to work on a RISC-V 64-bit processor using a host XV6 system

Functionalities: Preemtive (not for kernel code), C and C++ user interfaces, Periodic thread support, FIFO scheduling with time sharing, First fit allocation for heap, with block slicing/merging

Make commands: all clean qemu qemu-gdb

Required Linux packages:
build-essential
qemu-system-misc
gcc-riscv64-linux-gnu
binutils-riscv64-linux-gnu
gdb-multiarch
g++-riscv64-linux-gnu
