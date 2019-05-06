#include "fileinsightutils.h"

namespace FileInsightUtils
{
    // Converts a QString to const char*. The callee is responsibe for freeing the resulting buffer.
    const char* QStringToConstChar(QString text) {
        QByteArray bytes = text.toUtf8();
        // Explicitly make a copy of the byte array data, so it can't be destroyed and
        // and corrupt by garbage collection.
        char* data = new char[bytes.size() + 1];
        std::strcpy(data, bytes.constData());
        return data;
    }
}
