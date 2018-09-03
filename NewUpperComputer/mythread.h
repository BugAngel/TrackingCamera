#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QThread>

class mythread: public QThread
{
public:
    int shape=3;
    unsigned char x;
    unsigned char y;
    mythread();
    void run();
};

#endif // MYTHREAD_H
