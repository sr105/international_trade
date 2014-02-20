#include <QCoreApplication>
#include <QStringList>
#include <QDebug>

#include "xmlratereader.h"
#include "xmldomratereader.h"
#include "xqueryratereader.h"
#include "transactions.h"

#include "currency.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString currency("USD"), sku("DM1182");
    QStringList args = a.arguments();
    switch(args.size()) {
    default:
        qDebug() << "Usage:" << qPrintable(args[0]) << "rates.xml transactions.csv [sku [currency]]";
        return 1;
    case 5:
        currency = args[4];
    case 4:
        sku = args[3];
    case 3:
        break;
    }

    //readRatesUsingXQuery(args[1]);
    readRatesUsingXQuery2(args[1]);

    //XmlRateReader reader(args[1]);
    //reader.read();

    //readRatesFromXml(args[1]);

    // Read in sales csv and find total sales in currency for sku
    printf("%.2f\n", (float)totalTransactions(args[2], sku, currency));
    return 0;

    // Do all combinations to see if any of them trigger a rounding tie-breaker
    // Answer: nope
    QString b = "DM1182 DM1210 DM1230 DM1294 DM1537 DM1541 DM1724 DM1759 DM1786 DM1873";
    foreach (const QString &item_sku, b.split(" "))
        foreach (const QString &code, Currency::currencies())
            printf("%s %s %.2f\n",
                   qPrintable(item_sku),
                   qPrintable(code),
                   (float)totalTransactions(args[2], item_sku, code));

    return 0;
}
