#include "xqueryratereader.h"

#include <QtXmlPatterns/QXmlQuery>
#include <QStringList>
#include <QFileInfo>
#include <QDebug>

void readUsingXQuery(const QFileInfo file) {
    const QString queryUrl = QString("doc('%1')//file/string-join((hash, size)/string(), ',')")
                             .arg(file.absoluteFilePath());

    QStringList rates;
    QXmlQuery query;
    query.setQuery(queryUrl);
    query.evaluateTo(&rates);
    qulonglong total = 0;
    foreach (const QString &rate, rates) {
        QStringList values = rate.split(',');
        if (values.size() != 2 || values[0].isEmpty() || values[1].isEmpty())
            continue;
        if (values[0][0].isDigit())
            total += values[1].toULongLong();
    }
//    qDebug("total = %llu", total);
}
