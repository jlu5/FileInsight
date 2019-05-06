#ifndef FILEINSIGHTUTILS_H
#define FILEINSIGHTUTILS_H

#include <QByteArray>
#include <QDebug>
#include <QFileIconProvider>
#include <QIcon>
#include <QString>
#include <cstring>

namespace FileInsightUtils
{
    // Converts a QString to const char*. The callee is responsibe for freeing the resulting buffer.
    const char* QStringToConstChar(QString text);

    // Fetch an icon based on the MIME type and filename.
    QIcon getIcon(QString mimetype, QString filename);
}


#endif // FILEINSIGHTUTILS_H
