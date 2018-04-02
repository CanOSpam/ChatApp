#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectAll();
    crypto.setKey(Q_UINT64_C(0x0e99d0161aa9070c));
}

MainWindow::~MainWindow()
{
    //Send Quit Message
    disconnectFromServer();

    delete ui;
}

// Connect buttons and actions
void MainWindow::connectAll()
{
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::connectToServer);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::disconnectFromServer);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveToFile);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::sendToServer);
    connect(ui->sendEdit, &QLineEdit::returnPressed, this, &MainWindow::sendToServer);
    connect(this, &MainWindow::gotNewText, this, &MainWindow::writeReceivedToTextEdit);
}

// Connect to server
void MainWindow::connectToServer()
{
    memset((char *)&server, '\0', sizeof(struct sockaddr_in));

    // Create Socket
    if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        raiseWarning("Warning", "Socket failed to open.");
    }

    // Set Reusable
    if (setsockopt (listen_sd, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg)) == -1)
    {
        raiseWarning("Warning", "Setsockopt failed.");
    }

    bool ok = false;

    ui->messagesEdit->setText("");

    QString ipAddr = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                             tr("IP to connect to:"), QLineEdit::Normal,
                                             "0.0.0.0", &ok);

    username = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                             tr("Username:"), QLineEdit::Normal,
                                             "", &ok);

    if(ok)
    {
        ui->sendEdit->setEnabled(true);
        ui->sendButton->setEnabled(true);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->sendEdit->setMaxLength(255 - username.length() - 1);
    }
    else
    {
        raiseWarning("Warning", "Input error.");
        return;
    }

    // Connect to the server
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    hp = gethostbyname(ipAddr.toStdString().c_str());
    if (hp == NULL)
    {
        disconnectFromServer();
        raiseWarning("Warning", "Unknown server address.");
    }
    else
    {
        // Connecting to the server
        bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);
        if (::connect (listen_sd, (struct sockaddr *)&server, sizeof(server)) == -1)
        {
            disconnectFromServer();
            raiseWarning("Warning", "Cannot connect to server.");
        }
        else
        {
            // Start listener thread
            running = true;
            thread = std::thread([&]{
                while(running)
                {
                    char tempRecvBuf[BUFLEN];
                    memset(tempRecvBuf, '\0', BUFLEN);

                    // Receive and Write to textedit using function
                    if(recv(listen_sd, tempRecvBuf, BUFLEN, 0))
                    {
                        emit gotNewText(QString(tempRecvBuf));
                    }
                }

            });
        }
    }

}

// Disconnect from server and cleanup
void MainWindow::disconnectFromServer()
{
    // TODO:
    // Close connection to server
    shutdown(listen_sd, SHUT_RD);
    ::close(listen_sd);

    // Stop listener thread
    running = false;

    if(thread.joinable())
    {
        thread.join();
    }

    // Change enabled functions
    ui->sendEdit->setEnabled(false);
    ui->sendButton->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
}

// Save chatlog to file
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

// Send chat message to server
void MainWindow::sendToServer()
{
    // Write to textedit
    QString temp = username;
    temp.append("\t");
    temp.append(ui->sendEdit->text());
    temp = crypto.encryptToString(temp);

    // Write to textedit
    writeToTextEdit(temp);

    // Create c str buffer
    char tempSend[BUFLEN];
    memset(tempSend, '\0', BUFLEN);
    strcpy(tempSend, temp.toStdString().c_str());

    // Send
    send (listen_sd, tempSend, temp.length(), 0);

    //clear lineedit
    ui->sendEdit->setText("");
}

// Write a QString to chat messages window
void MainWindow::writeToTextEdit(QString str)
{
    str = crypto.decryptToString(str);
    ui->messagesEdit->append(QDateTime::currentDateTime().time().toString() + "\tMe:\t" + str);
}

void MainWindow::writeReceivedToTextEdit(QString str)
{
    str = crypto.decryptToString(str);
    ui->messagesEdit->append(QDateTime::currentDateTime().time().toString() + "\t" + str);
}

void MainWindow::raiseWarning(QString title, QString message)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(message);
    msgBox.setWindowTitle(title);
    msgBox.exec();
}
