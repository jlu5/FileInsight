#ifndef FILEINSIGHTSUBDIALOG_H
#define FILEINSIGHTSUBDIALOG_H

#include "constants.h"
#include <QWidget>
#include <QIcon>

namespace Ui {
class FileInsightSubdialog;
}

class FileInsightSubdialog : public QWidget
{
    Q_OBJECT

public:
    explicit FileInsightSubdialog(QWidget *parent = 0);
    ~FileInsightSubdialog();
    Ui::FileInsightSubdialog *ui;
    QString filename;

/*
private slots:
    void on_selectFileButton_clicked();

    void on_actionSelect_triggered();

    void on_reloadButton_clicked();
*/

private:
};

#endif // FILEINSIGHTSUBDIALOG_H
