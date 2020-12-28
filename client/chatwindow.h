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

QT_BEGIN_NAMESPACE
namespace Ui { class ChatWindow; }
QT_END_NAMESPACE


class ChatWindow : public QMainWindow
{
    Q_OBJECT
private:
    QString ip;
    QString port;
    QString password;
    QString nickname;
    QString username;
    QString realname;
    Ui::ChatWindow *ui;


public:
    ChatWindow(QString ip, QString port, QString password, QString nickname, QString username, QString realname, QWidget *parent = nullptr);
    ChatWindow();
    ~ChatWindow();
    void    do_connect(bool tls);
    void    chatloop(void);

    int sock;
};

#endif // CHATWINDOW_H
