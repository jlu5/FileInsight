#include "fileinsightsubdialog.h"
#include "ui_fileinsightsubdialog.h"

FileInsightSubdialog::FileInsightSubdialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileInsightSubdialog)
{
    ui->setupUi(this);
}

FileInsightSubdialog::~FileInsightSubdialog()
{
    delete ui;
}



    //ui->iconDisplay->setPixmap(icon.pixmap(128,128));
//}

/*
void FileInsightSubdialog::on_selectFileButton_clicked()
{
    this->chooseFile();
}

void FileInsightSubdialog::on_actionSelect_triggered()
{
    this->chooseFile();
}

void FileInsightSubdialog::on_reloadButton_clicked()
{
    if (!this->last_filename.isEmpty()) {
        this->openFile(this->last_filename);
     } else {
        QMessageBox::critical(this, tr("No file selected"), tr("You must select a file before reloading!"));
    }
}
*/
