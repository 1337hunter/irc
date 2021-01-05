#include "sendfile.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

SendFile::SendFile(QWidget *parent) : QDialog(parent)
{
    nick_lineEdit = new QLineEdit(this);
    path_lineEdit = new QLineEdit(this);
    nick_lineEdit->setPlaceholderText(QString("Nickname"));
    path_lineEdit->setPlaceholderText(QString("Path to file"));

    QPushButton *sendButton = new QPushButton(tr("Send"));
    sendButton->setDefault(true);

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(sendButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *lt = new QVBoxLayout;
    lt->addWidget(nick_lineEdit);
    lt->addWidget(path_lineEdit);
    lt->addWidget(buttonBox);
    setLayout(lt);
}

std::string SendFile::getNick(void)
{
    return std::string(nick_lineEdit->text().toLocal8Bit().constData());
}

std::string SendFile::getPath(void)
{
    return std::string(path_lineEdit->text().toLocal8Bit().constData());
}
