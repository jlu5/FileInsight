// main.cpp: the FileInsight launcher
#include "fileinsight.h"
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    // Main function: create a Qt application using the command line arguments given
    QApplication a(argc, argv);

    // Set the version and program name.
    QCoreApplication::setApplicationName("FileInsight");
    QCoreApplication::setApplicationVersion("0.3-beta1");

    // Handle command line arguments such as --help and an optional filename list
    QCommandLineParser parser;
    parser.setApplicationDescription("FileInsight is a GUI frontend for file type detection.");
    parser.addHelpOption();
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

    // Show the FileInsight window
    w.show();

    return a.exec(); // Run the Qt app and exit with its return code when finished
}
