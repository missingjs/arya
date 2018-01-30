#include "mainwindow.h"
#include <QApplication>

#include <QDebug>
#include <iostream>
#include <QFile>
#include <QTextStream>

#include "taskeditor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    TaskEditor te;
    QFile data("D:\\app.conf.20180130");
    if (data.open(QFile::ReadOnly)) {
        QTextStream out(&data);
        while (!out.atEnd()) {
            QString line = out.readLine();
            te.addLine(line);
        }
    }

    w.setContent(te.tasks());

    return a.exec();
}
