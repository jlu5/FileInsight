#ifndef LIBMAGICBACKEND_H
#define LIBMAGICBACKEND_H

#include <magic.h>
#include "fileinsightbackend.h"
#include "fileinsightutils.h"

#include <QString>
#include <QMessageBox>
#include <QDebug>

class LibmagicBackend : public FileInsightBackend
{
public:
    LibmagicBackend(QWidget* parent);

    QString getExtendedInfo(QString filename) override;
    QString getMimeType(QString filename) override;

private:
    // TODO: allow a global instance of this
    magic_t magic_cookie;
    magic_t magic_cookie_mime;
    bool getError(magic_t magic_cookie);
};

#endif // LIBMAGICBACKEND_H
