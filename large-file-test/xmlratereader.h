#ifndef XMLRATEREADER_H
#define XMLRATEREADER_H

#include <QXmlStreamReader>
#include <QString>
#include <QStringList>

class XmlRateReader
{
public:
    XmlRateReader(const QString filename);

    void read();

    typedef void (*fn_type)();
    typedef void (XmlRateReader::*member_fn_type)();

private:
    void printCurrent(QString extra = QString());

    void printStartElementTree();

    void processAllNamedElementsWithMethod(QString name, member_fn_type method);

    void processAllWithMethod(QString name, member_fn_type method);
    void processAllWithMethod(QStringList names, member_fn_type method);

    void processFile();
    QHash<QString, QString> getTextElements(QStringList names);
    QString getTextElement(const QString name, QString parentName = QString());
    QString errorString();

    QString _filename;
    QXmlStreamReader _xml;
    qulonglong _total;
};

#endif // XMLRATEREADER_H
