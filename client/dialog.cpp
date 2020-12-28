#include "dialog.h"

Dialog::Dialog(QString ip, QString port) : ip(ip), port(port)
{
    window = new QWidget;
    window->show();
}

Dialog::~Dialog()
{
    delete window;
}
