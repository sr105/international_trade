#ifndef CURRENCY_H
#define CURRENCY_H

#include <QHash>
#include <QString>

class Currency {
public:
    static Currency *get(const QString name);
    QString name() const;

    bool hasRate(QString currency) const;
    float to(QString currency) const;
    //float from(QString currency);

    void insert(const QString name, float conversion);
    static void printMap();

    static void fillInTable();
private:
    QString _name;
    QHash<QString, float> rates;
    static QHash<QString, Currency *> map;

    Currency(const QString name) : _name(name) {}
    Currency(const Currency &) {}

    void fillInCurrency(QSet<QString> currencies, bool tryReverse = false);
};

#endif // CURRENCY_H
