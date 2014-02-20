#include "xqueryratereader.h"

#include <QtXmlPatterns/QXmlQuery>
#include <QStringList>
#include <QFileInfo>
#include <QDebug>

#include "currency.h"

// Notes: my biggest issue with this design is that the different pieces of
//        each rate are pulled out separately. I would prefer one query that
//        pulled them out in sets.
void readRatesUsingXQuery(const QString filename) {
    QFileInfo fi(filename);
    const QString queryUrl = QString("doc('%1')//rate/%2/string()").arg(fi.absoluteFilePath());

    typedef QPair<QStringList &, QString> QueryPair;
    QList<QueryPair> queries;
    QStringList from, to, conversion;
    queries << QueryPair(from, "from") << QueryPair(to, "to") << QueryPair(conversion, "conversion");
    QXmlQuery query;
    foreach (QueryPair pair, queries) {
        query.setQuery(queryUrl.arg(pair.second));
        query.evaluateTo(&pair.first);
    }
    if (to.size() != from.size() || to.size() != conversion.size())
        return;
    for (int i = 0; i < to.size(); ++i) {
        qDebug() << qPrintable(QString("%1 => %2 : %3")
                               .arg(from.at(i).simplified())
                               .arg(to.at(i).simplified())
                               .arg(conversion.at(i).simplified()));
        bool ok = false;
        double rate = conversion.at(i).simplified().toDouble(&ok);
        if (ok)
            Currency::get(from.at(i).simplified())->insert(to.at(i).simplified(), rate);
    }
}

void readRatesUsingXQuery2(const QString filename) {
    QFileInfo fi(filename);
    const QString queryUrl = QString("doc('%1')//rate/string-join((from, to, conversion)/string(), ',')")
                             .arg(fi.absoluteFilePath());

    QStringList rates;
    QXmlQuery query;
    query.setQuery(queryUrl);
    query.evaluateTo(&rates);
    foreach (const QString &rate, rates) {
        QStringList values = rate.split(',');
        if (values.size() != 3)
            continue;
        qDebug() << qPrintable(QString("%1 => %2 : %3")
                               .arg(values[0].simplified())
                               .arg(values[1].simplified())
                               .arg(values[2].simplified()));
        bool ok = false;
        double conversion = values[2].simplified().toDouble(&ok);
        if (ok)
            Currency::get(values[0].simplified())->insert(values[1].simplified(), conversion);
    }
}
