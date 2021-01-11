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
    SSL                         *file_sslptr;
    int                         file_sock;
    int                         file_port;
    std::vector<unsigned char>  file_buf;
    std::vector<unsigned char>  file_header;
    size_t                      file_bytes_received;
    std::string                 file_name;
    std::string                 file_from;


    ChatWindow(QString ip, QString port, QString password, QString nickname, QString username, QString realname,
               bool tls, bool connected, size_t file_size, QWidget *parent = nullptr);
    ChatWindow();
    ~ChatWindow();

    void    do_connect(void);
    void    run(void);
    void    ReceiveMessage(void);
    void    SendMessage(bool file_ready);
    void    ProcessMessage(std::string msg);
    void    ProcessReply(std::vector<std::string> &split);
    void    keyPressEvent(QKeyEvent* event);
    void    closeEvent(QCloseEvent* event);
    //funny tools
    std::vector<std::string>   splitstring(std::string str, char delim);
    std::vector<std::string>   splitstringbyany(std::string msg, std::string const &delim);
    std::vector<std::string>   splitcmdbyspace(std::string msg);
    std::string strtoupper(std::string const &str);
    std::string strtolower(std::string const &str);
    void        error_exit(std::string msg);
    //ft
    void    reply_who_I_am(std::vector<std::string> &split);
    //int     receive_file(unsigned char *buf, size_t r);
    //int     append_to_file_buf(size_t pos, unsigned char *buf, size_t r);
    std::string get_filename_from_path(std::string path);
private slots:
    void    actionExit(void);
    void    on_actionExit_triggered();
    //void    on_actionExit_toggled(bool arg1); dont know yet how to delit it from moc_chatwindow.h
    void    on_actionSend_file_triggered();
};

#endif // CHATWINDOW_H
