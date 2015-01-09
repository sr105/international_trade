#include "xmlratereader.h"

#include <QFile>
#include <QDebug>

XmlRateReader::XmlRateReader(const QString filename) :
    _filename(filename),
    _total(0)
{}

void XmlRateReader::read() {
    QFile xmlFile(_filename);
    xmlFile.open(QIODevice::ReadOnly);
    _xml.setDevice(&xmlFile);

    processAllWithMethod(QStringList() << "update" << "install" << "file",
                          &XmlRateReader::processFile);

    // A single readNext() is required here or else we'll get an error.
    _xml.readNext();
    if (_xml.hasError()) {
        _xml.raiseError();
        qDebug() << errorString();
    }

    qDebug("total = %llu", _total);
}

void XmlRateReader::processAllWithMethod(QString name, member_fn_type method) {
    processAllWithMethod(QStringList() << name, method);
}

void XmlRateReader::processAllWithMethod(QStringList names, member_fn_type method)
{
    if (names.isEmpty()) {
        (this->*method)();
        return;
    }

    QString name = names.first();
    names.removeFirst();
    while (_xml.readNextStartElement()) {
        if (_xml.name() == name)
            processAllWithMethod(names, method);
        else
            _xml.skipCurrentElement();
    }
    _xml.skipCurrentElement();
}

void XmlRateReader::processFile() {
    if (!_xml.isStartElement() || _xml.name() != "file")
        return;

    QString size = getTextElement("size");
    QString hash = getTextElement("hash");
    if (hash.isEmpty() || size.isEmpty())
        return;

    if (hash[0].isDigit())
        _total += size.toULongLong();

    // Jump past end of rate
    _xml.skipCurrentElement();
}

QString XmlRateReader::getTextElement(const QString name) {
    while (_xml.readNextStartElement()) {
        if (_xml.name() == name)
            return _xml.readElementText();
        _xml.skipCurrentElement();
    }
    return QString();
}

QString XmlRateReader::errorString() {
    return QObject::tr("%1\nLine %2, column %3")
            .arg(_xml.errorString())
            .arg(_xml.lineNumber())
            .arg(_xml.columnNumber());
}
