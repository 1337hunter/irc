/********************************************************************************
** Form generated from reading UI file 'chatwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATWINDOW_H
#define UI_CHATWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatWindow
{
public:
    QWidget *centralwidget;
    QTextBrowser *mainchat;
    QLineEdit *lineEdit;
    QPushButton *send;
    QTreeWidget *treeWidget;
    QMenuBar *menubar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *ChatWindow)
    {
        if (ChatWindow->objectName().isEmpty())
            ChatWindow->setObjectName(QString::fromUtf8("ChatWindow"));
        ChatWindow->setEnabled(true);
        ChatWindow->resize(700, 470);
        ChatWindow->setMinimumSize(QSize(700, 470));
        ChatWindow->setMaximumSize(QSize(700, 470));
        ChatWindow->setMouseTracking(false);
        ChatWindow->setContextMenuPolicy(Qt::DefaultContextMenu);
        ChatWindow->setWindowOpacity(2.000000000000000);
        ChatWindow->setDockOptions(QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks);
        centralwidget = new QWidget(ChatWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        mainchat = new QTextBrowser(centralwidget);
        mainchat->setObjectName(QString::fromUtf8("mainchat"));
        mainchat->setGeometry(QRect(150, 0, 561, 381));
        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(150, 380, 461, 51));
        send = new QPushButton(centralwidget);
        send->setObjectName(QString::fromUtf8("send"));
        send->setGeometry(QRect(610, 380, 91, 51));
        treeWidget = new QTreeWidget(centralwidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setGeometry(QRect(0, 0, 151, 431));
        ChatWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ChatWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 700, 22));
        ChatWindow->setMenuBar(menubar);
        toolBar = new QToolBar(ChatWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        ChatWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        retranslateUi(ChatWindow);

        QMetaObject::connectSlotsByName(ChatWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ChatWindow)
    {
        ChatWindow->setWindowTitle(QApplication::translate("ChatWindow", "IRC chat", nullptr));
        send->setText(QApplication::translate("ChatWindow", "Send", nullptr));
        toolBar->setWindowTitle(QApplication::translate("ChatWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChatWindow: public Ui_ChatWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
