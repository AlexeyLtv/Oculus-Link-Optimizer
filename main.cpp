#include "linkpriority.h"

#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LinkPriority w;

    w.setFixedSize(358, 646);

    w.show();
    return a.exec();
}
