#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&server, &serverThread::addClient, this, &MainWindow::addClient);
    connect(&server, &serverThread::removeClient, this, &MainWindow::removeClient);
}



MainWindow::~MainWindow()
{
    server.running = false;
    // Need to skip blocking calls, once that is figured out this will work
    server.wait();
    delete ui;
}

void MainWindow::addClient(QString hostname)
{
    ui->listWidget->addItem(hostname);
}

void MainWindow::removeClient(QString hostname)
{
    QList<QListWidgetItem *> items = ui->listWidget->findItems(hostname, 0);

    foreach(QListWidgetItem * item, items)
    {
        delete ui->listWidget->takeItem(ui->listWidget->row(item));
    }
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
    server.running = false;
    // Need to skip blocking calls, once that is figured out this will work
    server.wait();
    ui->listWidget->clear();
}
