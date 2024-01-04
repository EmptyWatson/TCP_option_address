# TCP Option Address

The TCP Option Address (TOA) module is a kernel module that obtains the source IPv4 address from the option section of a TCP header.

此分支用于适配kylin v10 aarch64系统。

创建次分支的原因是官方版本在kylin v10 aarch64上编译失败(centos 7.x aarch64上同样编译失败)。

对应的bug
[TOA arm64 编译报错 #9](https://github.com/Huawei/TCP_option_address/issues/9)

更换了去除写保护的方法。

## Tested os version
### 4.19.90-24.4.v2101.ky10.aarch64 
Kylin-Server-10-SP2-Release-Build09-20210524-arm64

## How to use the TOA module?

### Requirement

The development environment for compiling the kernel module consists of: 
- kernel headers and the module library
- the gcc compiler
- GNU Make tool

### Installation

Download and decompress the source package:
```
https://github.com/Huawei/TCP_option_address/archive/master.zip
```

Enter the source code directory and compile the module:
```
cd src
make
```

Load the kernel module:
```
sudo insmod toa.ko
```

## Distribution license

TOA is distributed under the terms of the GNU General Public License v2.0. The full terms and conditions of this license are detailed in the LICENSE file.