#include "chatwindow.h"
#include "ui_chatwindow.h"
#include <iostream>
#include <QMessageBox>
#include <ctime>
#include "sendfile.h"
#include <QLabel>
#include <unistd.h>
#include <errno.h>

typedef struct addrinfo     t_addrinfo;

ChatWindow::ChatWindow()
{

}

ChatWindow::~ChatWindow()
{
    delete ui;
}

ChatWindow::ChatWindow(QString ip, QString port, QString password, QString nickname, QString username, QString realname,
    bool tls, bool connected, size_t file_size, QWidget *parent) : QMainWindow(parent), ip(ip), port(port), password(password), nickname(nickname), username(username),
    realname(realname), tls(tls), connected(connected), file_size(file_size), ui(new Ui::ChatWindow)
{
    ui->setupUi(this);
}

void    ChatWindow::do_connect()
{
    t_addrinfo      hints;
    t_addrinfo      *addr;
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

void    ChatWindow::SendMessage(void)
{
    QMessageBox     messageBox;
    ssize_t         r = 0;

    if (tls && sslptr)
        r = SSL_write(sslptr, wrbuf.c_str(), wrbuf.length());
    else
        r = write(sock, wrbuf.c_str(), wrbuf.size());
    if (r <= 0 && file_buf.empty())
        messageBox.critical(0, "Error", "SendMessage.\nPlease restart you client!");
    if (file_buf.size() != 0)
    {
        if (tls && sslptr)
            r = SSL_write(sslptr, file_buf.data(), file_buf.size());
        else
            r = write(sock, file_buf.data(), file_buf.size());
        if (r <= 0)
            messageBox.critical(0, "Error", "SendMessage.\nPlease restart you client!");
        file_buf.clear();
    }
    wrbuf.erase();
}

int ChatWindow::receive_file(unsigned char *buf, size_t r)
{
    size_t  pos = 0;
    QMessageBox         messageBox;

    std::vector<std::string>   split = splitstring(std::string((char*)buf), ' ');
    if (split.size() < 6)
        return r;
    file_name = split[1];
    file_from = split[3];
    file_size = stoi(split[4]);

    while (pos < r && buf[pos] != ':')
        pos++;
    if (pos == r)
        return r;
    pos++;

    file_bytes_received = 0;
    return append_to_file_buf(pos, buf, r);
    /*
    while (pos < r && file_bytes_received < file_size)
    {
        receive_file_buf.push_back(buf[pos]);
        file_bytes_received++;
        pos++;
    }
    if (file_bytes_received == file_size)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "File transfer", std::string(std::string("Receive file from ") + file_from + "?").c_str(),
                                                                         QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            fd = ::open((std::string("./") + file_name).c_str(), O_RDONLY);
            if (fd > 0)
            {
                ::close(fd);
                messageBox.critical(0, "Error", "File already exist!");
            }
            else
            {
                fd = ::open((std::string("./") + file_name).c_str(),  O_WRONLY | O_APPEND | O_CREAT, 0644);
                if (fd < 0)
                    messageBox.critical(0, "Error", "Cann't create file");
                else
                {
                    if (::write(fd, receive_file_buf.data(), receive_file_buf.size()) < 0)
                        messageBox.critical(0, "Error", "Write error ;(");
                    ::close(fd);
                }
            }
            std::cout << "fd: " << fd << "\n";
        }
        receive_file_buf.clear();
        file_name.erase();
        file_from.erase();
        file_size = 0;
        file_bytes_received = 0;
    }
    size_t i = 0;
    while (pos < r)
        buf[i++] = buf[pos++];
    buf[i] = 0;
    return i;*/
}

int     ChatWindow::append_to_file_buf(size_t pos, unsigned char *buf, size_t r)
{
    size_t  i;
    int     fd;
    QMessageBox         messageBox;

    while (pos < r && file_bytes_received < file_size)
    {
        receive_file_buf.push_back(buf[pos]);
        file_bytes_received++;
        pos++;
    }
    if (file_bytes_received == file_size)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "File transfer", std::string(std::string("Receive file from ") + file_from + "?").c_str(),
                                                                         QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            if ((fd = ::open((std::string("./") + file_name).c_str(), O_RDONLY)) > 0)
            {
                ::close(fd);
                messageBox.critical(0, "Error", "File already exist!");
            }
            else
            {
                if ((fd = ::open((std::string("./") + file_name).c_str(), O_WRONLY | O_APPEND | O_CREAT, 0644)) < 0)
                    messageBox.critical(0, "Error", "Can nor create file");
                else
                {
                    if (::write(fd, receive_file_buf.data(), receive_file_buf.size()) < 0)
                        messageBox.critical(0, "Error", "Write error ;(");
                    ::close(fd);
                }
            }
        }
        receive_file_buf.clear();
        file_name.erase();
        file_from.erase();
        file_size = 0;
        file_bytes_received = 0;
    }
    i = 0;
    while (pos < r)
       buf[i++] = buf[pos++];
    buf[i] = 0;
    return i;
}

void    ChatWindow::ReceiveMessage(void)
{
    QMessageBox     messageBox;
    ssize_t     r = 0;
    unsigned char        buf_read[1024 + 1];
    std::vector<std::string>    split;

    if (tls && sslptr)
        r = SSL_read(sslptr, buf_read, 1024);
    else
        r = recv(sock, buf_read, 1024, 0);
    if (r >= 0)
        buf_read[r] = 0;
    if (!(std::string((char*)buf_read).compare(0, 5, "FILE ")))
        r = receive_file(buf_read, r);
    else if (file_bytes_received < file_size)
        r = append_to_file_buf(0, buf_read,  r);
    if (r > 0)
    {
        split = splitstringbyany(std::string((char*)buf_read), "\r\n");
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
        if (wrbuf.size() > 0 || file_buf.size() > 0)
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
    (void)event;
    exit(0);
}

void    ChatWindow::actionExit(void)
{
    exit(0);
}

void ChatWindow::on_actionExit_triggered()
{
    exit(0);
}

std::string ChatWindow::get_filename_from_path(std::string path)
{
    std::vector<std::string> split = splitstring(path, '/');
    return split.back();
}

void ChatWindow::on_actionSend_file_triggered()
{
    QLabel          lbl;
    std::string     nick;
    std::string     path;
    std::string     filename;
    QMessageBox     messageBox;
    std::string     temp;
    std::vector<unsigned char>    temp_array;
    int             fd;
    int             red;
    unsigned char   buf[1024];
    SendFile        *sf = new SendFile(&lbl);
    lbl.show();

    if (sf->exec())
    {
        nick = sf->getNick();
        path = sf->getPath();
    }
    if ((fd = ::open(path.c_str(), O_RDONLY)) < 0)
    {
        messageBox.critical(0, "Error", "No such file or directory");
        return ;
    }
    filename = get_filename_from_path(path);
    temp = "FILE " + filename + " " + nick + " " + std::string(nickname.toLocal8Bit().constData()) + " ";
    while ((red = read(fd, buf, 1024)))
    {
        copy(buf, buf + red, back_inserter(file_buf));
        if (file_buf.size() > 524288000)
        {
            messageBox.critical(0, "Error", "Can't send more then 1 Gb");
            ::close(fd);
            file_buf.clear();
            return ;
        }
    }
    ::close(fd);
    temp += std::to_string(file_buf.size()) + " :";
    std::copy(temp.c_str(), temp.c_str() + temp.size(), std::back_inserter(temp_array));
    std::copy(file_buf.data(), file_buf.data() + file_buf.size(), back_inserter(temp_array));
    file_buf = temp_array;
}
