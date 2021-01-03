#include "chatwindow.h"
#include "ui_chatwindow.h"
#include <iostream>
#include <QMessageBox>
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
    while (true)
    {
        int res = ::connect(sock, addr->ai_addr, addr->ai_addrlen);
         ui->mainchat->append("Connecting ...");
        if (res == 0)
        {
            if (!tls)
            {
                connected = true;
                ui->mainchat->append("Connected!");
            }
            break ;
        }
        else
        {
            int i = 1000;
            while (--i > 0)
            {
                QApplication::processEvents();
                usleep(10000);
            }
        }
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
        ui->mainchat->append("Connected!\n");
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
    ssize_t         r = 0;

    if (tls && sslptr)
        r = SSL_write(sslptr, wrbuf.c_str(), wrbuf.length());
    else
        r = write(sock, wrbuf.c_str(), wrbuf.size());
    if (r <= 0)
        messageBox.critical(0, "Error", "SendMessage.\nPlease restart you client!");
    wrbuf.erase();
}

std::vector<std::string>   ChatWindow::splitstring(std::string str, char delim)
{
    std::vector<std::string>   split;
    size_t      pos(0);
    std::string temp;

    while ((pos = str.find(delim)) != std::string::npos)
    {
        temp = str.substr(0, pos);
        if (!temp.empty())
            split.push_back(temp);
        str.erase(0, pos + 1);
    }
    if (!str.empty())
        split.push_back(str);
    return split;
}

std::vector<std::string>   ChatWindow::splitstringbyany(std::string msg, std::string const &delim)
{
    std::vector<std::string>       split;
    size_t          pos = 0;
    std::string     tmp;

    while ((pos = msg.find_first_of(delim)) != std::string::npos)
    {
        tmp = msg.substr(0, pos);
        if (!tmp.empty())
            split.push_back(tmp);
        msg.erase(0, pos + 1);
    }
    if (!msg.empty())
        split.push_back(msg);
    return (split);
}

void    ChatWindow::ReceiveMessage(void)
{
    QMessageBox     messageBox;
    ssize_t     r = 0;
    char        buf_read[1024 + 1];
    std::vector<std::string>    split;

    if (tls && sslptr)
        r = SSL_read(sslptr, buf_read, 1024);
    else
        r = recv(sock, buf_read, 1024, 0);
    if (r >= 0)
        buf_read[r] = 0;
    if (r > 0)
    {
        split = splitstringbyany(std::string(buf_read), "\r\n");
        for (size_t i = 0; i < split.size(); ++i)
            ui->mainchat->append(split[i].c_str());
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
        timeout = {0, 50000};
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

void    ChatWindow::keyPressEvent(QKeyEvent *event)
{
    QKeyEvent* key = static_cast<QKeyEvent*>(event);

    if (key->key() == Qt::Key_Enter || key->key() == Qt::Key_Return)
    {
        wrbuf += std::string(ui->lineEdit->text().toLocal8Bit().constData());
        if (!wrbuf.empty())
        {
            ui->mainchat->append(ui->lineEdit->text());
            wrbuf += "\r\n";
        }
        ui->lineEdit->clear();
    }
}

void    ChatWindow::closeEvent(QCloseEvent* event)
{
    exit(0);
}
