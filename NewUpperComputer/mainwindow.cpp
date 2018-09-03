#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSerialPort serial; //不重新定义变量就会报错
    //查找可用的串口
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->portComboBox->addItem(serial.portName());
            serial.close();
        }
    }
    //设置波特率下拉菜单默认显示第0项
    ui->baudComboBox->setCurrentIndex(0);   

    imageThread=new mythread;

    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(send()));//注意第一个为timer
    timer->start(10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openSerialPushButton_clicked()
{
    if(ui->openSerialPushButton->text() == tr("打开串口"))
    {
        serial = new QSerialPort;
        //设置串口名
        serial->setPortName(ui->portComboBox->currentText());
        //打开串口
        serial->open(QIODevice::ReadWrite);

        //设置波特率
        switch (ui->bitComboBox->currentIndex())
        {
        case 0:
            serial->setBaudRate(QSerialPort::Baud9600);//设置波特率
            break;
        case 1:
            serial->setBaudRate(QSerialPort::Baud115200);//设置波特率
            break;
        default:
            break;
        }


        //设置数据位数
        switch (ui->bitComboBox->currentIndex())
        {
        case 0:
            serial->setDataBits(QSerialPort::Data8);//设置数据位8
            break;
        case 1:
            serial->setDataBits(QSerialPort::Data7);//设置数据位7
            break;
        case 2:
            serial->setDataBits(QSerialPort::Data6);//设置数据位6
            break;
        case 3:
            serial->setDataBits(QSerialPort::Data5);//设置数据位5
            break;
        default:
            break;
        }

        //设置校验位
        switch (ui->parityComboBox->currentIndex())
        {
        case 0:
            serial->setParity(QSerialPort::NoParity);
            break;
        case 1:
            serial->setParity(QSerialPort::OddParity);
            break;
        case 2:
            serial->setParity(QSerialPort::EvenParity);
            break;
        default:
            break;
        }

        //设置停止位
        switch (ui->stopComboBox->currentIndex())
        {
        case 0:
            serial->setStopBits(QSerialPort::OneStop);//停止位设置为1
            break;
        case 1:
            serial->setStopBits(QSerialPort::OneAndHalfStop);//停止位设置为1.5
            break;
        case 2:
            serial->setStopBits(QSerialPort::TwoStop);//停止位设置为2
        default:
            break;
        }

        //设置流控制
        serial->setFlowControl(QSerialPort::NoFlowControl);//设置为无流控制

        //关闭设置菜单使能
        ui->portComboBox->setEnabled(false);
        ui->baudComboBox->setEnabled(false);
        ui->bitComboBox->setEnabled(false);
        ui->parityComboBox->setEnabled(false);
        ui->stopComboBox->setEnabled(false);
        ui->flashSerialPushButton->setEnabled(false);
        ui->openSerialPushButton->setText(tr("关闭串口"));

        //连接信号槽
        QObject::connect(serial,&QSerialPort::readyRead,this,&MainWindow::ReadData);
    }
    else
    {
        //关闭串口
        serial->clear();
        serial->close();

        //恢复设置使能
        ui->portComboBox->setEnabled(true);
        ui->baudComboBox->setEnabled(true);
        ui->bitComboBox->setEnabled(true);
        ui->parityComboBox->setEnabled(true);
        ui->stopComboBox->setEnabled(true);
        ui->flashSerialPushButton->setEnabled(true);
        ui->openSerialPushButton->setText(tr("打开串口"));

    }

}

//读取接收到的信息
void MainWindow::ReadData()
{
    QByteArray buf;
    buf = serial->readAll();//buf就是接收到的信息
    if(!buf.isEmpty())
    {
        //这里对接收到的信息处理
    }
    buf.clear();
}



void MainWindow::on_trackStartPushButton_clicked()
{
    imageThread->start();
}

void MainWindow::on_trackStopPushButton_clicked()
{
    imageThread->quit();
}

void MainWindow::on_triangleRadioButton_clicked()
{
    imageThread->shape=3;
}

void MainWindow::on_squareRadioButton_clicked()
{
    imageThread->shape=4;
}

void MainWindow::send()
{
    if(serial->isOpen())
    {
        //发送数据
        QByteArray Send_Data;
        Send_Data.resize(5);
        Send_Data[0]=0xff;
        Send_Data[1]=imageThread->x;
        Send_Data[2]=imageThread->y;
        Send_Data[3]=0x0d;
        Send_Data[4]=0x0a;
        serial->write(Send_Data);
    }
}

void MainWindow::on_flashSerialPushButton_clicked()
{
    QSerialPort serial;
    //查找可用的串口
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->portComboBox->addItem(serial.portName());
            serial.close();
        }
    }
}
