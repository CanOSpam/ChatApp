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
    void addClient(std::string hostname);
    void removeClient(std::string hostname);
};

#endif // SERVERTHREAD_H
