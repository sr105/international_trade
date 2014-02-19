#ifndef CURRENCY_H
#define CURRENCY_H

#include <QHash>
#include <QString>

class Currency {
public:
    static Currency *get(const QString name);
    QString name() const;

    float to(QString currency) const;
    //float from(QString currency);

    void insert(const QString name, float conversion);
    static void printMap();

private:
    QString _name;
    QHash<QString, float> rates;
    static QHash<QString, Currency *> map;

    Currency(const QString name) : _name(name) {}
    Currency(const Currency &) {}
};

#endif // CURRENCY_H
