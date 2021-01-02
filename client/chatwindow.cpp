#include "chatwindow.h"
#include "ui_chatwindow.h"
#include <string>
#include <iostream>
#include <QMessageBox>
#include <thread>
#include <ctime>

typedef struct addrinfo     t_addrinfo;

ChatWindow::ChatWindow()
{

}

ChatWindow::~ChatWindow()
{
    delete ui;
}

ChatWindow::ChatWindow(QString ip, QString port, QString password, QString nickname, QString username, QString realname,
    bool tls, bool connected, QWidget *parent) : QMainWindow(parent), ip(ip), port(port), password(password), nickname(nickname), username(username),
    realname(realname), tls(tls), connected(connected), ui(new Ui::ChatWindow)
{
    ui->setupUi(this);

}

/*void    thread_connect(ChatWindow *obj, bool tls)
{

}*/


void    ChatWindow::do_connect()
{
    //std::thread t1(thread_connect, this, tls);
    //ui->mainchat->append("Connecting every 10 seconds ...");
    //t1.join();
    t_addrinfo  hints;
    t_addrinfo  *addr;
    QMessageBox     messageBox;

    hints.ai_flags = 0;
    messageBox.setFixedSize(200,100);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    if ((sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        messageBox.critical(0, "Error", "Somethig went wrong.\nPlease restart you client!\n(socket)");
        return ;
    }

    if (::getaddrinfo(std::string(ip.toLocal8Bit().constData()).c_str(), std::string(port.toLocal8Bit().constData()).c_str(), &hints, &addr))
    {
        messageBox.critical(0, "Error", "Somethig went wrong.\nPlease restart you client!\n(getaddrinfo)");
        return ;
    }
    int res = ::connect(sock, addr->ai_addr, addr->ai_addrlen);
    if (res == 0)
    {
        if (!tls)
        {
            connected = true;
            ui->mainchat->append("Connected!");
        }
    }
    else
    {
        ui->mainchat->append("Connection failed!");
        return ;
    }
    if (tls)
    {
        SSL_library_init();
        SSL_load_error_strings();
        if (!(sslctx = SSL_CTX_new(TLS_method())))
        {
            messageBox.critical(0, "Error", "SSL error.\nPlease restart you client!\n(getaddrinfo)");
            return ;
        }
        if (SSL_CTX_set_ecdh_auto(ctx, 1) <= 0)
        {
            messageBox.critical(0, "Error", "SSL error.\nPlease restart you client!\n(getaddrinfo)");
            return ;
        }
        if ((sslptr = SSL_new(sslctx)) == 0)
        {
            messageBox.critical(0, "Error", "SSL error.\nPlease restart you client!\n(getaddrinfo)");
            return ;
        }
        if (!(SSL_set_fd(sslptr, sock)))
        {
            messageBox.critical(0, "Error", "SSL error.\nPlease restart you client!\n(getaddrinfo)");
            return ;
        }

        int     handshake = 0;
        handshake = SSL_connect(sslptr);

        if (handshake != 1)
        {
            int err = SSL_get_error(sslptr, handshake);
            if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE)
                exit(0);
        }
        connected = true;
        ui->mainchat->append("Connected!");
    }
    if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
    {
        messageBox.critical(0, "Error", "Somethig went wrong.\nPlease restart you client!\n(fcntl)");
        return ;
    }
    wrbuf += "NICK " + std::string(nickname.toLocal8Bit().constData()) + "\r\n";
    wrbuf += "USER " + std::string(username.toLocal8Bit().constData()) + " 0 * :" + std::string(realname.toLocal8Bit().constData()) + "\r\n";
    freeaddrinfo(addr);
    this->run();
}

void    ChatWindow::SendMessage(void)
{
    QMessageBox     messageBox;
    ssize_t     r = 0;

    wrbuf += "\r\n";
    if (tls && sslptr)
        r = SSL_write(sslptr, wrbuf.c_str(), wrbuf.length());
    else
    {
        std::cout  << wrbuf << " " << wrbuf.size() <<  '\n';
        r = write(sock, wrbuf.c_str(), wrbuf.size());
    }
    if (r <= 0)
        messageBox.critical(0, "Error", "SendMessage.\nPlease restart you client!");
    wrbuf.erase();
}

void    ChatWindow::ReceiveMessage(void)
{
    QMessageBox     messageBox;
    ssize_t     r = 0;
    char        buf_read[1024 + 1];

    if (tls && sslptr)
        r = SSL_read(sslptr, buf_read, 1024);
    else
        r = recv(sock, buf_read, 1024, 0);
    if (r >= 0)
        buf_read[r] = 0;
    if (r > 0)
    {
        ui->mainchat->append(buf_read);
    }
    else if (r < 0)
        messageBox.critical(0, "Error", "ReceiveMessage.\nPlease restart you client!");
}

void    ChatWindow::run(void)
{
    int readyfds;
    timeval timeout;

    while (1)
    {
        timeout = {0, 500};
        FD_ZERO(&fdset_read);
        FD_ZERO(&fdset_write);
        FD_SET(sock, &fdset_read);
        if (wrbuf.size() > 0)
            FD_SET(sock, &fdset_write);
        readyfds = select(sock + 1, &fdset_read, &fdset_write, 0, &timeout);
        if (readyfds < 0)
        {
            exit(0);
            return ;
        }
        bool    isread = FD_ISSET(sock, &fdset_read);
        bool    iswrite = FD_ISSET(sock, &fdset_write);
        if (isread)
            ReceiveMessage();
        if (iswrite)
            SendMessage();
        QApplication::processEvents();
    }
}

void ChatWindow::on_send_clicked()
{
    wrbuf = std::string(ui->lineEdit->text().toLocal8Bit().constData());
    if (!wrbuf.empty())
        ui->mainchat->append(ui->lineEdit->text());
    ui->lineEdit->clear();
}
