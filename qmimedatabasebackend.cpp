#include "qmimedatabasebackend.h"

QMimeDatabaseBackend::QMimeDatabaseBackend(QWidget* parent) : FileInsightBackend(parent)
{

}

QString QMimeDatabaseBackend::getMimeType(QString filename) {
    QMimeDatabase mimedb;
    QMimeType mimeobj;
    mimeobj = mimedb.mimeTypeForFile(filename);
    return mimeobj.name();
}
