// Header file for FileInsight
#ifndef FILEINSIGHT_H
#define FILEINSIGHT_H

#include <magic.h>
#include <iostream>
#include "constants.h"
#include "fileinsightsubdialog.h"

#include <QWidget>
#include <QMainWindow>
#include <QFileDialog>
#include <QByteArray>
#include <QFileIconProvider>
#include <QProcess>
#include <QMimeDatabase>
#include <QMessageBox>
#include <QProcessEnvironment>

namespace Ui {
class FileInsight;
}

class FileInsight : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileInsight(QWidget *parent = 0);
    ~FileInsight();
    int getBackend();

private slots:
    void on_actionQuit_triggered();
    void on_selectFileButton_clicked();
    void on_actionSelect_triggered();
    void on_reloadButton_clicked();

private:
    Ui::FileInsight *ui;

    void chooseFile();
    void openFile(QString filename);
    QIcon getIcon(QString mimetype);
    QString getMimeType(QString filename);
    QString getMagicInfo(QString filename);
    QString getTridInfo(QString filename);
    const char * QStringToConstChar(QString text);
    FileInsightSubdialog * getCurrentTab();

    QProcess trid_subprocess;
    QString trid_command;
    magic_t magic_cookie;
    magic_t magic_cookie_mime;
    QFileIconProvider iconprovider;
};

#endif // FILEINSIGHT_H
