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
    Ui::ChatWindow *ui;

    SSL_CTX *sslctx;
    SSL     *sslptr;
    int     sock;
    fd_set  fdset_read;
    fd_set  fdset_write;
    std::string wrbuf;
    std::string rdbuf;
    std::vector<unsigned char>  file_buf;
    ChatWindow(QString ip, QString port, QString password, QString nickname, QString username, QString realname,
               bool tls, bool connected, QWidget *parent = nullptr);
    ChatWindow();
    ~ChatWindow();

    void    do_connect(void);
    void    run(void);
    void    ReceiveMessage(void);
    void    SendMessage(void);
    void    keyPressEvent(QKeyEvent* event);
    void    closeEvent(QCloseEvent* event);
    std::vector<std::string>   splitstring(std::string str, char delim);
    std::vector<std::string>   splitstringbyany(std::string msg, std::string const &delim);
    std::string get_filename_from_path(std::string path);
private slots:
    void    actionExit(void);
    void    on_actionExit_triggered();
    //void    on_actionExit_toggled(bool arg1); dont know yet how to delit it from moc_chatwindow.h
    void    on_actionSend_file_triggered();
};

#endif // CHATWINDOW_H
