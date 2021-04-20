#include "chmlhandler.h"
#include <QDebug>
#include "globals.h"
#include "mainwindow.h"

CHMLHandler::CHMLHandler()
{
}

bool CHMLHandler::startDocument()
{
    inEventsSection = inEvent = false;
    currentEventProperty = "none";
    return true;
}

bool CHMLHandler::startElement( const QString &, const QString & name, const QString &, const QXmlAttributes &atts)
{
    if (name == "events")
    {
        inEventsSection = true;
    }
    else if (name == "event")
    {
        currentEvent = new Event;
        inEvent = true;
    }
    else if (name == "timestamp" ||
             name == "type" ||
             name == "value")
    {
        currentEventProperty = name;
    }
    else if (name == "metadata")
    {
        inMetadata = true;
        currentEvent->metadata = new EventMetadata;
    }
    else if (name == "tempo" ||
             name == "numerator" ||
             name == "denominator" ||
             name == "startsNewPattern" ||
             name == "startsNewRound" ||
             name == "endsRound")
    {
        currentEventProperty = name;
    }
    else if (name == "pattern")
    {
        currentEvent->metadata->patternIndex = atts.value(QString(),"patternIndex").toInt();
        currentEventProperty = name;
    }
    else
    {
        qDebug() << name << " is not a recognised XML node.";
        return false;
    }

    return true;
}

bool CHMLHandler::characters ( const QString & text )
{
    if (inEventsSection && inEvent && currentEventProperty != "none")
    {
        if (currentEventProperty == "timestamp")
            currentEvent->timestamp = text.toLong();
        else if (currentEventProperty == "type")
            currentEvent->setTypeRaw(text.toInt());
        else if (currentEventProperty == "value")
            currentEvent->value = text.toInt();
        else if (inMetadata)
        {
            if (currentEventProperty == "tempo")
                currentEvent->metadata->tempo = text.toFloat();
            else if (currentEventProperty == "numerator")
                currentEvent->metadata->valueNumerator = text.toInt();
            else if (currentEventProperty == "denominator")
                currentEvent->metadata->valueDenominator = text.toInt();
            else if (currentEventProperty == "startsNewPattern")
                currentEvent->metadata->startsNewPattern = text == "true";
            else if (currentEventProperty == "startsNewRound")
                currentEvent->metadata->startsNewRound = text == "true";
            else if (currentEventProperty == "endsRound")
                currentEvent->metadata->endsRound = text == "true";
            else if (currentEventProperty == "pattern")
                currentEvent->metadata->patternName = text;
        }
    }

    return true;
}

bool CHMLHandler::endElement( const QString & , const QString & name, const QString &)
{
    if (name == "events")
    {
        inEventsSection = false;
    }
    else if (name == "event")
    {
        mainWindow->addEventToTable(*currentEvent);
        delete currentEvent;
        currentEvent = NULL;
        inEvent = false;
    }
    else if (name == "metadata")
    {
        inMetadata = false;
    }
    else if (name == "timestamp" ||
             name == "type" ||
             name == "value" ||
             name == "tempo" ||
             name == "numerator" ||
             name == "denominator" ||
             name == "startsNewPattern" ||
             name == "startsNewRound" ||
             name == "endsRound" ||
             name == "pattern")
    {
        currentEventProperty = "none";
    }
    else
    {
        qDebug() << name << " is not a recognised XML node.";
        return false;
    }

    return true;
}
