#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <string>
#include <iostream>
#include <cctype>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool    is_valid_ip(std::string ip)
{
    if (ip.empty())
        return false;
    return true;
}

bool    is_valid_port(std::string port)
{
    if (port.empty())
        return false;
    if (port.find_first_not_of("0123456789") != std::string::npos)
        return false;
    if (port.size() < 1 || port.size() > 5)
        return false;
    if (stoi(port) < 1 || stoi(port) > 65535)
        return false;
    return true;
}

void MainWindow::on_connect_clicked()
{
    QString _ip = ui->ip->text();
    QString _port = ui->port->text();
    QString _pass = ui->password->text();
    QString _nick = ui->nickname->text();
    QString _user = ui->username->text();
    QString _real = ui->realname->text();
    QMessageBox     messageBox;

    if (!is_valid_ip(_ip.toLocal8Bit().constData()))
    {
        messageBox.critical(0, "Error", "ip address is invalid");
        messageBox.setFixedSize(200,100);
        return ;
    }
    if (!is_valid_port(_port.toLocal8Bit().constData()))
    {
        messageBox.critical(0, "Error", "port is invalid");
        messageBox.setFixedSize(200, 100);
        return ;
    }
    if (_nick.isEmpty())
    {
        messageBox.critical(0, "Error", "nickname is invalid");
        messageBox.setFixedSize(200, 100);
        return ;
    }
    if (_user.isEmpty())
    {
        messageBox.critical(0, "Error", "username is invalid");
        messageBox.setFixedSize(200, 100);
        return ;
    }
    if (_real.isEmpty())
    {
        messageBox.critical(0, "Error", "realname is invalid");
        messageBox.setFixedSize(200, 100);
        return ;
    }
    chat = new ChatWindow(_ip, _port, _pass, _nick, _user, _real, ui->tls->isChecked(), false);
    this->hide();
    chat->show();
    chat->do_connect();
}

void    MainWindow::keyPressEvent(QKeyEvent *event)
{
    QKeyEvent* key = static_cast<QKeyEvent*>(event);

    if (key->key() == Qt::Key_Enter || key->key()==Qt::Key_Return)
        on_connect_clicked();
}
