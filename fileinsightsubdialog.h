#ifndef FILEINSIGHTSUBDIALOG_H
#define FILEINSIGHTSUBDIALOG_H

#include <QWidget>
#include <QIcon>

namespace Ui {
class FileInsightSubdialog;
}

class FileInsightSubdialog : public QWidget
{
    Q_OBJECT

public:
    explicit FileInsightSubdialog(QWidget *parent = nullptr);
    ~FileInsightSubdialog();
    Ui::FileInsightSubdialog *ui;
    QString filename;
};

#endif // FILEINSIGHTSUBDIALOG_H
