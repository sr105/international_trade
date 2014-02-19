#include <QCoreApplication>

#include <QFile>
#include <QByteArray>
#include <QStringList>
#include <QDebug>

#include "currency.h"
#include "xmlratereader.h"

#include <math.h>

double roundToEven(double value) {
    // Only need tie-breaker if the fraction is
    // 0.5 exactly
    if (qFuzzyCompare(fabs(value - trunc(value)), 0.5)) {
        qDebug() << " **** ";
        // return nearest even
        double a = ceil(value);
        if (int(a) % 2 == 0)
            return a;
        return floor(value);
    }

    return round(value);
}

void testRounding() {
    QList<double> nums;
    nums << 24.50 << 23.67 << 23.50 << 23.35 << 23.00 << 0 << -23.00 << -23.35 << -23.50 << -23.67 << -24.50;
    foreach (double f, nums)
        qDebug() << f << roundToEven(f);
}

void total(QString filename, QString item, QString currency) {
    // for each line, split, if matches item, get currency, convert, add to total
    double sum = 0.0;
    QFile csv(filename);
    csv.open(QIODevice::ReadOnly);
    while (true) {
        QString line = csv.readLine();
        if (line.isEmpty())
            break;
        QStringList split = line.split(QRegExp("[, \n]"));
        if (split.size() < 4 || split[1] != item)
            continue;
        bool ok = false;
        double amount = split[2].toDouble(&ok);
        Currency *c = Currency::get(split[3]);
        if (!ok) {
            qDebug() << "Failed to parse amount:" << split[2];
            continue;
        }
        if (!c || !c->hasRate(currency)) {
            qDebug() << "No known conversion from" << split[3] << "to" << currency;
            continue;
        }
        qDebug("%18.12g", sum);
        qDebug("%18.12g", roundToEven(amount * c->to(currency) * 100.0) / 100.0);
        qDebug() << split[0].trimmed();
//        qDebug() << sum << amount << c->to(currency) << "+="
//                 << amount * c->to(currency) << "("
//                 << roundToEven(amount * c->to(currency) * 100.0) / 100.0 << ")"
//                 << line.trimmed();
        sum += roundToEven(amount * c->to(currency) * 100.0) / 100.0;
    }
    qDebug("sum: %5.7g", sum);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //testRounding();

    //XmlRateReader reader("/Users/hchapman/Desktop/international-trade/doc/test_rates.xml");

    //XmlRateReader reader("/Users/hchapman/Desktop/international-trade/doc/SAMPLE_RATES.xml");
    XmlRateReader reader("/Users/hchapman/Desktop/international-trade/doc/RATES.xml");
    reader.read();

    Currency::printMap();
    Currency::fillInTable();
    Currency::printMap();

    // Read in sales csv
    // Find total sales in USD for DM1182
    //total("/Users/hchapman/Desktop/international-trade/doc/SAMPLE_TRANS.csv", "DM1182", "USD");
    total("/Users/hchapman/Desktop/international-trade/doc/TRANS.csv", "DM1182", "USD");

    return 0;
//    return a.exec();
}
