#include "mainwindow.h"
#include <QApplication>

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
        te->resetByFile(filePath);
        w.refreshTask();
        QString fileName = Utils::basename(filePath);
        w.setWindowTitle(QString("%1[*] - Arya Editor").arg(fileName));
    }

    return a.exec();
}
