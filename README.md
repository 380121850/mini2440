# 基于mini2440学习板的嵌入式系统搭建
## 背景介绍：  
  FriendlyARM-2440整个资料光盘算是比较完整了。但是对于习惯整个SDK进行产品开发的开发者来说，2440的资料光盘略显零碎了。里面各个工具，开源组件的使用，包含了很多繁琐配置的，手动拷贝的参与；所以基于FriendlyARM-2440，想整合一个较完整的，类似一般SOC SDK的开发环境，其实就是个人开发环境的定制化；当拿到SOC的SDK后，基于某个确定的编译服务器，几步安装动作或者配置命令后，便可以进行整个嵌入式系统的开发、验证；项目放在GitHub上，方便后面使用；

## 一、环境：  
基于 FriendlyARM-2440-DVD-20151113 光盘
编译服务器环境 Linux ubuntu18 4.15.0-136-generic （友善是基于Fedora，但是另外一个SOC的SDK需要使用Ubuntu18，就一并放一起了）
开发板是FriendlyARM-2440，256M NAND FLASH，64MB的DDR内存，2MB的NOR FLASH，X35显示的配置
## 二、开发记录  
### 2021.03.09
基于UBOOT引导，内核和rootfs使用initram方式；使用uImage，根文件系统rootfs精简
busybox升级到最新版本。

### 2021.03.14
1、代码、编译Makefile、组件依赖、分区、文件系统映像的制作；
2、BSP（UBOOT、Linux 内核、根文件系统（包含busybox及启动需要的几个libc库等））
3、ubi文件系统的支持；除了BSP（UBOOT和OS分区），剩余空间划分为应用程序APP分区；
   后续各种应用、驱动可以直接在上面更新；就不用折腾UBOOT和OS了
   
分区划分：  
dev:    size   erasesize  name  
mtd0: 00080000 00020000 "uboot"  
mtd1: 00020000 00020000 "env"  
mtd2: 00700000 00020000 "os"  
mtd3: 0f860000 00020000 "app"  

### 2021.06.16
1、增加QT，但是基于UBUNTU18，没有编译通过
2、内核编译使用-O选项，并且initram通过配置文件方式，可以不需要用ROOT用户；
