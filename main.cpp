// main.cpp: the FileInsight launcher
#include "fileinsight.h"
#include <QApplication>
#include <QCommandLineParser>

static QString ICON_NAME = "search";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("FileInsight");
    QCoreApplication::setApplicationVersion("0.3.1");

    // Handle command line arguments such as --help and an optional filename list
    QCommandLineParser parser;
    parser.setApplicationDescription("FileInsight is a GUI frontend for file type detection.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("files", QCoreApplication::translate("main",
                                 "Optional list of filenames to open."), "[filenames...]");
    parser.process(a);

    FileInsight w;
    QStringList args = parser.positionalArguments();
    int argcount = args.count();

    // If we're given filenames on the command line, open and spawn a new tab for each.
    for (int i=0; i<argcount; i++) {
        w.openFile(args[i], false);
    }

    // Set an icon for the program. For now we're lazily using the "Search" icon of the
    // current theme...
    w.setWindowIcon(QIcon::fromTheme(ICON_NAME));
    w.show();
    return a.exec();
}
