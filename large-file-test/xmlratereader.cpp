#include "xmlratereader.h"

#include <QFile>
#include <QHash>
#include <QDebug>

// Pick one of these:
//
//#define PRINT_START_ELEMENT_TREE 1
#define USE_RECURSIVE_APPROACH 1       // uses a heirarchy
//#define USE_FIND_ALL_NAMED_ELEMENTS 1  // like QDomDocument::elementsByTagName()

XmlRateReader::XmlRateReader(const QString filename) :
    _filename(filename),
    _total(0)
{}

void XmlRateReader::read() {
    QFile xmlFile(_filename);
    xmlFile.open(QIODevice::ReadOnly);
    _xml.setDevice(&xmlFile);

#if PRINT_START_ELEMENT_TREE
    printStartElementTree();
#elif USE_RECURSIVE_APPROACH
    processAllWithMethod(QStringList() << "update" << "install" << "file",
                          &XmlRateReader::processFile);
#elif USE_FIND_ALL_NAMED_ELEMENTS
    processAllNamedElementsWithMethod("file", &XmlRateReader::processFile);
#endif

    // A single readNext() is required here or else we'll get an error.
    _xml.readNext();
    if (_xml.hasError()) {
        _xml.raiseError();
        qDebug() << errorString();
    }
}

// This method serves to illustrate how readNextStartElement()
// works. The key things to remember are that the current stream
// location may not match the current element, and it always
// descends the tree. Did you get that? Every time it's called it
// descends one level. It returns false and points to an End element
// when it can no longer descend. At that point the stream will point
// to the end element, but the current logical element is the parent
// containing that end element. This is imporant because if you call
// skipCurrentElement() while sitting on an end element, you're
// actually skipping to the end of the parent because it is the
// current element. In the example below, calling skipCurrentElement()
// at the end of line 4 would move the stream to the end of line 7.
//
// Example: pretend we call readNextStartElement() at each line below.
//                      stream      current     returns     skip moves to
// 0. --- nothing ---
// 1. <zero>               zero        zero        true     8
// 2.    <one>             one         one         true     7
// 3.       <two>          two         two         true     4
// 4.       </two>         /two        one         false    7
// 5.       <three>        three       three       true     6
// 6.       </three>       /three      one         false    7
// 7.    </one>            /one        zero        false    8
// 8. </zero>              /zero       ??          false    end of doc
void XmlRateReader::printStartElementTree() {
    int indent = 4;
    int level = 0;
    while (!_xml.atEnd()) {
        while (_xml.readNextStartElement())
            printCurrent(QString(level++ * indent, ' '));
        level--;
    }
    printCurrent(QString(level * indent, ' '));
}

// Helper method to print out where we are in the stream
void XmlRateReader::printCurrent(QString extra) {
    qDebug("%s %s: %s",
           qPrintable(extra),
           qPrintable(_xml.tokenString().remove("Element")),
           qPrintable(_xml.name().toString()));
}

// Acts like QDomDocument::elementsByTagName(name), applying method to each one
void XmlRateReader::processAllNamedElementsWithMethod(QString name, member_fn_type method) {
    if (name.isEmpty() || method == 0)
        return;

    while (!_xml.atEnd()) {
        while (_xml.readNextStartElement()) {
            if (_xml.name() == name) {
                (this->*method)();
                // Ensure that we are at the end of the element
                // matching name or else we might match a nested
                // element with the same name:
                // <name> ... <name>...<name/> ... </name>
                //
                // skipCurrentElement() moves forward to the nearest
                // end element which moves the current element up a
                // level. Using <0><1></1><2></2></0>: if we start at
                // <1>, we'd skip to </1>, then </0>.
                //
                // Edge case that we can't do anything about:
                // <0>  <1>  <0></0> <0></0> </1> </0>
                //           ^- start here
                // If we start from the second <0>, skip moves to the
                // nearby </0>. This will drop us out of the skip loop.
                // Then readNextStartElement() will put as at the next
                // <0> which will trigger a call to method(). The only
                // way to avoid this is to build our own local readNext
                // skip methods which keep track of what level we're on.
                // Then, you'd need to ensure that method() only used
                // our methods to move around. QXmlStreamReader doesn't
                // expose any way of knowing where we are in the
                // tree.
                while(!(_xml.isEndElement() && _xml.name() == name))
                    _xml.skipCurrentElement();
            }
        }
    }
}

// Like processAllNamedElementsWithMethod() above but with a defined heirarchy.
// method will only be applied to elements with a heirarchy that matches names.
// Example: names = QStringList() << "root element" << "next level" << ... << "name";
void XmlRateReader::processAllWithMethod(QStringList names, member_fn_type method)
{
    if (names.isEmpty() || method == 0)
        return;

    QString currentElementName = _xml.name().toString();
    QString name = names.first();
    names.removeFirst();
    while (!_xml.atEnd()) {
        while (_xml.readNextStartElement()) {
            if (_xml.name() == name) {
                if (names.isEmpty())
                    (this->*method)();
                else
                    processAllWithMethod(names, method);
                // The just called level may not have left us
                // at the end of the element named name. This
                // explicitly moves us there. Otherwise we might
                // recurse unexpectedly. Example: if "name" has
                // a child element named "name" and we've returned
                // here before seeing the child, we'll end up
                // processing the child as if it were the parent.
                while(!(_xml.isEndElement() && _xml.name() == name))
                    _xml.skipCurrentElement();
            } else {
                if (!_xml.isEndElement())
                    _xml.skipCurrentElement();
            }
        }
        if (_xml.isEndElement() && _xml.name() == currentElementName)
            break;
    }
}

void XmlRateReader::processFile() {
    QHash<QString, QString> results = getTextElements(QStringList() << "size" << "hash");
    QString size = results.value("size", "");
    QString hash = results.value("hash", "");
    if (hash.isEmpty() || size.isEmpty())
        return;

    if (hash[0].isDigit())
        _total += size.toULongLong();
}

// Find each child element of the current element with a name found in names.
// Return a hash table of string results.
// The code can be made to return early once all names have been found.
// One method is to stop when the number of results equals the number of names.
// The other is to remove keys from names as they're found and return when
// names is empty. Performance may differ based on the number of child elements
// and the number of keys in names. Regardless of which approach is used, sorting
// names by the order that the keys can expect to be seen will yield the best results.
QHash<QString, QString> XmlRateReader::getTextElements(QStringList names) {
    QHash<QString, QString> results;
    // readNextStartElement() descends the XML tree which
    // is undesirable, but we skip over descendants below
    // which keeps us on the same level.
    while (!names.isEmpty() && _xml.readNextStartElement()) {
        if (!names.contains(_xml.name().toString())) {
            _xml.skipCurrentElement();
            continue;
        }
        // readElementText() internally skips the current element
        // TODO: I think readElementText() can throw
        results.insert(_xml.name().toString(),
                       _xml.readElementText(_xml.SkipChildElements));
        names.removeOne(_xml.name().toString());
    }
    return results;
}


QString XmlRateReader::errorString() {
    return QObject::tr("%1\nLine %2, column %3")
            .arg(_xml.errorString())
            .arg(_xml.lineNumber())
            .arg(_xml.columnNumber());
}
