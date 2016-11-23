// fileinsightsubdialog.cpp: Subwidget used inside tabs, with text boxes for displaying information.
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
