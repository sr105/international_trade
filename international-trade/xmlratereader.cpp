#include "xmlratereader.h"

#include <QFile>
#include <QDebug>

#include "currency.h"

XmlRateReader::XmlRateReader(const QString filename) :
    _filename(filename)
{}

void XmlRateReader::read() {
    QFile xmlFile(_filename);
    xmlFile.open(QIODevice::ReadOnly);
    xml.setDevice(&xmlFile);

    if (xml.readNextStartElement() && xml.name() == "rates")
       processRates();

    // readNextStartElement() leaves the stream in
    // an invalid state at the end. A single readNext()
    // will advance us to EndDocument.
    if (xml.tokenType() == QXmlStreamReader::Invalid)
        xml.readNext();

    if (xml.hasError()) {
        xml.raiseError();
        qDebug() << errorString();
    }
}

void XmlRateReader::processRates() {
    if (!xml.isStartElement() || xml.name() != "rates")
        return;
    while (xml.readNextStartElement()) {
        if (xml.name() == "rate")
            processRate();
        else
            xml.skipCurrentElement();
    }
    xml.skipCurrentElement();
}

void XmlRateReader::processRate() {
    if (!xml.isStartElement() || xml.name() != "rate")
        return;

    Currency::addRate(getTextElement("from"), getTextElement("to"), getTextElement("conversion"));
    // Jump past end of rate
    xml.skipCurrentElement();
}

QString XmlRateReader::getTextElement(const QString name) {
    while (xml.readNextStartElement()) {
        if (xml.name() == name)
            return xml.readElementText();
        xml.skipCurrentElement();
    }
    return QString();
}

QString XmlRateReader::errorString() {
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}
