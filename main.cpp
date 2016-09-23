#include "fileinsight.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // Main function: create a Qt application using the command line arguments given
    QApplication a(argc, argv);

    // Create an instance of the FileInsight window, and display it
    FileInsight w;
    w.show();

    return a.exec(); // Run the Qt app and exit with its return code
}
