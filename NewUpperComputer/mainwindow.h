#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <mythread.h>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;

private slots:
    void on_openSerialPushButton_clicked();

    void ReadData();

    void on_trackStartPushButton_clicked();

    void on_trackStopPushButton_clicked();

    void on_triangleRadioButton_clicked();

    void on_squareRadioButton_clicked();

    void send();
    void on_flashSerialPushButton_clicked();

private:
    QSerialPort *serial;
    mythread *imageThread;
};

extern MainWindow w;
#endif // MAINWINDOW_H
