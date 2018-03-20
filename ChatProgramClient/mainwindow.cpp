#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectAll();
    //Receiver Thread
    //Print to text window function
    //Send Function
}

MainWindow::~MainWindow()
{
    //Send Quit Message
    delete ui;
}

void MainWindow::connectAll()
{
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::connectToServer);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::disconnectFromServer);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveToFile);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::sendToServer);
    connect(ui->sendEdit, &QLineEdit::returnPressed, this, &MainWindow::sendToServer);
}

void MainWindow::connectToServer()
{
    ui->sendEdit->setEnabled(true);
    ui->sendButton->setEnabled(true);
}

void MainWindow::disconnectFromServer()
{

}

void MainWindow::saveToFile()
{
    std::ofstream saveFile;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Chat Log"),
            "",
            tr("Text data (*.txt)"));

    saveFile.open(fileName.toStdString());

    saveFile << ui->messagesEdit->toPlainText().toStdString();

    saveFile.close();
}

void MainWindow::sendToServer()
{
    //Write to textedit
    QString temp = ui->sendEdit->text();
    ui->messagesEdit->append(QDateTime::currentDateTime().time().toString() + "\tMe: " + temp);

    //Send contents of lineedit


    //clear lineedit
    ui->sendEdit->setText("");
}
