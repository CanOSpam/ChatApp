#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    crypto.setKey(Q_UINT64_C(0x0e99d0161aa9070c));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addClient(std::string hostname)
{
    ui->listWidget->addItem(QString::fromStdString(hostname));
}
