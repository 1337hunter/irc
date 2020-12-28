#include "chatwindow.h"
#include "ui_chatwindow.h"
#include <string>

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
    chatloop();
    ui->mainchat->append("Connecting to " + ip + ":" + port);
}

void    ChatWindow::chatloop(void)
{

}
