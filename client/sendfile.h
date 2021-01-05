#ifndef SENDFILE_H
#define SENDFILE_H

#include <QDialog>
#include <QLineEdit>
#include <string>

class SendFile : public QDialog
{
    Q_OBJECT
public:
    explicit SendFile(QWidget *parent = 0);
    std::string getNick();
    std::string getPath();
private:
    QLineEdit   *nick_lineEdit;
    QLineEdit   *path_lineEdit;
    std::string nick;
    std::string path;
};

#endif // SENDFILE_H
