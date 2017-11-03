# TrackingCamera
这是工控实验课上图像追踪识别摄像头的所有程序。
Camera.cpp文件里是opencv识别摄像头图像的程序,opencv版本2.4.8，编译器为VS2010。

InterFace文件夹下是C#界面程序。编译器为VS2010。

MicroProcessor文件夹下是单片机程序。使用STM32Cube+MDK5生成。STM32Cube版本4.20.1。

注意：C#程序与C++程序通讯需要在E盘下建一个test.txt文档。如果没有E盘，则需要更改这两个程序中关于文件读写的文件路径。