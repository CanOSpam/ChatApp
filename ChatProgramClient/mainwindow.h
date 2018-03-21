#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdio.h>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>

#include <QMainWindow>
#include <QTime>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "SimpleCrypt.h"

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
    SimpleCrypt crypto;

    Ui::MainWindow *ui;
    void connectAll();
    void writeToTextEdit(QString str);

public slots:
    void connectToServer();
    void disconnectFromServer();
    void saveToFile();
    void sendToServer();
};

#endif // MAINWINDOW_H
