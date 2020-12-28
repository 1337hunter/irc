#include "chatwindow.h"
#include "ui_chatwindow.h"
#include <string>
#include <iostream>
#include <QMessageBox>

typedef struct addrinfo     t_addrinfo;

ChatWindow::ChatWindow()
{

}

ChatWindow::~ChatWindow()
{
    delete ui;
}

ChatWindow::ChatWindow(QString ip, QString port, QString password, QString nickname, QString username, QString realname,
    QWidget *parent) : QMainWindow(parent), ip(ip), port(port), password(password), nickname(nickname), username(username),
    realname(realname),
    ui(new Ui::ChatWindow)
{
    ui->setupUi(this);

}

void delay()
{
    QTime dieTime= QTime::currentTime().addSecs(10);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void    ChatWindow::do_connect(bool tls)
{
    t_addrinfo  hints;
    t_addrinfo  *addr;
    QMessageBox     messageBox;

    hints.ai_flags = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    if ((sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        messageBox.critical(0, "Error", "Somethig went wrong.\nPlease restart you client!\n(socket)");
        messageBox.setFixedSize(200,100);
        return ;
    }
    /*if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
    {

        messageBox.critical(0, "Error", "Somethig went wrong.\nPlease restart you client!\n(fcntl)");
        messageBox.setFixedSize(200,100);
        return ;
    }*/
    if (::getaddrinfo(std::string(ip.toLocal8Bit().constData()).c_str(), std::string(port.toLocal8Bit().constData()).c_str()    , &hints, &addr))
    {
        messageBox.critical(0, "Error", "Somethig went wrong.\nPlease restart you client!\n(getaddrinfo)");
        messageBox.setFixedSize(200,100);
        return ;
    }
    int res = -1;
    while (res != 0)
    {
        ui->mainchat->append("Connecting to " + ip + ":" + port + " ...");
        res = ::connect(sock, addr->ai_addr, addr->ai_addrlen);
        if (res == 0)
            break ;
        else
            ui->mainchat->append("Connection faild. Next try in 10 seconds ...");
        delay();
    }
    ui->mainchat->append("Connected!");
    freeaddrinfo(addr);
}

void    ChatWindow::chatloop(void)
{

}
