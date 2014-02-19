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

    while (xml.readNextStartElement())
        if (xml.name() == "rates")
            processRates();
    // A single readNext() is required here or else we'll get an error.
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
        //qDebug() << "  " << __FUNCTION__ << xml.name();
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

    QString from = getTextElement("from");
    if (from.isEmpty())
        return;
    //qDebug() << "    " << "from" << from;
    Currency *c = Currency::get(from);
    if (!c)
        return;

    QString to = getTextElement("to");
    if (to.isEmpty())
        return;
    //qDebug() << "    " << "to" << to;

    double conversion = 0.0;
    bool ok = false;
    conversion = getTextElement("conversion").toDouble(&ok);
    if (ok)
        c->insert(to, conversion);
    //qDebug() << "    " << "conversion" << conversion;
    // Jump past end of rate
    xml.skipCurrentElement();
}

QString XmlRateReader::getTextElement(const QString name) {
//    qDebug() << "NAME:" << xml.name() << xml.isStartElement();
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
