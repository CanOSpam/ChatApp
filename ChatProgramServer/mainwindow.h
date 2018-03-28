#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


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
    void removeClient(std::string hostname);
};

#endif // MAINWINDOW_H
