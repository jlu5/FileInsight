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
    // On Windows, set the path and name of our bundled icon theme
    QIcon::setThemeName("oxygen");
    QIcon::setThemeSearchPaths(QStringList() << appdir + "/icons");
#endif

    magicbackend = new LibmagicBackend(this);
    tridbackend = new TrIDBackend(this);
    qmimebackend = new QMimeDatabaseBackend(this);

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

// Returns a pointer to the currently selected backend
FileInsightBackend* FileInsight::getBackend() const {
    if (ui->backend_trid->isChecked()) {
        return tridbackend;
    } else if (ui->backend_qt->isChecked()) {
        return qmimebackend;
    } else {
        // Use libmagic as default
        return magicbackend;
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

    // Get descriptive info from our backend
    FileInsightBackend* backend = this->getBackend();
    QString ext_info = backend->getExtendedInfo(filename);
    currentTab->ui->output->setPlainText(ext_info);

    // Get the MIME type and use it to fetch the icon
    QString mimetype = backend->getMimeType(filename);
    currentTab->ui->mimeOutput->setPlainText(mimetype);

    QIcon icon = FileInsightUtils::getIcon(mimetype, filename);

    // Display the icon based on the smaller of either 128x128 or the actual icon size
    // TODO: consider scaling the icon size based on the window size as well
    QSize iconSize = icon.actualSize(QSize(128, 128));
    currentTab->ui->iconDisplay->setPixmap(icon.pixmap(iconSize));
    currentTab->ui->iconDisplay->setMinimumSize(iconSize);
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
