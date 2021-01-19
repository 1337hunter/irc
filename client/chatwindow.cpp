#include "chatwindow.h"
#include "ui_chatwindow.h"
#include <iostream>
#include <QMessageBox>
#include <ctime>
#include "sendfile.h"
#include <QLabel>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <pwd.h>
#include <list>


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
    file_sock_status = false;
    dcc_sock_status = false;
    sock = -1;
    dcc_sock = -1;
    file_sock = -1;
    ui->setupUi(this);
}

void    ChatWindow::error_exit(std::string msg)
{
    QMessageBox messageBox;

    messageBox.critical(0, "Error", msg.c_str());
    exit(1);
}

int    ChatWindow::do_sock(std::string _ip, std::string _port)
{
    t_addrinfo      hints;
    t_addrinfo      *addr;
    QMessageBox     messageBox;
    int             temp_socket;

    hints.ai_flags = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    if ((temp_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        messageBox.critical(0, "Error", "(socket)");
        return -1;
    }
    if (::getaddrinfo(_ip.c_str(), _port.c_str(), &hints, &addr))
    {
        messageBox.critical(0, "Error", "(getaddrinfo)");
        return -1;
    }
    int tries = 6;
    while (--tries)
	{
		int res = ::connect(temp_socket, addr->ai_addr, addr->ai_addrlen);
        if (res == 0)
			break ;
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
    if (!tries)
    {
        messageBox.critical(0, "Error", "Conntion failed");
        return -1;
    }
    return temp_socket;
}

void    ChatWindow::do_connect(void)
{
    t_addrinfo      hints;
    t_addrinfo      *addr;
    QMessageBox     messageBox;
    int             temp_socket;
    SSL             *temp_sslptr = 0;

    hints.ai_flags = 0;
    messageBox.setFixedSize(200,100);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    if ((temp_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
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
        int res = ::connect(temp_socket, addr->ai_addr, addr->ai_addrlen);
        ui->mainchat->append("Connecting ...");
        if (res == 0)
        {
            if (!tls)
                connected = true;
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
            messageBox.critical(0, "Error", "SSL error.\n(SSL_CTX_new)");
            return ;
        }
        if (SSL_CTX_set_ecdh_auto(ctx, 1) <= 0)
        {
            messageBox.critical(0, "Error", "SSL error.\n(SSL_CTX_set_ecdh_auto)");
            return ;
        }
        if ((temp_sslptr = SSL_new(sslctx)) == 0)
        {
            messageBox.critical(0, "Error", "SSL error.\n(SSL_new)");
            return ;
        }
        if (!(SSL_set_fd(temp_sslptr, temp_socket)))
        {
            messageBox.critical(0, "Error", "SSL error.\n(SSL_set_fd)");
            return ;
        }
        int     handshake = 0;
        handshake = SSL_connect(temp_sslptr);
        if (handshake != 1)
        {
            int err = SSL_get_error(temp_sslptr, handshake);
            if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE)
                exit(0);
        }
        connected = true;
    }
    if (fcntl(temp_socket, F_SETFL, O_NONBLOCK) < 0)
    {
        messageBox.critical(0, "Error", "Somethig went wrong.(fcntl)");
        return ;
    }
    if (!password.isEmpty())
        wrbuf += "PASS " + std::string(password.toLocal8Bit().constData()) + "\r\n";
    sslptr = temp_sslptr;
    sock = temp_socket;
    wrbuf += "NICK " + std::string(nickname.toLocal8Bit().constData()) + "\r\n";
    wrbuf += "USER " + std::string(username.toLocal8Bit().constData()) + " 0 * :" + std::string(realname.toLocal8Bit().constData()) + "\r\n";
    wrbuf += "WHO " + std::string(nickname.toLocal8Bit().constData()) + "\r\n";
    freeaddrinfo(addr);
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
    if (r < 0 && file_buf.empty())
    {
        connected = false;
        messageBox.critical(0, "Error", "Conenction terminated!");
    }
    wrbuf.erase();
}

std::string ChatWindow::get_nick_from_info(std::string const &info)
{
    size_t  pos = 0;
    std::string nick;

    if (info.empty())
        return nick;
    if (info[pos] == ':')
        pos++;
    while (pos < info.size() && info[pos] != '!')
        nick += info[pos++];
    return nick;
}

void    ChatWindow::receive_file(std::vector<std::string> &split)
{
    QMessageBox         messageBox;
    QMessageBox::StandardButton reply;
    uint32_t    ip32;
    std::string ips;
    size_t      usize;
    int         remainder;
    std::list<std::string>  prefix;
    std::list<std::string>::iterator pos;


    prefix.push_back("bytes"); prefix.push_back("KB"); prefix.push_back("MB"); prefix.push_back("TB");
    file_name.erase();
    size_t i = 0;
    if (split[5][i] == '"')
        i++;
    while (i < split[5].size() && split[5][i] != '"')
        file_name += split[5][i++];
    file_from = get_nick_from_info(split[0]);
    split[8].pop_back();
    file_size = stoi(split[8]);
    usize = file_size;
    pos = prefix.begin();
    remainder = 0;
    while (usize >= 1024)
    {
        pos++;
        remainder = usize % 1024;
        while (remainder > 10)
            remainder /= 2;
        usize /= 1024;
    }
    reply = QMessageBox::question(this, "File transfer",
                std::string(std::string("Receive file " + file_name + " (" + std::to_string(usize) + "," + std::to_string(remainder) +
                " " + *pos + ") from ") + file_from + "?").c_str(), QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No || reply == QMessageBox::Close)
    {
        wrbuf += "NOTICE " + file_from + " :" + (char)(1) + "DCC REJECT SEND file" + (char)(1) + "\r\n";
        file_name.erase(); file_size = 0; file_from.erase();
        return ;
    }
    file_bytes_received = 0;
    int tries = 0;
    struct passwd *pw = getpwuid(getuid());
    if ((file_fd = open(std::string(std::string(pw->pw_dir) + "/Downloads/" + file_name).c_str(), O_RDONLY)) >= 0)
    {
        ::close(file_fd);
        tries++;
    }

    std::string dir_file;
    while (tries)
    {
        dir_file = std::string(pw->pw_dir) + "/Downloads/" + file_name + "(" + std::to_string(tries) + ")";
        if ((file_fd = open(dir_file.c_str(), O_RDONLY)) >= 0)
        {
            ::close(file_fd);
            tries++;
        }
        else
        {
            ::close(file_fd);
            break ;
        }
    }

    dir_file = std::string(pw->pw_dir) + "/Downloads/" + file_name + (tries == 0 ? "" : "(" + std::to_string(tries) + ")");
    if ((file_fd = open(dir_file.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0644)) < 0)
    {
        messageBox.critical(0, "Error", "Can not create file"); return ;
    }
    try { ip32 = stoul(split[6]); } catch(...) { return ;}
    ips = std::to_string((ip32 >> 24) & 0xFF) + '.' + std::to_string((ip32 >> 16) & 0xFF) + '.' + std::to_string((ip32 >> 8) & 0xFF) + '.' +
            std::to_string(ip32 & 0xFF);
    dcc_sock = do_sock(ips, split[7]);
    dcc_sock_status = true;
}

void    ChatWindow::ReceiveFile(void)
{
    QMessageBox     messageBox;
    ssize_t         r = 0;
    unsigned char   buf[1024];


    //if (tls && dcc_sslptr)
        //r = SSL_read(sslptr, buf, 1024);
    //else
    r = recv(dcc_sock, buf, 1024, 0);
    if (r > 0)
        r = write(file_fd, buf, r);
    else if (r <= 0)
    {
        if (r < 0)
            messageBox.critical(0, "Error", "File receive error ;(");
        closeDCC();
        //::close(file_fd);
    }
}


std::string ChatWindow::strtolower(std::string const &str)
{
    std::string res = str;

    for (int i = 0; res[i] != '\0'; i++)
    {
        if (res[i] >= 'A' && res[i] <= 'Z')
            res[i] = std::tolower(res[i]);
    }
    return (res);
}

std::string ChatWindow::strtoupper(std::string const &str)
{
    std::string res = str;

    for (int i = 0; res[i] != '\0'; i++)
    {
        if (res[i] >= 'a' && res[i] <= 'z')
            res[i] = std::toupper(res[i]);
    }
    return (res);
}

std::vector<std::string>   ChatWindow::splitcmdbyspace(std::string msg)
{
    std::vector<std::string>       split;
    size_t          pos = 0;
    std::string     tmp;

    while ((pos = msg.find_first_of(" ")) != std::string::npos)
    {
        tmp = msg.substr(0, pos);
        if (!tmp.empty())
            split.push_back(tmp);
        msg.erase(0, pos + 1);
        if (!msg.empty() && msg[0] == ':')
            break ;
    }
    if (!msg.empty())
        split.push_back(msg);
    size_t  i = 0;
    if (split.size() > 0 && split[0][0] == ':')
        i = 1;
    if (split.back().size() > 0 && split.back()[0] == ':' &&
        (strtoupper(split[i]) != "PRIVMSG"))
        split.back() = split.back().substr(1);
    return split;
}

void    ChatWindow::reply_who_I_am(std::vector<std::string> &split)
{
    external_ip = split[5];
}

void    ChatWindow::ProcessReply(std::vector<std::string> &split)
{
    int reply_number;

    reply_number = stoi(split[1]);

    if (reply_number == 352 && split[2] == std::string(nickname.toLocal8Bit().constData()))
        reply_who_I_am(split);
}

void    ChatWindow::cmd_privmsg(std::vector<std::string> &split)
{
    std::string header = ":";
    header += (char)(1);
    header += "DCC";

    if (split.size() > 8 && split[3] == header && split[4] == "SEND")
        receive_file(split);
    else if (split.size() <= 8 && split.size() > 3 && split[3] == header)
        wrbuf += "NOTICE " + get_nick_from_info(split[0]) + " :DCC - not enouth parameters\r\n";
}

void    ChatWindow::cmd_ping(std::vector<std::string> split)
{
    if (!split[1].empty() && split[1][0] == ':')
        wrbuf += "PONG " + std::string(split[1], 1) + "\r\n";
    else if (!split[1].empty())
        wrbuf += "PONG " + split[1] + "\r\n";
}

void    ChatWindow::ProcessMessage(std::string msg)
{

    std::vector<std::string>   split = splitstring(msg, ' ');

    if (split.empty())
        return ;
    if (split.size() > 3 && split[1].size() == 3 &&
        split[1].find_first_not_of("0123456789") == std::string::npos)
        ProcessReply(split);
    if  (split.size() > 2 && split[1] == "PRIVMSG")
        cmd_privmsg(split);
    if (split.size() > 1 && split[0] == "PING")
        cmd_ping(split);
}

void    ChatWindow::ReceiveMessage(void)
{
    QMessageBox     messageBox;
    ssize_t         r = 0;
    unsigned char   buf[1024 + 1];
    std::vector<std::string>    split;


    if (tls && sslptr)
        r = SSL_read(sslptr, buf, 1024);
    else
        r = recv(sock, buf, 1024, 0);

    if (r >= 0)
        buf[r] = 0;
    if (r > 0)
    {
        split = splitstringbyany(std::string((char*)buf), "\r\n");
        for (size_t i = 0; i < split.size(); i++)
            ProcessMessage(split[i]);
        for (size_t i = 0; i < split.size(); ++i)
            ui->mainchat->append(split[i].c_str());
    }
    else if (r <= 0)
    {
        connected = false;
        messageBox.critical(0, "Error", "Conenction terminated!");
    }
}
void    ChatWindow::Accept(void)
{
   struct sockaddr_in   csin;
   socklen_t            csin_len;
   QMessageBox          messageBox;

   csin_len = sizeof(csin);
   dcc_sock = accept(file_sock, (struct sockaddr*)&csin, &csin_len);
   if (dcc_sock < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
   {
       messageBox.critical(0, "Error", "accept"); return ;
   }
   else if (dcc_sock < 0)
   {
       errno = 0;
       return ;
   }
   if (fcntl(dcc_sock, F_SETFL, O_NONBLOCK) < 0)
   {
       messageBox.critical(0, "Error", "accept fcntl"); return ;
   }
   /*if (tls)
   {
       if (!(dcc_sslptr = SSL_new(sslctx)))
       {
           messageBox.critical(0, "Error", "SSL_new"); return ;
       }
       if (SSL_set_fd(dcc_sslptr, dcc_sock) < 1)
       {
           messageBox.critical(0, "Error", "SSL_new"); return ;
       }
   }*/
   dcc_sock_status = true;
   file_sock_status = false;
}

void    ChatWindow::closeDCC(void)
{
    dcc_sock_status = false;
   // if (tls)
  //  {
       // SSL_shutdown(dcc_sslptr);
      //  SSL_free(dcc_sslptr);
   // }
    ::close(dcc_sock);
    dcc_sock = -1;
}

void    ChatWindow::TransferFile(void)
{
    int red = 0;
    unsigned char   buf[1024];
    ssize_t         r;
    QMessageBox     messageBox;

  //  if (tls && dcc_sslptr)
    //    r = SSL_write(dcc_sslptr, file_buf.data(), std::min(file_buf.size(), (size_t)1024));
   // else
        r = write(dcc_sock, file_buf.data(), std::min(file_buf.size(), (size_t)1024));
    if (r < 0)
    {
        messageBox.critical(0, "Error", "SendFile"); return ;
    }
    if (r > 0)
        file_buf.erase(file_buf.begin(), file_buf.begin() + r);
    if ((red = read(file_fd, buf, 1024)) < 0)
    {
        messageBox.critical(0, "Error", "Bad file descriptor.");
        closeDCC();
        return ;
    }
    if (red == 0)
    {
        ::close(file_fd);
        ::close(file_sock);
        file_sock = -1;
        closeDCC();
        return ;
    }
    for (int i = 0; i < red; i++)
        file_buf.push_back(buf[i]);
}

void    ChatWindow::run(void)
{
    int readyfds;
    timeval timeout;

    while (true)
    {
        if (!connected)
            break ;
        timeout = {0, 50000};
        FD_ZERO(&fdset_read);
        FD_ZERO(&fdset_write);
        FD_SET(sock, &fdset_read); //messages read
        if (file_sock_status)
            FD_SET(file_sock, &fdset_read);//w8ing for connection accept
        if (wrbuf.size() > 0)
            FD_SET(sock, &fdset_write);//messages write
        if (dcc_sock_status && file_buf.size() > 0)
            FD_SET(dcc_sock, &fdset_write);//dcc write
        else if (dcc_sock_status)
            FD_SET(dcc_sock, &fdset_read);//dcc read

        if ((readyfds = select(std::max(std::max(sock, dcc_sock), file_sock) + 1, &fdset_read, &fdset_write, 0, &timeout)) < 0)
            error_exit("select error!");

        if (FD_ISSET(sock, &fdset_read))
            ReceiveMessage();
        if (FD_ISSET(sock, &fdset_write))
            SendMessage();
        if (file_sock_status && FD_ISSET(file_sock, &fdset_read))
            Accept();
        if (dcc_sock_status && FD_ISSET(dcc_sock, &fdset_write))
            TransferFile();
       else if (dcc_sock_status && FD_ISSET(dcc_sock, &fdset_read))
            ReceiveFile();
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
    SendFile        *sf = new SendFile(&lbl);
    struct sockaddr_in  sockin;
    struct protoent     *pe = NULL;
    int                 port;
    int                 red;
    struct in_addr      ip_store;
    struct stat         file_stat;
    unsigned char       buf[1024];
    int                 optval = 1;

    lbl.show();
    if (sf->exec())
    {
        nick = sf->getNick();
        path = sf->getPath();
    }

    if (nick.empty() || path.empty())
    {
        messageBox.critical(0, "Warning", "No path or nick are given.");
        return ;
    }


	if (!(pe = getprotobyname("tcp")))
    {
        messageBox.critical(0, "Error", "getprotobyname.");
        exit(0);
    }
    if ((file_sock = socket(PF_INET, SOCK_STREAM, pe->p_proto)) < 0)
		error_exit("socket error");
    if (fcntl(file_sock, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
	sockin.sin_family = AF_INET;
    if (external_ip.empty())
    {
        messageBox.critical(0, "Error", "No ip given. Try to obtain ip with 'WHO <selfnick>' query");
        return ;
    }
	else
        sockin.sin_addr.s_addr = inet_addr(external_ip.c_str());
    if (setsockopt(file_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
        error_exit("set socket option returned error");
    port = 5550;
    sockin.sin_port = htons(port);
    while (bind(file_sock, (struct sockaddr*)&sockin, sizeof(sockin)) < 0 && port < 6000)
        sockin.sin_port = htons(++port);
    if (port == 6000)
    {
        messageBox.critical(0, "Error", "Bind error"); return ;
    }
    file_port = port;
    if (listen(file_sock, 1) < 0)
    {
        messageBox.critical(0, "Error", "listen"); return ;
    }
    if (!inet_aton(external_ip.c_str(), &ip_store))
    {
        messageBox.critical(0, "Error", "inet_aton"); return ;
    }
    filename = get_filename_from_path(path);
    if ((file_fd = open(path.c_str(), O_RDONLY)) < 0)
    {
        messageBox.critical(0, "Error", "inet_aton"); return ;
    }
    if (fstat(file_fd, &file_stat))
    {
        messageBox.critical(0, "Error", "fstat"); return ;
    }
    wrbuf +=  "PRIVMSG " + nick + " :" + (char)(1) + "DCC SEND \"" + filename + "\" " + std::to_string(::htonl(ip_store.s_addr)) +
            " " + std::to_string(port) + " " + std::to_string(file_stat.st_size) + (char)(1) +  "\r\n";
    file_sock_status = true;
    if ((red = read(file_fd, buf, 1024)) < 0)
    {
        messageBox.critical(0, "Error", "Bad file descriptor.");
        ::close(file_sock);
        return ;
    }
    for (int i = 0; i < red; i++)
        file_buf.push_back(buf[i]);
}
