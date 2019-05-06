#include "fileinsightutils.h"

namespace FileInsightUtils
{
    // Converts a QString to const char*. The callee is responsibe for freeing the resulting buffer.
    const char* QStringToConstChar(QString text) {
        QByteArray bytes = text.toUtf8();
        // Explicitly make a copy of the byte array data, so it can't be destroyed and
        // and corrupt by garbage collection.
        char* data = new char[bytes.size() + 1];
        std::strcpy(data, bytes.constData());
        return data;
    }

    /* Fetch an icon based on the MIME type string given. This uses freedesktop.org
     * comptible icon themes, which are native to Linux but can be ported to Windows
     * by bundling a theme.
     * On Linux, this will use whichever icon theme the desktop is currently using.
     * On Windows, a copy of the (KDE) Oxygen icon theme is bundled so that the theme
     * can be loaded.
     */
    QIcon getIcon(QString mimetype, QString filename) {
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
                    QFileIconProvider iconprovider;
                    icon = QIcon::fromTheme("unknown", iconprovider.icon(QFileIconProvider::File));
                }
            }
        }
        return icon;
    }
}
