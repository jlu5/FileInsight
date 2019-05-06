#ifndef TRIDBACKEND_H
#define TRIDBACKEND_H

#include "libmagicbackend.h"
#include "fileinsightutils.h"
#include <QProcess>
#include <QProcessEnvironment>

class TrIDBackend : public LibmagicBackend
{
public:
    TrIDBackend(QWidget* parent);

    QString getExtendedInfo(QString filename) override;

private:
    QProcess trid_subprocess;
    QString trid_command;
};

#endif // TRIDBACKEND_H
