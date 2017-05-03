// main.cpp: the FileInsight launcher
#include "fileinsight.h"
#include <QApplication>
#include <QCommandLineParser>

QString ICONNAME = "search";

int main(int argc, char *argv[])
{
    // Main function: create a Qt application using the command line arguments given
    QApplication a(argc, argv);

    // Set the version and program name.
    QCoreApplication::setApplicationName("FileInsight");
    QCoreApplication::setApplicationVersion("0.3.0");

    // Handle command line arguments such as --help and an optional filename list
    QCommandLineParser parser;
    parser.setApplicationDescription("FileInsight is a GUI frontend for file type detection.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("files", QCoreApplication::translate("main",
                                 "Optional list of filenames to open."), "[filenames...]");
    parser.process(a);

    // Create an instance of the FileInsight window
    FileInsight w;
    QStringList args = parser.positionalArguments();
    int argcount = args.count();

    // If we're given filenames on the command line, open and spawn a new tab for each.
    for (int i=0; i<argcount; i++) {
        w.openFile(args[i], false);
    }

    // Set an icon for the program. For now we're lazily using the "Search" icon of the
    // current theme...
    w.setWindowIcon(QIcon::fromTheme(ICONNAME));

    // Show the FileInsight window
    w.show();

    return a.exec(); // Run the Qt app and exit with its return code when finished
}
