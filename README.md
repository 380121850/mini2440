# mini2440
背景：
FriendlyARM-2440整个资料光盘算是比较完整了。但是对于习惯整个SDK进行产品开发的开发者来说，2440的资料光盘略零碎了。里面各个工具，开源组件的使用，包含了很多繁琐配置的，手动拷贝的参与；所以基于FriendlyARM-2440，想整合一个较完整的，类似一般SOC SDK的开发环境，其实就是个人开发环境的定制化；并且放在GitHub上，方便后面使用；

一、环境：
基于 FriendlyARM-2440-DVD-20151113 光盘
编译服务器环境 Linux ubuntu18 4.15.0-136-generic （友善是基于Fedora，但是本来另外一个需要使用Ubuntu18，就一并放一起了）
开发板是256M NAND FLASH，X35显示的配置

2021.03.09
基于UBOOT引导，内核和rootfs使用initram方式；使用uImage，根文件系统rootfs精简
busybox升级到最新版本。

