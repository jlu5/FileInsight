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
    explicit FileInsightSubdialog(QWidget *parent = 0);
    ~FileInsightSubdialog();
    Ui::FileInsightSubdialog *ui;
    QString filename;

private:
};

#endif // FILEINSIGHTSUBDIALOG_H
