// fileinsight.cpp: The main FileInsight shell.
#include "fileinsight.h"
#include "ui_fileinsight.h"
#include "fileinsightsubdialog.h"
#include "ui_fileinsightsubdialog.h"

// Main window class: this is derived from QMainWindow but implements custom routines
// so the program is useful.
FileInsight::FileInsight(QWidget *parent) : QMainWindow(parent), ui(new Ui::FileInsight) {
    ui->setupUi(this);
    setAcceptDrops(true);

#ifdef Q_OS_WIN
    // On Windows, use an absolute path for TrID (in our thirdparty/ folder)
    QString appdir = QCoreApplication::applicationDirPath();
    this->trid_command = appdir + "/thirdparty/trid";

    // Set the path and name of our bundled icon theme
    QIcon::setThemeName("oxygen");
    QIcon::setThemeSearchPaths(QStringList() << appdir + "/icons");
#else
    // Otherwise, just look for TrID in the path.
    this->trid_command = "trid";
#endif

    /* Initialize libmagic by fetching ourselves special cookies from magic_open() - this is
     * similar to fetching a specific instance of a class. Libmagic's API is documented here:
     * https://linux.die.net/man/3/libmagic
     */
    // libmagic flags go here (e.g. MAGIC_CHECK to verify that libmagic's data files are loaded)
    this->magic_cookie = magic_open(MAGIC_CHECK | MAGIC_COMPRESS);

    qDebug() << "libmagic cookie: " << this->magic_cookie;

    // Tell libmagic to load the default file type definition
    magic_load(this->magic_cookie, nullptr);

    // Repeat the above process for a second instance of libmagic, specifically used to find MIME
    // types. (set MAGIC_MIME_TYPE)
    this->magic_cookie_mime = magic_open(MAGIC_CHECK | MAGIC_MIME_TYPE);
    qDebug() << "libmagic cookie_mime: " << this->magic_cookie_mime;
    magic_load(this->magic_cookie_mime, nullptr);

    // Initialize an empty new tab.
    this->newTab(true);
}

FileInsightSubdialog * FileInsight::newTab(bool starting)
{
    int newIndex = ui->tabWidget->addTab(new FileInsightSubdialog(this), tr("New Tab"));
    ui->tabWidget->setCurrentIndex(newIndex);

    if (!starting) {
        // If closing tabs was disabled, enable it again.
        ui->tabWidget->setTabsClosable(true);
    }
    return static_cast<FileInsightSubdialog *>(ui->tabWidget->widget(newIndex));
}

// Destructor for the FileInsight class:
FileInsight::~FileInsight()
{
    // Delete the temporary ui variable that we assigned in the constructor.
    delete ui;
}

void FileInsight::chooseFile()
{
    // This implements file selection via Qt's built-in file dialog
    QString filename = QFileDialog::getOpenFileName(this, tr("Select File"), QString(),
            tr("All Files (*)"));
    qDebug() << "Selected file: " << filename;

    if (!filename.isEmpty()) {
        // Open the file if one is selected: if the user clicks cancel on the file
        // dialog, this if statement makes sure that nothing happens.
        this->openFile(filename);
    }
}

// Display libmagic errors from the last call, if any. Returns true if there was an error,
// and false otherwise.
bool FileInsight::getMagicError(magic_t magic_cookie)
{
    QString error_text = magic_error(magic_cookie);
    if (!error_text.isEmpty()) {
        QMessageBox::critical(this, tr("libmagic error"),
                              tr("The libmagic backend encountered an error: ") + error_text);
        return true;
    }
    return false;
}

// Gets file type output from libmagic
QString FileInsight::getMagicInfo(QString filename)
{
    QString magic_output = magic_file(this->magic_cookie, this->QStringToConstChar(filename));
    qDebug() << "Got libmagic output: " << magic_output;
    getMagicError(this->magic_cookie);
    return magic_output;
}


// Gets extended file info using the TrID command line program, by running it in a subprocess.
QString FileInsight::getTridInfo(QString filename)
{
    QString data;

    // TrID's command line argument handling isn't great (it breaks on filenames with
    // spaces or hyphens in it), so we use its interactive read-from-STDIN mode instead.
    this->trid_subprocess.start(this->trid_command, QStringList() << "-@");
    this->trid_subprocess.write(this->QStringToConstChar(filename));
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
        QMessageBox::critical(this, tr("Failed to start TrID"),
                             tr("An error occurred while retrieving data from the TrID backend. "
                                "Check that TrID is installed and in your PATH!"));
    }

    return data;
}

/* Fetch the MIME type of the selected file, using either Qt 5 or libmagic,
 * depending on which one is selected. See https://en.wikipedia.org/wiki/Media_type#Naming
 * for the relevant format.
 */
QString FileInsight::getMimeType(QString filename)
{
    QString mimetype;
    FileInsightBackend backend = this->getBackend();

    if (backend == BACKEND_QT || backend == BACKEND_QT_FILEONLY) {
        // Qt5 / QMimeDatabase backend
        QMimeDatabase mimedb;

        QMimeType mimeobj;

        // If we're using the option to look at data only, pass that on to
        // mimeTypeForFile()
        if (backend == BACKEND_QT_FILEONLY)
        {
            mimeobj = mimedb.mimeTypeForFile(filename, QMimeDatabase::MatchContent);
        } else {
            mimeobj = mimedb.mimeTypeForFile(filename);
        }
        mimetype = mimeobj.name();
    } else {
        // libmagic MIME type backend
        mimetype = magic_file(this->magic_cookie_mime, this->QStringToConstChar(filename));
        getMagicError(this->magic_cookie_mime);
    }
    return mimetype;
}

const char * FileInsight::QStringToConstChar(QString text) {
    // Convert QStrings to const char *, so that it can be used by libmagic, etc.
    // Source: http://doc.qt.io/qt-5/qbytearray.html#data
    QByteArray bytes = text.toUtf8();
    // Explicitly make a copy of the byte array data, so it can't be destroyed and
    // and corrupt by garbage collection.
    char *data = new char[bytes.size() + 1];
    std::strcpy(data, bytes.constData());
    qDebug() << "QStringToConstChar output: " << data;
    return data;
}

FileInsightBackend FileInsight::getBackend() {
    // Returns the backend currently in use. These integer values are
    // defined in constants.h.
    if (ui->backend_trid->isChecked()) {
        return BACKEND_TRID;
    } else if (ui->backend_qt->isChecked()) {
        return BACKEND_QT;
    } else if (ui->backend_qt_fileonly->isChecked()) {
        return BACKEND_QT_FILEONLY;
    } else {
        // Use libmagic as default
        return BACKEND_MAGIC;
    }
}

FileInsightSubdialog* FileInsight::getCurrentTab() {
    // Return the current tab, explicitly casted to the FileInsightSubDialog type.
    return static_cast<FileInsightSubdialog *>(ui->tabWidget->currentWidget());
}

void FileInsight::openFile(QString filename, bool overwrite)
{
    FileInsightSubdialog *currentTab = this->getCurrentTab();

    if (!currentTab->filename.isEmpty() && !overwrite) {
        // If the current tab already has a file loaded and overwriting isn't enabled,
        // automatically create a new tab.
        currentTab = this->newTab();
    }
    currentTab->ui->filenameOutput->setPlainText(filename);
    currentTab->filename = filename;

    QFileInfo fi(filename);
    if (!fi.exists()) {
        QMessageBox::critical(this, tr("Error reading file"), "Can't find \"" + filename + "\"");
        return;
    }

    // Update the tab title to be the file name, stripped of its full path
    // (e.g. C:\test.txt becomes "test.txt")
    QString stripped_filename = fi.fileName();
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), stripped_filename);

    QString ext_info;
    FileInsightBackend backend = this->getBackend();

    // Fill in extended info: use the TrID or libmagic backends (whichever is selected)
    if (backend == BACKEND_TRID)
    {
        ext_info = this->getTridInfo(filename);
    } else if (backend == BACKEND_MAGIC) {
        ext_info = this->getMagicInfo(filename);
    }
    currentTab->ui->output->setPlainText(ext_info);

    // Get the MIME type and use it to fetch the icon
    QString mimetype = this->getMimeType(filename);
    currentTab->ui->mimeOutput->setPlainText(mimetype);

    QIcon icon = this->getIcon(mimetype, filename);

    // Display the icon based on the smaller of either 128x128 or the actual icon size
    // TODO: consider scaling the icon size based on the window size as well
    QSize iconSize = icon.actualSize(QSize(128, 128));
    currentTab->ui->iconDisplay->setPixmap(icon.pixmap(iconSize));
    currentTab->ui->iconDisplay->setMinimumSize(iconSize);
}

QIcon FileInsight::getIcon(QString mimetype, QString filename) {
    /* Fetch an icon based on the MIME type string given. This uses freedesktop.org
     * comptible icon themes, which are native to Linux but can be ported to Windows
     * by bundling a theme.
     * On Linux, this will use whichever icon theme the desktop is currently using.
     * On Windows, a copy of the (KDE) Oxygen icon theme is bundled so that the theme
     * can be loaded.
     */
    QIcon icon;
    QString iconname = mimetype;

    /* Generic MIME types are created by taking first part of the type (e.g. "video" from "video/ogg")
     * and adding "-x-generic" to it. So, the generic type for video/ogg would be video-x-generic.
     */
    QString generic_type = mimetype.split("/")[0] + "-x-generic";

    // Per the freedesktop specification, replace any slashes in the MIME type with a hyphen.
    // This is based off QMimeType::iconTheme() as seen here:
    // https://github.com/qt/qtbase/blob/6bceb4a8/src/corelib/mimetypes/qmimetype.cpp#L299-L316
    int slashlocation = iconname.indexOf("/");
    if (slashlocation != -1) {
        iconname.replace(slashlocation, 1, "-");
    }

    qDebug() << "Looking up icon for MIME type " << mimetype << " (generic name: " << generic_type << ")";

    if (QIcon::hasThemeIcon(iconname)) {
        /* If the icon theme we're using has an icon for the MIME type we're looking for,
         * use that. */
        icon = QIcon::fromTheme(iconname);
    } else {
        /* Otherwise, fall back to the following in order:
         * 1) The icon for the generic type (e.g. a "video" icon for .mp4 files)
         * 2) The "unknown file type" icon in the icon theme used.
         * 3) IF on Windows, the shell file type icon for the given file.
         * 4) Qt's (small and out of place) generic file icon.
         */
        if (QIcon::hasThemeIcon(generic_type)) {
            icon = QIcon::fromTheme(generic_type);
        } else {
            #ifdef Q_OS_WIN
                // Fetch the Windows icon using shell32.SHGetFileInfo:
                // https://msdn.microsoft.com/en-us/library/windows/desktop/bb762179(v=vs.85).aspx
                // TODO: support the extra large icon sizes on Windows Vista and above.
                SHFILEINFOW shellfileinfo;
                SHGetFileInfo((LPCTSTR) filename.utf16(),
                              FILE_ATTRIBUTE_NORMAL, &shellfileinfo, sizeof(shellfileinfo),
                              SHGFI_ICON | SHGFI_LARGEICON | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
                // Then, convert it into a QIcon.
                QPixmap pixmap = QtWin::fromHICON(shellfileinfo.hIcon);
                icon = QIcon(pixmap);
            #endif

            if (icon.isNull()) {
                icon = QIcon::fromTheme("unknown", this->iconprovider.icon(QFileIconProvider::File));
            }
        }
    }
    return icon;
}

// Implement the Quit action in the File menu.
void FileInsight::on_actionQuit_triggered()
{
    qApp->quit();
}

void FileInsight::on_selectFileButton_clicked()
{
    this->chooseFile();
}

void FileInsight::on_actionSelect_triggered()
{
    this->chooseFile();
}

// Reload the file in the current tab, if one has been selected.
void FileInsight::on_reloadButton_clicked()
{
    FileInsightSubdialog *currentTab = this->getCurrentTab();
    if (!currentTab->filename.isEmpty()) {
        this->openFile(currentTab->filename);
    } else {
        QMessageBox::critical(this, tr("No file selected"), tr("You must select a file before reloading!"));
    }
}

// This implements the close button on the tab widget, when multiple are open
void FileInsight::on_tabWidget_tabCloseRequested(int index)
{
    int count = ui->tabWidget->count();
    if (count >= 2) {
        ui->tabWidget->removeTab(index);

        // If there were only 2 tabs (1 now since the current was closed),
        // disable the close button on the last tab.
        if (count == 2) {
            ui->tabWidget->setTabsClosable(false);
        }
    } else {
        qDebug() << "Refusing to remove the last tab!";
    }
}

void FileInsight::on_addTabButton_clicked()
{
    this->newTab();
}

// This implements the About option in the File menu.
void FileInsight::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About FileInsight"),
                       tr("This is FileInsight %1").arg(QCoreApplication::applicationVersion()));
}

// This implements the About Qt option in the File menu.
void FileInsight::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

// Handle attempts to drag & drop URLs into the window
void FileInsight::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

// Drop handler: it takes the file paths given and process each one individually.
void FileInsight::dropEvent(QDropEvent *event)
{
    QUrl url;
    QString localUrl;
    foreach (url, event->mimeData()->urls()) {
        localUrl = url.toLocalFile();

        // Create a new tab to open each file that was dropped.
        this->openFile(localUrl, false);
    }
}
