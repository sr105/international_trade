#include "currency.h"

#include <QDebug>

QHash<QString, Currency *> Currency::map;

Currency *Currency::get(const QString name) {
    Currency *c = map.value(name, 0);
    if (!c) {
        c = new Currency(name);
        map[name] = c;
    }
    return c;
}

QString Currency::name() const {
    return _name;
}

float Currency::to(QString currency) const {
    return rates.value(currency, 0.0f);
}
//float from(QString currency);

void Currency::insert(const QString name, float conversion) {
    if (rates.contains(name))
        qDebug() << "WARNING: old value for" << name << "is" << rates[name];
    rates[name] = conversion;
}

void Currency::printMap() {
    foreach (const QString &key, map.keys()) {
        qDebug("%s:", qPrintable(key));
        Currency *c = map[key];
        foreach (const QString &key2, c->rates.keys()) {
            qDebug("\t%s:  %f", qPrintable(key2), c->to(key2));
        }
    }
}
