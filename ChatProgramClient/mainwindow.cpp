/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: mainwindow.cpp - A client paired with a server that allows text chat.
--
-- PROGRAM: Chat client server
--
-- FUNCTIONS:
--    void connectAll( void )
--    void raiseWarning(QString title, QString message)
--    void writeToTextEdit(QString str)
--
-- SLOTS:
--    void connectToServer( void )
--    void disconnectFromServer( void )
--    void saveToFile( void )
--    void sendToServer( void )
--    void writeReceivedToTextEdit(QString str)
--
-- SIGNALS:
--    void gotNewText(QString str)
--
--
-- DATE: April 3, 2018
--
-- REVISIONS: None to date
--
-- DESIGNER: Tim Bruecker
--
-- PROGRAMMER: Tim Bruecker
--
-- NOTES:
-- The program starts a client that can then connect to a server by specifying an ip address and user name.
-- The communications are obfuscated end to end by using the qt simplecrypt library, which changes the encoding of the text
-- using a seed key. This method of 'encryption' isn't very strong, but does prevent the server, or anyone sniffing packets
-- from being to read plain text.
----------------------------------------------------------------------------------------------------------------------*/
#include "mainwindow.h"
#include "ui_mainwindow.h"

// Constructor initialises the crypto key.
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectAll();
    crypto.setKey(Q_UINT64_C(0x0e99d0161aa9070c));
}

// Destructor disconnects from server and quits.
MainWindow::~MainWindow()
{
    disconnectFromServer();

    delete ui;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: connectAll
--
-- DATE: April 3, 2018
--
-- REVISIONS: None to date
--
-- DESIGNER: Tim Bruecker
--
-- PROGRAMMER: Tim Bruecker
--
-- INTERFACE: void connectAll (void)
--
-- RETURNS: void.
--
-- NOTES:
-- This function must be called in the constructor, it connect all the required signals and slots in the client.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::connectAll()
{
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::connectToServer);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::disconnectFromServer);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveToFile);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::sendToServer);
    connect(ui->sendEdit, &QLineEdit::returnPressed, this, &MainWindow::sendToServer);
    connect(this, &MainWindow::gotNewText, this, &MainWindow::writeReceivedToTextEdit);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: connectToServer
--
-- DATE: April 3, 2018
--
-- REVISIONS: None to date
--
-- DESIGNER: Tim Bruecker
--
-- PROGRAMMER: Tim Bruecker
--
-- INTERFACE: void connectToServer (void)
--
-- RETURNS: void.
--
-- NOTES:
-- This function creates a socket, then gets user input for the ip address and user name to connect to. After succesfully
-- reading input, it opens the socket and connects to the server. If this is succesful, it starts a thread that reads
-- messages from the server and emits a signal every time it catches new data.
----------------------------------------------------------------------------------------------------------------------*/
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

    QString ipAddr = QInputDialog::getText(this, tr("IP Address"),
                                             tr("IP to connect to:"), QLineEdit::Normal,
                                             "0.0.0.0", &ok);

    username = QInputDialog::getText(this, tr("Username"),
                                             tr("Username:"), QLineEdit::Normal,
                                             "", &ok);

    if(ok)
    {
        ui->sendEdit->setEnabled(true);
        ui->sendButton->setEnabled(true);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->sendEdit->setMaxLength(255 - username.length() - 2);
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: disconnectFromServer
--
-- DATE: April 3, 2018
--
-- REVISIONS: None to date
--
-- DESIGNER: Tim Bruecker
--
-- PROGRAMMER: Tim Bruecker
--
-- INTERFACE: void disconnectFromServer (void)
--
-- RETURNS: void.
--
-- NOTES:
-- This function closes the socket currently in use and stops the receiver thread. It also resets UI elements so
-- the program is ready to connect to a server again.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::disconnectFromServer()
{
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: saveToFile
--
-- DATE: April 3, 2018
--
-- REVISIONS: None to date
--
-- DESIGNER: Tim Bruecker
--
-- PROGRAMMER: Tim Bruecker
--
-- INTERFACE: void saveToFile (void)
--
-- RETURNS: void.
--
-- NOTES:
-- This function gets user input for a filename and location to save to, then writes the contents of the chat window
-- to that file.
----------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: sendToServer
--
-- DATE: April 3, 2018
--
-- REVISIONS: None to date
--
-- DESIGNER: Tim Bruecker
--
-- PROGRAMMER: Tim Bruecker
--
-- INTERFACE: void sendToServer (void)
--
-- RETURNS: void.
--
-- NOTES:
-- This function sends a chat message to the server. it sends the username and chat message, seperated by a colon.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::sendToServer()
{
    // Write to textedit
    QString temp = username;
    temp.append(": ");
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: writeToTextEdit
--
-- DATE: April 3, 2018
--
-- REVISIONS: None to date
--
-- DESIGNER: Tim Bruecker
--
-- PROGRAMMER: Tim Bruecker
--
-- INTERFACE: void writeToTextEdit (QString str)
--             QString str: The string to write to the chat window. Contains a username and message.
--
-- RETURNS: void.
--
-- NOTES:
-- This function takes in a QString, decrypts it using crypto and appends it to the chat window with a timestamp.
-- This function is the same as the one below, but it is a standalone function instead of a slot.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::writeToTextEdit(QString str)
{
    str = crypto.decryptToString(str);
    ui->messagesEdit->append(QDateTime::currentDateTime().time().toString() + " ~ " + str);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: writeToTextEdit
--
-- DATE: April 3, 2018
--
-- REVISIONS: None to date
--
-- DESIGNER: Tim Bruecker
--
-- PROGRAMMER: Tim Bruecker
--
-- INTERFACE: void writeToTextEdit (QString str)
--             QString str: The string to write to the chat window. Contains a username and message.
--
-- RETURNS: void.
--
-- NOTES:
-- This function takes in a QString, decrypts it using crypto and appends it to the chat window with a timestamp.
-- This function is the same as the one above, but it is a slot instead of a stand alone function.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::writeReceivedToTextEdit(QString str)
{
    str = crypto.decryptToString(str);
    ui->messagesEdit->append(QDateTime::currentDateTime().time().toString() + " ~ " + str);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: raiseWarning
--
-- DATE: April 3, 2018
--
-- REVISIONS: None to date
--
-- DESIGNER: Tim Bruecker
--
-- PROGRAMMER: Tim Bruecker
--
-- INTERFACE: void raiseWarning (QString title, QString message)
--             QString title: The title of the warning window.
--             QString message: The message in the warning window.
--
-- RETURNS: void.
--
-- NOTES:
-- This function is a wrapper that creates a warning popup displayed to the user.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::raiseWarning(QString title, QString message)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(message);
    msgBox.setWindowTitle(title);
    msgBox.exec();
}
