#ifndef DIALOG_H
#define DIALOG_H

#include <QString>
#include <QWidget>

class QWidget;

class Dialog : public QWidget
{
private:
    QString ip;
    QString port;
    QWidget *dialog;
public:
    Dialog(QString ip, QString port);
    ~Dialog(void);
};

#endif // DIALOG_H
