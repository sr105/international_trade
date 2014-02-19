#ifndef CURRENCY_H
#define CURRENCY_H

#include <QHash>
#include <QString>
#include <QStringList>

class Currency {
public:
    static Currency *get(const QString name);
    QString name() const;

    bool hasRate(QString currency) const;
    double to(QString currency) const;
    //double from(QString currency);

    void insert(const QString name, double conversion);
    static void printMap();
    static void fillInTable();
    static QStringList currencies();

private:
    QString _name;
    QHash<QString, double> rates;
    static QHash<QString, Currency *> map;

    Currency(const QString name) : _name(name) {}
    Currency(const Currency &) {}

    void fillInCurrency(QSet<QString> currencies, bool tryReverse = false);
};

#endif // CURRENCY_H
