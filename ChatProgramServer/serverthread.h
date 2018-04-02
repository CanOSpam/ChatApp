#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H


#include <QtCore>

class serverThread : public QThread
{
    Q_OBJECT

public:
    serverThread();
    void run();
    bool running;

signals:
    void addClient(QString hostname);
    void removeClient(QString hostname);
};

#endif // SERVERTHREAD_H
