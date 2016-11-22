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
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QMimeData>
#include <QList>

namespace Ui {
class FileInsight;
}

class FileInsight : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileInsight(QWidget *parent = 0);
    ~FileInsight();
    void chooseFile();
    int getBackend();
    FileInsightSubdialog * getCurrentTab();
    QIcon getIcon(QString mimetype);
    QString getMagicInfo(QString filename);
    QString getMimeType(QString filename);
    QString getTridInfo(QString filename);
    FileInsightSubdialog * newTab(bool starting = false);
    void openFile(QString filename, bool starting = true);
    const char * QStringToConstChar(QString text);

private slots:
    void on_actionQuit_triggered();
    void on_selectFileButton_clicked();
    void on_actionSelect_triggered();
    void on_reloadButton_clicked();

    void on_tabWidget_tabCloseRequested(int index);

    void on_addTabButton_clicked();

private:
    Ui::FileInsight *ui;

    QProcess trid_subprocess;
    QString trid_command;
    magic_t magic_cookie;
    magic_t magic_cookie_mime;
    QFileIconProvider iconprovider;

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif // FILEINSIGHT_H
