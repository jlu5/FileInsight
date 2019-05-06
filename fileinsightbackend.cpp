// Base class for FileInsight backend

#include "fileinsightbackend.h"
#include "fileinsight.h"

FileInsightBackend::FileInsightBackend(QWidget* parent) : parent(parent)
{

}

QString FileInsightBackend::getExtendedInfo(QString) {
    return "";
}

FileInsightBackend::~FileInsightBackend() {

}
