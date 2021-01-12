#ifndef CHATWINDOW_H
#define CHATWINDOW_H
#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <fcntl.h>
#include <QTime>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <QKeyEvent>
#include <QCloseEvent>
#include <vector>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class ChatWindow; }
QT_END_NAMESPACE


class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString ip;
    QString port;
    QString password;
    QString nickname;
    QString username;
    QString realname;
    bool    tls;
    bool    connected;
    size_t  file_size;
    Ui::ChatWindow *ui;

    SSL_CTX *sslctx;
    SSL     *sslptr;
    int     sock;

    fd_set  fdset_read;
    fd_set  fdset_write;
    std::string wrbuf;
    std::string rdbuf;

    std::string                 external_ip;
    int                         file_fd;
    int                         file_sock;
    int                         file_port;
    bool                        file_sock_status;
    int                         dcc_sock;
    bool						dcc_sock_status;
	SSL                         *dcc_sslptr;
    std::vector<unsigned char>  file_buf;
    size_t                      file_bytes_received;
    std::string                 file_name;
    std::string                 file_from;


    ChatWindow(QString ip, QString port, QString password, QString nickname, QString username, QString realname,
               bool tls, bool connected, size_t file_size, QWidget *parent = nullptr);
    ChatWindow();
    ~ChatWindow();

    void    do_connect(void);
    int     do_sock(std::string _ip, std::string _port);
    void    run(void);
    void    ReceiveMessage(void);
    void    SendMessage(void);
    void    Accept(void);
    void    ProcessMessage(std::string msg);
    void    TransferFile(void);
    void    ReceiveFile(void);
    void    ProcessReply(std::vector<std::string> &split);
    void    keyPressEvent(QKeyEvent* event);
    void    closeEvent(QCloseEvent* event);

    //funny tools
    std::vector<std::string>   splitstring(std::string str, char delim);
    std::vector<std::string>   splitstringbyany(std::string msg, std::string const &delim);
    std::vector<std::string>   splitcmdbyspace(std::string msg);
    std::string get_nick_from_info(std::string const &info);
    std::string strtoupper(std::string const &str);
    std::string strtolower(std::string const &str);
    void        error_exit(std::string msg);
    void        closeDCC(void);
    void        receive_file(std::vector<std::string> &split);


    void        cmd_privmsg(std::vector<std::string> &split);
    //ft
    void    reply_who_I_am(std::vector<std::string> &split);
    std::string get_filename_from_path(std::string path);
private slots:
    void    actionExit(void);
    void    on_actionExit_triggered();
    void    on_actionSend_file_triggered();
};

#endif // CHATWINDOW_H
