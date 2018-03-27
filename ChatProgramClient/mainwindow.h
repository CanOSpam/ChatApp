#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdio.h>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <thread>
#include <netinet/in.h>
#include <netdb.h>

#include <QDebug>

#include <QMainWindow>
#include <QTime>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "SimpleCrypt.h"

#define PORT 42069
#define BUFLEN 255

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    int listen_sd;
    int arg;
    bool running;
    SimpleCrypt crypto;
    std::thread thread;
    struct sockaddr_in server;
    struct hostent	*hp;

    Ui::MainWindow *ui;
    void connectAll();
    void writeToTextEdit(QString str);
    void raiseWarning(QString title, QString message);


public slots:
    void connectToServer();
    void disconnectFromServer();
    void saveToFile();
    void sendToServer();
};

#endif // MAINWINDOW_H
