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
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatWindow
{
public:
    QAction *actionExit;
    QAction *actionSend_file;
    QWidget *centralwidget;
    QTextBrowser *mainchat;
    QLineEdit *lineEdit;
    QMenuBar *menubar;
    QMenu *menuFile;

    void setupUi(QMainWindow *ChatWindow)
    {
        if (ChatWindow->objectName().isEmpty())
            ChatWindow->setObjectName(QString::fromUtf8("ChatWindow"));
        ChatWindow->setEnabled(true);
        ChatWindow->resize(700, 416);
        ChatWindow->setMinimumSize(QSize(700, 416));
        ChatWindow->setMaximumSize(QSize(700, 416));
        ChatWindow->setMouseTracking(false);
        ChatWindow->setContextMenuPolicy(Qt::DefaultContextMenu);
        ChatWindow->setWindowOpacity(2.000000000000000);
        ChatWindow->setTabShape(QTabWidget::Triangular);
        ChatWindow->setDockOptions(QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks);
        actionExit = new QAction(ChatWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionExit->setCheckable(false);
        actionSend_file = new QAction(ChatWindow);
        actionSend_file->setObjectName(QString::fromUtf8("actionSend_file"));
        centralwidget = new QWidget(ChatWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        mainchat = new QTextBrowser(centralwidget);
        mainchat->setObjectName(QString::fromUtf8("mainchat"));
        mainchat->setGeometry(QRect(0, 0, 700, 361));
        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(0, 360, 701, 35));
        lineEdit->setAutoFillBackground(true);
        ChatWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ChatWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 700, 22));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        ChatWindow->setMenuBar(menubar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionSend_file);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);

        retranslateUi(ChatWindow);

        QMetaObject::connectSlotsByName(ChatWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ChatWindow)
    {
        ChatWindow->setWindowTitle(QApplication::translate("ChatWindow", "IRC chat", nullptr));
        actionExit->setText(QApplication::translate("ChatWindow", "Exit", nullptr));
        actionSend_file->setText(QApplication::translate("ChatWindow", "Send file", nullptr));
        menuFile->setTitle(QApplication::translate("ChatWindow", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChatWindow: public Ui_ChatWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
