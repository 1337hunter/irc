/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *connect;
    QLabel *label;
    QLineEdit *ip;
    QLineEdit *port;
    QLabel *label_2;
    QLineEdit *password;
    QLabel *label_3;
    QLineEdit *nickname;
    QLineEdit *username;
    QLineEdit *realname;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QCheckBox *tls;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(216, 265);
        MainWindow->setMinimumSize(QSize(216, 265));
        MainWindow->setMaximumSize(QSize(216, 265));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        connect = new QPushButton(centralwidget);
        connect->setObjectName(QString::fromUtf8("connect"));
        connect->setGeometry(QRect(80, 220, 101, 31));
        connect->setMinimumSize(QSize(0, 0));
        connect->setMaximumSize(QSize(999, 999));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 41, 21));
        ip = new QLineEdit(centralwidget);
        ip->setObjectName(QString::fromUtf8("ip"));
        ip->setGeometry(QRect(70, 10, 111, 21));
        port = new QLineEdit(centralwidget);
        port->setObjectName(QString::fromUtf8("port"));
        port->setGeometry(QRect(70, 40, 111, 21));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 40, 51, 17));
        password = new QLineEdit(centralwidget);
        password->setObjectName(QString::fromUtf8("password"));
        password->setGeometry(QRect(70, 70, 111, 21));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 70, 51, 21));
        nickname = new QLineEdit(centralwidget);
        nickname->setObjectName(QString::fromUtf8("nickname"));
        nickname->setGeometry(QRect(100, 130, 81, 21));
        username = new QLineEdit(centralwidget);
        username->setObjectName(QString::fromUtf8("username"));
        username->setGeometry(QRect(100, 160, 81, 21));
        realname = new QLineEdit(centralwidget);
        realname->setObjectName(QString::fromUtf8("realname"));
        realname->setGeometry(QRect(100, 190, 81, 21));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 130, 71, 21));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(20, 160, 71, 21));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(20, 190, 71, 21));
        tls = new QCheckBox(centralwidget);
        tls->setObjectName(QString::fromUtf8("tls"));
        tls->setGeometry(QRect(70, 100, 61, 21));
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        connect->setText(QApplication::translate("MainWindow", "connect", nullptr));
        label->setText(QApplication::translate("MainWindow", "ip ", nullptr));
        ip->setText(QString());
#ifndef QT_NO_TOOLTIP
        port->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        port->setText(QString());
        label_2->setText(QApplication::translate("MainWindow", "port", nullptr));
        password->setText(QString());
        label_3->setText(QApplication::translate("MainWindow", "pass", nullptr));
        nickname->setText(QString());
        username->setText(QString());
        realname->setText(QString());
        label_4->setText(QApplication::translate("MainWindow", "nickname", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "username", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "realname", nullptr));
        tls->setText(QApplication::translate("MainWindow", "tls", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
