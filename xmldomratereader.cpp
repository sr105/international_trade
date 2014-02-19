#include "xmldomratereader.h"

#include <QDomDocument>
#include <QFile>

#include "currency.h"

void readRatesFromXml(const QString &filename) {
    QDomDocument doc;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file))
        return;

    QDomNodeList rates = doc.elementsByTagName("rate");
    for (int i = 0; i < rates.length(); i++) {
        QDomNode n = rates.item(i);
        QDomElement from = n.firstChildElement("from");
        QDomElement to = n.firstChildElement("to");
        QDomElement conversion = n.firstChildElement("conversion");
        if (from.isNull() || to.isNull() || conversion.isNull() ||
            from.text().isEmpty() || to.text().isEmpty() || conversion.text().isEmpty())
            continue;
        double rate = 0.0;
        bool ok = false;
        rate = conversion.text().toDouble(&ok);
        if (ok)
            Currency::get(from.text())->insert(to.text(), rate);
    }
    Currency::fillInTable();
}
