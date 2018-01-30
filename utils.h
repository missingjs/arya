#ifndef UTILS_H
#define UTILS_H

#include <QString>

class Utils
{
public:
    Utils();

    static QString dirname(const QString &path);

    static QString basename(const QString &path);
};

#endif // UTILS_H
