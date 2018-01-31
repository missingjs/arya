#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // FIXME: open file after window display
    if (argc == 2) {
        QString filePath = QString::fromLocal8Bit(argv[1]);
        w.openFile(filePath);
    }

    return a.exec();
}
