#include "xmldomratereader.h"

#include <QDomDocument>
#include <QFile>

void readUsingDomDocument(const QString &filename) {
    QDomDocument doc;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file))
        return;

    QDomNodeList rates = doc.elementsByTagName("file");
    qulonglong total = 0;
    for (int i = 0; i < rates.size(); i++) {
        QDomNode n = rates.item(i);
        QDomElement hash = n.firstChildElement("hash");
        QDomElement size = n.firstChildElement("size");
        if (hash.isNull() || size.isNull())
            continue;

        if (hash.text()[0].isDigit())
            total += size.text().toULongLong();
    }

    qDebug("total = %llu", total);
}
