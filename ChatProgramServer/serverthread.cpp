#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include <QtCore>
#include <QDebug>
#include "serverthread.h"
#include "extras.h"
#include "serverthread.h"
#define PORT 42069
#define MAXLINE 255

serverThread::serverThread()
{
    running = true;
}

void serverThread::run()
{
    int listenfd;
    int i;
    int sockfd;
    int maxfd;
    int arg;
    int maxi = -1;
    int confd;
    int port = PORT;
    int nready;
    int client[FD_SETSIZE];
    char buf[MAXLINE];
    ssize_t n;
    struct sockaddr_in cliaddr, servaddr;
    socklen_t clilen;
    fd_set rset, allset;


    // clear the buffer
    memset(&buf,0,sizeof(buf));

    // Create the listen socket
    if ((listenfd = socket(AF_INET,SOCK_STREAM, 0)) == -1)
    {
        qDebug() << "Failed to Create Listen Socket";
    }

    // Set so immediate reuse of port is possible
    arg = 1;
    if (setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&arg,sizeof(arg)) == -1)
    {
        qDebug() << "Failure setsockopt";
    }

    // Zero out memory of client structure
    memset(&servaddr,0,sizeof(servaddr));

    // Fill out the server structure
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // Bind to socket
    if (bind(listenfd,(sockaddr *) &servaddr,sizeof(servaddr)) == -1)
    {
        qDebug() << "Failed to Bind to Listen Socket";
    }

    // Make socket a listening socket
    if (listen(listenfd,SOMAXCONN) == -1)
    {
        qDebug() << "Failed to Listen";
    }

    maxfd = listenfd;
    maxi = -1;

    // set all member of client array to -1
    for (int i = 0;i < FD_SETSIZE;i++)
    {
        client[i] = -1;
    }

    // clear allset
    FD_ZERO(&allset);

    // add listen socket to allset
    FD_SET(listenfd, &allset);

    while (running)
    {
        qDebug() << "Listening For Connections";
        rset = allset;

        // Select blocks waiting for action
        if ((nready = select(maxfd + 1,&rset, NULL,NULL,NULL)) == -1)
        {
            qDebug() << "Select Failure";
        }

        if (FD_ISSET(listenfd,&rset))
        {
            clilen = sizeof(cliaddr);

            // Accept connection
            if ((confd = accept(listenfd, (sockaddr *) &cliaddr, &clilen)) == -1)
            {
                qDebug() << "Accept Failure";
            }

            // set the first empty element in client to client descriptor
            for (i = 0;i < FD_SETSIZE;i++)
            {
                if (client[i] < 0)
                {
                    client[i] = confd;
                    break;
                }
            }

            // test to make sure you do not have too many clients
            if (i == FD_SETSIZE)
            {
                qDebug() << "Failure too many clients";
            }

            // set the allset value to 1
            FD_SET(confd, &allset);

            //
            if (confd > maxfd)
            {
                maxfd = confd;
            }

            if (i > maxi)
            {
                maxi = i;
            }

            if (--nready <= 0)
            {
                continue;
            }
        }



        for (i = 0; i <= maxi; i++)
        {
            if ((sockfd = client[i]) < 0)
            {
                continue;
            }

            if (FD_ISSET(sockfd, &rset))
            {
                if ((n = read(sockfd, buf, MAXLINE)) == 0)
                {
                    Close(sockfd);
                    FD_CLR(sockfd,&allset);
                    client[i] = -1;
                }
                else
                {
                    write(sockfd,buf,n);
                }

                if (--nready <= 0)
                {
                    break;
                }
            }
        }
    }
}
