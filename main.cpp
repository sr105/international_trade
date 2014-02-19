#include <QCoreApplication>

#include "currency.h"
#include "xmlratereader.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    XmlRateReader reader("/Users/hchapman/Desktop/international-trade/doc/test_rates.xml");

//    XmlRateReader reader("/Users/hchapman/Desktop/international-trade/doc/SAMPLE_RATES.xml");
    //XmlRateReader reader("/Users/hchapman/Desktop/international-trade/doc/RATES.xml");
    reader.read();

    Currency::printMap();
    Currency::fillInTable();
    Currency::printMap();

    return 0;
//    return a.exec();
}
