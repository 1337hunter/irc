#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
   // delete chat;
}


void MainWindow::on_connect_clicked()
{
    chat = new ChatWindow(ui->ip->text(), ui->port->text(), ui->password->text(), ui->nickname->text(),
                          ui->username->text(),
                          ui->realname->text());
    this->hide();
    chat->show();
    chat->chatloop();
}
