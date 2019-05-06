#include "fileinsightutils.h"

namespace FileInsightUtils
{
    const char* QStringToConstChar(QString text) {
        // Convert QStrings to const char *, so that it can be used by libmagic, etc.
        // Source: http://doc.qt.io/qt-5/qbytearray.html#data
        QByteArray bytes = text.toUtf8();
        // Explicitly make a copy of the byte array data, so it can't be destroyed and
        // and corrupt by garbage collection.
        char *data = new char[bytes.size() + 1];
        std::strcpy(data, bytes.constData());
        qDebug() << "QStringToConstChar output: " << data;
        return data;
    }
}
