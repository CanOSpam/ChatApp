#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H


#include <QtCore>

class serverThread : public QThread
{
public:
    serverThread();
    void run();
};

#endif // SERVERTHREAD_H
