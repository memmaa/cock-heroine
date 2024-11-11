#ifndef CHMLHANDLER_H
#define CHMLHANDLER_H

#include <QXmlDefaultHandler>
#include "event.h"

class CHMLHandler : public QXmlDefaultHandler
{
public:
    CHMLHandler();

    bool startDocument();

    bool startElement( const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes & atts );

    bool characters ( const QString & text );

    bool endElement( const QString & namespaceURI, const QString & name, const QString & qName );

private:
    bool inEventsSection;
    bool inEvent;
    bool inMetadata;
    QString currentEventProperty;
    Event * currentEvent;

};

#endif // CHMLHANDLER_H
