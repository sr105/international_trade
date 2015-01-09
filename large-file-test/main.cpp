#include <QCoreApplication>
#include <QStringList>
#include <QDebug>

#include "xmlratereader.h"
#include "xmldomratereader.h"
#include "xqueryratereader.h"

void runTests(const QString filename) {
    // Run a test parse using a large XML file and check the
    // performance of the different methods.

    readUsingXQuery(filename);

    XmlRateReader reader(filename);
    reader.read();

    readUsingDomDocument(filename);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();
    switch(args.size()) {
    default:
        qDebug() << "Usage:" << qPrintable(args[0]) << "test.xml";
        return 1;
    case 2:
        break;
    }

    runTests(args[1]);

    return 0;
}
