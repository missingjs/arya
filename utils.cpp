#include "utils.h"

#include <QDir>

Utils::Utils()
{

}

QString Utils::dirname(const QString &path)
{
    return QDir::fromNativeSeparators(QFileInfo(path).path());
}

QString Utils::basename(const QString &path)
{
    return QFileInfo(path).fileName();
}
