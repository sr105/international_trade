#include <QCoreApplication>
#include <QStringList>
#include <QDebug>
#include <QTime>

#include "xmldomratereader.h"
#include "xqueryratereader.h"
#include "easyxmlstreamreader.h"

int runTest(void (*fn)(QString), QString filename) {
    int runs = 40;
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

void processFile2(EasyXmlStreamReader &reader, void *data) {
    QHash<QString, QString> results = reader.getTextElements2(QStringList() << "size" << "hash");
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
    //    qDebug("total = %llu", total);
}

void readUsingEasyStreamReader2(const QString filename) {
    EasyXmlStreamReader reader(filename);
    qulonglong total = 0;
    reader.processElementsByTagNameHierarchy(QStringList() << "update" << "install" << "file", processFile, &total);
    //    qDebug("total = %llu", total);
}

void readUsingEasyStreamReader2_2(const QString filename) {
    EasyXmlStreamReader reader(filename);
    qulonglong total = 0;
    reader.processElementsByTagNameHierarchy(QStringList() << "update" << "install" << "file", processFile2, &total);
    //    qDebug("total = %llu", total);
}

void runTests(const QString filename) {
    // Run a test parse using a large XML file and check the
    // performance of the different methods.

    QFile f(filename);
    f.readAll();
    f.close();

    qDebug("%10d ms XmlStreamReader::processElementsByTagNameHierarchy2", runTest(&readUsingEasyStreamReader2_2, filename));
    qDebug("%10d ms XmlStreamReader::processElementsByTagNameHierarchy", runTest(&readUsingEasyStreamReader2, filename));
    qDebug("%10d ms XmlStreamReader::processElementsByTagName", runTest(&readUsingEasyStreamReader1, filename));

    qDebug("%10d ms XDomDocument", runTest(&readUsingDomDocument, filename));
    qDebug("%10d ms XQuery", runTest(&readUsingXQuery, filename));
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
