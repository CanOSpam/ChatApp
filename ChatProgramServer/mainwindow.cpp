#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serverthread.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    addClient("Test1");
    addClient("Test2");
    addClient("Test3");

}

serverThread server;

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addClient(std::string hostname)
{
    ui->listWidget->addItem(QString::fromStdString(hostname));
}


void MainWindow::on_actionStart_triggered()
{
    ui->actionStart->setEnabled(false);
    ui->actionStop->setEnabled(true);
    server.start();
}

void MainWindow::on_actionStop_triggered()
{
    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(false);
    //stop the thread somehow
}
