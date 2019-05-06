#include "libmagicbackend.h"


LibmagicBackend::LibmagicBackend(QWidget* parent) : FileInsightBackend(parent)
{
    /* Initialize libmagic by fetching ourselves special cookies from magic_open() - this is
     * similar to fetching a specific instance of a class. Libmagic's API is documented here:
     * https://linux.die.net/man/3/libmagic
     */
    this->magic_cookie = magic_open(MAGIC_CHECK | MAGIC_COMPRESS);

    qDebug() << "libmagic cookie: " << this->magic_cookie;

    // Tell libmagic to load the default file type definition
    magic_load(this->magic_cookie, nullptr);

    // Repeat the above process for a second instance of libmagic, specifically used to find MIME
    // types. (set MAGIC_MIME_TYPE)
    this->magic_cookie_mime = magic_open(MAGIC_CHECK | MAGIC_MIME_TYPE);
    qDebug() << "libmagic cookie_mime: " << this->magic_cookie_mime;
    magic_load(this->magic_cookie_mime, nullptr);

}

// Display libmagic errors from the last call, if any. Returns true if there was an error,
// and false otherwise.
bool LibmagicBackend::getError(magic_t magic_cookie)
{
    QString error_text = magic_error(magic_cookie);
    if (!error_text.isEmpty()) {
        QMessageBox::critical(parent, tr("libmagic error"),
                              tr("The libmagic backend encountered an error: ") + error_text);
        return true;
    }
    return false;
}

QString LibmagicBackend::getMimeType(QString filename) {
    const char* buf = FileInsightUtils::QStringToConstChar(filename);
    QString mimetype = magic_file(this->magic_cookie_mime, buf);
    getError(this->magic_cookie_mime);
    delete buf;
    return mimetype;
}

// Gets file type output from libmagic
QString LibmagicBackend::getExtendedInfo(QString filename)
{
    const char* buf = FileInsightUtils::QStringToConstChar(filename);
    QString magic_output = magic_file(this->magic_cookie, buf);
    qDebug() << "Got libmagic output: " << magic_output;
    getError(this->magic_cookie);
    delete buf;
    return magic_output;
}
