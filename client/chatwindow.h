#ifndef CHATWINDOW_H
#define CHATWINDOW_H
#include <QMainWindow>
#include <QWidget>
#include <QString>


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

    void    chatloop(void);
};

#endif // CHATWINDOW_H
