#include "mainwindow.h"
#include <iostream>
#include <signal.h>
#include <QApplication>
#include <QString>

void	error_exit(std::string const &s1)
{
	std::cerr << s1 << std::endl;
	exit(1);
}

int		main(int argc, char *argv[])
{
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		error_exit("signal error");

	QApplication a(argc, argv);
	MainWindow w;
	MainWindow client;
	w.show();
	return a.exec();
}
