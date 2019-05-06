// Header file for FileInsight: this declares variables, functions, modules to depend on,
// etc.
#ifndef FILEINSIGHT_H
#define FILEINSIGHT_H

#include "fileinsightbackend.h"
#include "fileinsightsubdialog.h"
#include "libmagicbackend.h"
#include "tridbackend.h"
#include "qmimedatabasebackend.h"

#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QList>
#include <QMainWindow>
#include <QMessageBox>
#include <QMimeData>
#include <QUrl>
#include <QWidget>

#ifdef Q_OS_WIN
#include <windows.h>
#include <QtWinExtras>
#endif

namespace Ui {
class FileInsight;
}

class FileInsight : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileInsight(QWidget *parent = nullptr);
    ~FileInsight();
    void chooseFile();
    FileInsightBackend* getBackend() const;
    FileInsightSubdialog* getCurrentTab();
    QIcon getIcon(QString mimetype, QString filename);
    QString getMagicInfo(QString filename);
    QString getMimeType(QString filename);
    QString getTridInfo(QString filename);
    FileInsightSubdialog* newTab(bool starting = false);
    void openFile(QString filename, bool starting = true);
    const char * QStringToConstChar(QString text);

protected slots:
    void on_actionQuit_triggered();
    void on_selectFileButton_clicked();
    void on_actionSelect_triggered();
    void on_reloadButton_clicked();
    void on_tabWidget_tabCloseRequested(int index);
    void on_addTabButton_clicked();
    void on_actionAbout_triggered();
    void on_actionAbout_Qt_triggered();

private:
    Ui::FileInsight *ui;
    QFileIconProvider iconprovider;

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    FileInsightBackend* magicbackend;
    FileInsightBackend* tridbackend;
    FileInsightBackend* qmimebackend;
};

#endif // FILEINSIGHT_H
