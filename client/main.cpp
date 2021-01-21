#include "mainwindow.h"
#include <signal.h>
#include <QApplication>
#include <QString>



int main(int argc, char *argv[])
{
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		error_exit("signal error");

    QApplication a(argc, argv);
    MainWindow w;
    MainWindow client;
    w.show();
    return a.exec();
}
