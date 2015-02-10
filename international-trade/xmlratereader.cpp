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

// Uncomment this to see another way to read element
// text. It returns the concatenation of the text
// from all child elements.
//#define USE_READ_ELEMENT_TEXT 1

void XmlRateReader::processRate() {
    if (!xml.isStartElement() || xml.name() != "rate")
        return;

    QString from;
    QString to;
    QString conversion;
    while (xml.readNextStartElement()) {
        if (xml.name() == "from")
            from = readNextText();
        else if (xml.name() == "to")
            to = readNextText();
        else if (xml.name() == "conversion")
            conversion = readNextText();
#ifndef USE_READ_ELEMENT_TEXT
        xml.skipCurrentElement();
#endif
    }

    if (!(from.isEmpty() || to.isEmpty() || conversion.isEmpty()))
        Currency::addRate(from, to, conversion);
}

QString XmlRateReader::readNextText() {
#ifndef USE_READ_ELEMENT_TEXT
    xml.readNext();
    return xml.text().toString();
#else
    return xml.readElementText();
#endif
}

QString XmlRateReader::errorString() {
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}
