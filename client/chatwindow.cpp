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
    ui->mainchat->append("Connecting to " + ip + ":" + port);
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
        messageBox.critical(0, "Error", "Somethig went wrong.\nPlease restart you client!(socket)");
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
    int res = ::connect(sock, addr->ai_addr, addr->ai_addrlen);
    freeaddrinfo(addr);
    if (res == -1 && errno == EINPROGRESS)
    {
        messageBox.critical(0, "Error", "Somethig went wrong.\nPlease restart you client!\n(EINPROGRESS)");
        messageBox.setFixedSize(200,100);
        return ;
    }
    else if (res != -1 && res < 0)
    {
        std::cout << res << " - res\n\n";
        messageBox.critical(0, "Error", "Somethig went wrong.\nPlease restart you client!\n(connect)");
        messageBox.setFixedSize(200,100);
        return ;
    }
}

void    ChatWindow::chatloop(void)
{

}
