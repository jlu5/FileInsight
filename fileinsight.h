// Header file for FileInsight

#include <magic.h>
#include <iostream>

#ifndef FILEINSIGHT_H
#define FILEINSIGHT_H

#include <QMainWindow>
#include <QIcon>
#include <QWidget>
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
    void openFile(QString filename);
    void chooseFile();
    void showIcon(QString mimetype);
    QString getMimeType();
    QString getMagicInfo();
    QString getTridInfo();

private slots:
    void on_actionQuit_triggered();

    void on_selectFileButton_clicked();

    void on_actionSelect_triggered();

    void on_reloadButton_clicked();

private:
    Ui::FileInsight *ui;
    magic_t magic_cookie;
    magic_t magic_cookie_mime;
    QFileIconProvider iconprovider;
    const char * cfilename;
    QByteArray filename_bytes;
    QString last_filename;
    QIcon icon;
    QProcess trid_subprocess;
    QString trid_command;
};

#endif // FILEINSIGHT_H
