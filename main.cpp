#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>

#include "taskeditor.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    auto te = TaskEditor::instance();
    QObject::connect(te, &TaskEditor::modified, [&]() {
        w.setWindowModified(true);
    });

    if (argc == 2) {
        QString filePath = QString::fromLocal8Bit(argv[1]);
        QFile data(filePath);
        if (data.open(QFile::ReadOnly)) {
            QTextStream out(&data);
            while (!out.atEnd()) {
                QString line = out.readLine();
                te->addLine(line);
            }
        }

        w.refreshTask();

        QString fileName = Utils::basename(filePath);
        w.setWindowTitle(QString("%1[*] - Arya Editor").arg(fileName));
    }

    return a.exec();
}
