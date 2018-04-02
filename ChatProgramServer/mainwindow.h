#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "serverthread.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionStart_triggered();
    void on_actionStop_triggered();
    void addClient(std::string hostname);
    void removeClient(std::string hostname);

private:
    Ui::MainWindow *ui;
    serverThread server;
};

#endif // MAINWINDOW_H
