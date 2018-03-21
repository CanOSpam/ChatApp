#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectAll();

    if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Socket failed to open");
        msgBox.setWindowTitle("Warning");
        msgBox.exec();
    }

    if (setsockopt (listen_sd, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg)) == -1)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Setsockopt failed");
        msgBox.setWindowTitle("Warning");
        msgBox.exec();
    }

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
    bool ok = false;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                             tr("IP to connect to:"), QLineEdit::Normal,
                                             "0.0.0.0", &ok);

    if(ok)
    {
        ui->sendEdit->setEnabled(true);
        ui->sendButton->setEnabled(true);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
    }
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
    writeToTextEdit(ui->sendEdit->text());

    //Send contents of lineedit


    //clear lineedit
    ui->sendEdit->setText("");
}

void MainWindow::writeToTextEdit(QString str)
{
     ui->messagesEdit->append(QDateTime::currentDateTime().time().toString() + "\tMe: " + str);
}
