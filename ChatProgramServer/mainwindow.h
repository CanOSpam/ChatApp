#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private slots:
    void on_actionStart_triggered();

    void on_actionStop_triggered();

private:
    Ui::MainWindow *ui;
    void addClient(std::string hostname);
    SimpleCrypt crypto;
};

#endif // MAINWINDOW_H
