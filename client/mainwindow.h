#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QWidget>
#include "chatwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ChatWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    ChatWindow *chat;
    void    keyPressEvent(QKeyEvent *event);

private slots:

    void    on_connect_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
