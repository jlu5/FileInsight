#include "tridbackend.h"

TrIDBackend::TrIDBackend(QWidget* parent) : LibmagicBackend(parent)
{
#ifdef Q_OS_WIN
    // On Windows, use an absolute path for TrID (in our thirdparty/ folder)
    QString appdir = QCoreApplication::applicationDirPath();
    this->trid_command = appdir + "/thirdparty/trid";
#else
    // Otherwise, just look for TrID in the path.
    this->trid_command = "trid";
#endif
}

// Gets extended file info using the TrID command line program, by running it in a subprocess.
QString TrIDBackend::getExtendedInfo(QString filename)
{
    QString data;

    // TrID's command line argument handling isn't great (it breaks on filenames with
    // spaces or hyphens in it), so we use its interactive read-from-STDIN mode instead.
    this->trid_subprocess.start(this->trid_command, QStringList() << "-@");
    const char* filenamebuf = FileInsightUtils::QStringToConstChar(filename);
    this->trid_subprocess.write(filenamebuf);
    delete filenamebuf;
    this->trid_subprocess.waitForBytesWritten();
    this->trid_subprocess.closeWriteChannel();

    QByteArray result;
    // Grab all of the subprocess' text output using a loop.
    while (this->trid_subprocess.waitForReadyRead())
    {
        result += this->trid_subprocess.readAll();
    }
    data = result.data();
    // Trim any leading or trailing whitespace.
    data = data.trimmed();

    if (data.isEmpty())
    {
        // The TrID process failed to start or otherwise returned no data.
        // Raise an error.
        QMessageBox::critical(this->parent, tr("Failed to start TrID"),
                             tr("An error occurred while retrieving data from the TrID backend. "
                                "Check that TrID is installed and in your PATH!"));
    }

    return data;
}
