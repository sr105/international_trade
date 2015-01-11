#include <QCoreApplication>
#include <QStringList>
#include <QDebug>
#include <QTime>

#include "xmlratereader.h"
#include "xmldomratereader.h"
#include "xqueryratereader.h"
#include "easyxmlstreamreader.h"

int runTest(void (*fn)(QString), QString filename) {
    int runs = 10;
    QList<int> runTimes;
    QTime t;

    for (int i = 0; i < runs; i++) {
        t.start();
        fn(filename);
        runTimes.append(t.elapsed());
    }
    // sort, average from 1 to runs-2, inclusive
    qSort(runTimes);
    runTimes.removeFirst();
    runTimes.removeLast();
    double total = 0.0;
    foreach(int time, runTimes)
        total += time;
    total /= runs - 2;
    return total;
}

void readUsingXQuery(const QString filename) {
    readUsingXQuery(QFileInfo(filename));
}

void readUsingStreamReader(const QString filename) {
    XmlRateReader reader(filename);
    reader.read();
}

void processFile(EasyXmlStreamReader &reader, void *data) {
    QHash<QString, QString> results = reader.getTextElements(QStringList() << "size" << "hash");
    QString size = results.value("size", "");
    QString hash = results.value("hash", "");
    if (hash.isEmpty() || size.isEmpty())
        return;

    qulonglong &total = *(qulonglong *)data;
    if (hash[0].isDigit())
        total += size.toULongLong();
}

void readUsingEasyStreamReader1(const QString filename) {
    EasyXmlStreamReader reader(filename);
    qulonglong total = 0;
    reader.processElementsByTagName("file", processFile, &total);
}

void readUsingEasyStreamReader2(const QString filename) {
    EasyXmlStreamReader reader(filename);
    qulonglong total = 0;
    reader.processElementsByTagNameHierarchy(QStringList() << "update" << "install" << "file", processFile, &total);
}

void runTests(const QString filename) {
    // Run a test parse using a large XML file and check the
    // performance of the different methods.

    qDebug("%10d ms XmlStreamReader", runTest(&readUsingEasyStreamReader1, filename));
    qDebug("%10d ms XmlStreamReader", runTest(&readUsingEasyStreamReader2, filename));

    qDebug("%10d ms XQuery", runTest(&readUsingXQuery, filename));
    qDebug("%10d ms XmlStreamReader", runTest(&readUsingStreamReader, filename));
    qDebug("%10d ms XDomDocument", runTest(&readUsingDomDocument, filename));

    {
        EasyXmlStreamReader reader("sample.xml");
        reader.printElementTree();
    }
    {
        EasyXmlStreamReader reader2("sample.xml");
        reader2.printStartElementTree();
    }
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
