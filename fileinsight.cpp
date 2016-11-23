// fileinsight.cpp: The main FileInsight shell.
#include "fileinsight.h"
#include "ui_fileinsight.h"
#include "fileinsightsubdialog.h"
#include "ui_fileinsightsubdialog.h"

// Main window class: this is derived from QMainWindow but implements custom routines
// so the program is useful.
FileInsight::FileInsight(QWidget *parent) : QMainWindow(parent), ui(new Ui::FileInsight) {
    // Set up the UI by creating an instance of our UI class and initializing it
    ui->setupUi(this);

    // Enable drag and drop
    setAcceptDrops(true);

#ifdef Q_OS_WIN
    // On Windows, use an absolute path for TrID (in our thirdparty/ folder), so that
    // the program can be found.
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

    // Tell libmagic to load the default file type definitions by passing NULL as filename argument
    magic_load(this->magic_cookie, NULL);

    // Repeat the above process for a second instance of libmagic, specifically used to find MIME
    // types. (set MAGIC_MIME_TYPE)
    this->magic_cookie_mime = magic_open(MAGIC_CHECK | MAGIC_MIME_TYPE);
    magic_load(this->magic_cookie_mime, NULL);

    // Initialize an empty new tab.
    this->newTab(true);
}

FileInsightSubdialog * FileInsight::newTab(bool starting)
{
    // Add the new tab.
    int newIndex = ui->tabWidget->addTab(new FileInsightSubdialog(this), tr("New Tab"));

    // Set the focus to the new tab as it is created.
    ui->tabWidget->setCurrentIndex(newIndex);

    if (!starting) {
        // If closing tabs was disabled, enable it again.
        ui->tabWidget->setTabsClosable(true);
    }

    // Return the newly created widget/tab, and explicitly cast it to the
    // FileInsightSubdialog type so we can access its UI elements.
    return (FileInsightSubdialog *) ui->tabWidget->widget(newIndex);
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
    std::cout << "Selected file: " << filename.toStdString() << std::endl;

    if (!filename.isEmpty()) {
        // Open the file if one is selected: if the user clicks cancel on the file
        // dialog, this if statement makes sure that nothing happens.
        this->openFile(filename);
    }
}

QString FileInsight::getMagicInfo(QString filename)
{
    // Tell libmagic to open the filename - it will return a string describing the file.
    QString magic_output = magic_file(this->magic_cookie, this->QStringToConstChar(filename));
    return magic_output;
}


QString FileInsight::getTridInfo(QString filename)
{
    // This method gets extended file info using the TrID command line program,
    // by running it in a subprocess.
    QString data;

    // TrID's command line argument handling isn't great (it breaks on filenames with
    // spaces or hyphens in it), so we use its interactive read-from-STDIN mode instead.
    this->trid_subprocess.start(this->trid_command, QStringList() << "-@");
    this->trid_subprocess.write(this->QStringToConstChar(filename));
    // FIXME: sometimes this breaks (TrID says it can't find the file given).
    // Could this be a race condition?
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

QString FileInsight::getMimeType(QString filename)
{
    QString mimetype;
    int backend = this->getBackend();

    /* Fetch the MIME type of the selected file, using either Qt 5 or libmagic,
     * depending on which one is selected. See https://en.wikipedia.org/wiki/Media_type#Naming
     * for the relevant format.
     */
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
    }
    return mimetype;
}

const char * FileInsight::QStringToConstChar(QString text) {
    /* Convert QString's to const char *, so that it can be used by libmagic, etc.
     * Note: the QByteArray created by toUtf8() must be kept as a variable, or
     * the pointer returned by constData() will be invalidated.
     */
    QByteArray bytes = text.toUtf8();
    return bytes.constData();
}

int FileInsight::getBackend() {
    // Returns the backend currently in use. These integer values are
    // defined in constants.h.
    if (ui->backend_magic->isChecked()) {
        return BACKEND_MAGIC;
    } else if (ui->backend_trid->isChecked()) {
        return BACKEND_TRID;
    } else if (ui->backend_qt->isChecked()) {
        return BACKEND_QT;
    } else if (ui->backend_qt_fileonly->isChecked()) {
        return BACKEND_QT_FILEONLY;
    } else {
        return -1;
    }
}

FileInsightSubdialog* FileInsight::getCurrentTab() {
    // Return the current tab, explicitly casted to the FileInsightSubDialog type.
    return (FileInsightSubdialog *) ui->tabWidget->currentWidget();
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

    // Save the filename in the tab object, so we know whether it has a file attached
    // in the future.
    currentTab->filename = filename;

    // Check to make sure the file that was given exists
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
    int backend = this->getBackend();

    // Fill in extended info: use the TrID or libmagic backends (whichever is selected)
    if (backend == BACKEND_TRID)
    {
        ext_info = this->getTridInfo(filename);
    } else if (backend == BACKEND_MAGIC) {
        ext_info = this->getMagicInfo(filename);
    }
    // Write the extended output into the text box in the tab.
    currentTab->ui->output->setPlainText(ext_info);

    // Get the MIME type and use it to fetch the icon
    QString mimetype = this->getMimeType(filename);
    currentTab->ui->mimeOutput->setPlainText(mimetype);

    QIcon icon = this->getIcon(mimetype);

    // TODO: consider scaling the icon display based on the window size
    currentTab->ui->iconDisplay->setPixmap(icon.pixmap(128,128));
}

QIcon FileInsight::getIcon(QString mimetype) {
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

    std::cout << "Looking up icon for MIME type " << mimetype.toStdString() <<
                 " (generic name: " << generic_type.toStdString() << ")" << std::endl;

    if (QIcon::hasThemeIcon(iconname)) {
        /* If the icon theme we're using has an icon for the MIME type we're looking for,
         * use that. */
        icon = QIcon::fromTheme(iconname);
    } else {
        /* Otherwise, fall back to the following in order:
         * 1) The icon for the generic type (e.g. a "video" icon for .mp4 files)
         * 2) The "unknown file type" icon in the icon theme used.
         * 3) Qt's (small and out of place) generic file icon.
         */
        if (QIcon::hasThemeIcon(generic_type)) {
            icon = QIcon::fromTheme(generic_type);
        } else {
            icon = QIcon::fromTheme("unknown", this->iconprovider.icon(QFileIconProvider::File));
        }
    }
    return icon;
}

// These implement slots (event handlers), such as the Quit menu option, select file
// button, etc.

void FileInsight::on_actionQuit_triggered()
{
    // Implement the Quit action in the File menu. This calls quit() on the global "qApp"
    // pointer, which refers to the current running QApplication instance
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

void FileInsight::on_reloadButton_clicked()
{
    // Reload the file in the current tab, if one has been selected.
    FileInsightSubdialog *currentTab = this->getCurrentTab();
    if (!currentTab->filename.isEmpty()) {
        this->openFile(currentTab->filename);
    } else {
        QMessageBox::critical(this, tr("No file selected"), tr("You must select a file before reloading!"));
    }
}

void FileInsight::on_tabWidget_tabCloseRequested(int index)
{
    // This implements the close button on the tab widget, when multiple
    // tabs are open.
    int count = ui->tabWidget->count();
    if (count >= 2) {
        ui->tabWidget->removeTab(index);

        // If there were only 2 tabs (1 now since the current was closed),
        // disable the close button on the last tab.
        if (count == 2) {
            ui->tabWidget->setTabsClosable(false);
        }
    } else {
        std::cout << "Refusing to remove the last tab!" << std::endl;
    }
}

void FileInsight::on_addTabButton_clicked()
{
    this->newTab();
}

void FileInsight::on_actionAbout_triggered()
{
    // This implements the About option in the File menu.
    QMessageBox::about(this, tr("About FileInsight"),
                       tr("This is FileInsight %1 by J. Lu").arg(QCoreApplication::applicationVersion()));
}

void FileInsight::on_actionAbout_Qt_triggered()
{
    // "About Qt" is implemented in the framework itself; we just have to call it
    // within our program.
    QMessageBox::aboutQt(this);
}

void FileInsight::dragEnterEvent(QDragEnterEvent *event)
{
    // Handle attempts to drag & drop things into the window: only let it succeed if we're
    // given URLs (in this case, paths to files we can open).
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void FileInsight::dropEvent(QDropEvent *event)
{
    // Drop handler: it takes the file paths given and process each one individually.

    // Partly based off https://wiki.qt.io/Drag_and_Drop_of_files
    QUrl url;
    QString localUrl;
    foreach (url, event->mimeData()->urls()) {
        localUrl = url.toLocalFile();

        // Create a new tab to open each file that was dropped.
        this->openFile(localUrl, false);
    }
}
