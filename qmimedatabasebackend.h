#ifndef QMIMEDATABASEBACKEND_H
#define QMIMEDATABASEBACKEND_H

#include "fileinsightbackend.h"
#include "fileinsightutils.h"

#include <QMimeDatabase>

class QMimeDatabaseBackend : public FileInsightBackend
{
public:
    QMimeDatabaseBackend(QWidget* parent);

    QString getMimeType(QString filename) override;
};

#endif // QMIMEDATABASEBACKEND_H
