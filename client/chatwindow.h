#ifndef CHATWINDOW_H
#define CHATWINDOW_H
#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <fcntl.h>
#include <QTime>
#include <QThread>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

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
    ChatWindow(QString ip, QString port, QString password, QString nickname, QString username, QString realname,
               bool tls, bool connected, QWidget *parent = nullptr);
    ChatWindow();
    ~ChatWindow();

    void    do_connect(void);

    void    run(void);
    void    ReceiveMessage(void);
    void    SendMessage(void);
private slots:
    void on_send_clicked();
};

#endif // CHATWINDOW_H
