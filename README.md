# TrackingCamera
这是工控实验课上图像追踪识别摄像头的所有程序。

NewUpperComputer文件夹中的是之后做的
内容如下
之前那个需要一个界面程序搭配一个C++程序，这个一个界面程序就可以完成，更加简便
当时课上推荐VC来做串口，需要一堆麻烦的操作，但这个方法不但不需要那么多操作，而且可以自动搜索串口，不用管设备管理器
程序可以使用QT5直接打开,串口部分可以直接使用,opencv部分需要编译opencv源码
编译OpenCV的方法见https://blog.csdn.net/qq_32506245/article/details/80286535

OldUpperComputer文件夹中的是当时课上做的
内容如下
Camera.cpp文件里是opencv识别摄像头图像的程序,opencv版本2.4.8，编译器为VS2010。
InterFace文件夹下是C#界面程序。编译器为VS2010。
注意：C#程序与C++程序通讯需要在E盘下建一个test.txt文档。如果没有E盘，则需要更改这两个程序中关于文件读写的文件路径。

MicroProcessor文件夹下是单片机程序。使用STM32Cube+MDK5生成。STM32Cube版本4.20.1。

