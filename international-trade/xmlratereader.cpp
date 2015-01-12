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
}

void XmlRateReader::processRate() {
    if (!xml.isStartElement() || xml.name() != "rate")
        return;

    QString from;
    QString to;
    QString conversion;
     while (xml.readNextStartElement()) {
        if (xml.name() == "from")
            from = xml.text().toString();
        else if (xml.name() == "to")
            to = xml.text().toString();
        else if (xml.name() == "conversion")
            conversion = xml.text().toString();
         xml.skipCurrentElement();
     }

    if (!(from.isEmpty() || to.isEmpty() || conversion.isEmpty()))
        Currency::addRate(from, to, conversion);
}

QString XmlRateReader::errorString() {
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}
