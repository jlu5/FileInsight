// Header file for FileInsight

#include <magic.h>
#include <iostream>

#ifndef FILEINSIGHT_H
#define FILEINSIGHT_H

#include <QMainWindow>
#include <QIcon>
#include <QPaintEvent>
#include <QWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QMimeType>
//#include <fimimetype.h>

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

    private slots:
        void on_actionQuit_triggered();

        void on_selectFileButton_clicked();

        void on_actionSelect_triggered();

    private:
        Ui::FileInsight *ui;
        magic_t magic_cookie = 0;
        magic_t magic_cookie_mime = 0;
        QFileIconProvider iconprovider;
};

#endif // FILEINSIGHT_H
