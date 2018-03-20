#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdio.h>
#include <fstream>

#include <QMainWindow>
#include <QTime>
#include <QFileDialog>

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
    Ui::MainWindow *ui;
    void connectAll();

public slots:
    void connectToServer();
    void disconnectFromServer();
    void saveToFile();
    void sendToServer();
};

#endif // MAINWINDOW_H
