#ifndef FILEINSIGHTBACKEND_H
#define FILEINSIGHTBACKEND_H

#include <QString>
#include <QMainWindow>
#include <QObject>

class FileInsight; // forward decl

class FileInsightBackend : public QObject
{
    Q_OBJECT

public:
    FileInsightBackend(QWidget* parent);

    virtual QString getExtendedInfo(QString filename);

    /* Fetch the MIME type of the selected file */
    virtual QString getMimeType(QString filename) = 0;
    virtual ~FileInsightBackend() = 0;

protected:
    QWidget* parent;
};

#endif // FILEINSIGHTBACKEND_H
