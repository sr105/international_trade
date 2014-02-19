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

bool Currency::hasRate(QString currency) const
{
    return currency == name() || rates.contains(currency);
}

float Currency::to(QString currency) const {
    if (currency == name())
        return 1.0f;
    return rates.value(currency, 0.0f);
}
//float from(QString currency);

void Currency::insert(const QString name, float conversion) {
    if (rates.contains(name))
        qDebug() << "WARNING: old value for" << name << "is" << rates[name];
    rates[name] = conversion;
}

void Currency::printMap() {
    qDebug("====================");
    foreach (const QString &key, map.keys()) {
        qDebug("%s:", qPrintable(key));
        Currency *c = map[key];
        foreach (const QString &key2, c->rates.keys()) {
            qDebug("\t%s:  %f", qPrintable(key2), c->to(key2));
        }
    }
}

void Currency::fillInTable()
{
    // Goal: fill out the rates map for each currency to have a "to" mapping
    //       to each of the other currencies
    //       - not concerned with efficiency here

    // Plan:
    //       1. Make a set of all currencies.
    //       2. For each member of the set, try to fill in missing rates for the
    //       other currencies. Do this by looking to see if any of them can be
    //       satisfied in 1 degree of separation. If not, skip it, and move on.
    //       3. Repeat step 2 until no more progress is made (either by success
    //       or failure). If no more progress is made and the table is still
    //       incomplete, try using reverse conversions.

    // Note:
    //       I'm nervous that the randomness of the order could affect the outcome.
    //       I'm also nervous that we'll perform a reverse calculation in lieu of
    //       a forward conversion when the forward is available after a previous
    //       reverse.
    bool tableIsComplete = false;
    bool tryReverse = false;
    bool workPerformed = true;
    while (!tableIsComplete) {
        // Only use reverse conversions if all possible forward conversions
        // have been exhausted and the table is still incomplete.
        tryReverse = !workPerformed;
        tableIsComplete = true;
        workPerformed = false;

        QSet<QString> currencies = map.keys().toSet();
        // Check each currency for missing mappings
        foreach (Currency *c, map.values()) {
            // Check each mapping except for identity
            QSet<QString> others = currencies;
            others -= c->name();
            int size = c->rates.size();
            c->fillInCurrency(others, tryReverse);
            tableIsComplete = tableIsComplete && (c->rates.size() == others.size());
            workPerformed = workPerformed || (size != c->rates.size());
        }

        if (!workPerformed && tryReverse)
            break;
    }
}

void Currency::fillInCurrency(QSet<QString> currencies, bool tryReverse) {
    // Make sure this currency has a rate for a direct conversion
    // to each of the currencies passed in.
    foreach (const QString &destination, currencies) {
        // If the mapping already exists, we're good.
        if (hasRate(destination))
            continue;

        // No mapping.
        // For each mapping that we do know, is there a mapping from
        // that currency to the one we want?
        foreach (const QString &key, rates.keys()) {
            Currency *c2 = Currency::get(key);
            if (c2->hasRate(destination)) {
                insert(destination, rates[key] * c2->rates[destination]);
                break;
            }
        }

        if (hasRate(destination) || !tryReverse)
            continue;

        Currency *c3 = Currency::get(destination);
        // See if there's a direct reversal first
        if (c3->hasRate(name())) {
            insert(destination, 1.0f / c3->rates[name()]);
            continue;
        }
        // See if there's an indirect reversal??
    }
}
