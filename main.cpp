#include <QCoreApplication>

#include "currency.h"
#include "xmlratereader.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    XmlRateReader reader("/Users/hchapman/Desktop/international-trade/doc/RATES.xml");
    reader.read();
    return 0;
//    return a.exec();
}
