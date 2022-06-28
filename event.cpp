#include "event.h"
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include "mainwindow.h" //just for tr()

Event::Event()
    : timestamp(0),
      typeData(0),
      value(0),
      metadata(NULL)
{
    //nothing to do here...
}

Event::Event(long timestamp, unsigned char type, short value)
    : timestamp(timestamp),
      typeData(type),
      value(value),
      metadata(NULL)
{
    //nothing to do here...
}

Event::Event(long timestamp, unsigned char type, short value, bool optional)
    : timestamp(timestamp),
      typeData(optional ?
                   type | 0x80 :
                   type & 0x7F),
      value(value),
      metadata(NULL)
{
    //nothing to do here...
}

bool Event::operator==(const Event & other) const
{
    return timestamp == other.timestamp &&
           typeData      == other.typeData      &&
           value     == other.value     ;
}

QString Event::toAsciiString()
{
    QString returnValue = QString("a%1,%2,%3.").arg(timestamp).arg((int)typeData).arg(value);
//    qDebug() << "Created Ascii String: " << returnValue;
    return returnValue;
}

unsigned char Event::type() const
{
    return typeData & 0x7F;
}

QString Event::typeName() const
{
    unsigned char t = type();
    switch (t) {
    case EVENT_RESET_TIMECODE:
        return MainWindow::tr("Reset Timecode");
    case EVENT_NORMAL_WAND_PULSE:
        return MainWindow::tr("Full Stroke");
    case EVENT_NORMAL_WAND_PULSE_END:
        return MainWindow::tr("Wand Pulse End");
    case EVENT_TIMED_WAND_PULSE:
        return MainWindow::tr("Fixed Length Vibrator Pulse");
    case EVENT_TIMED_WAND_PULSE_END:
        return MainWindow::tr("Fixed Length Vibrator Pulse End");
    case EVENT_EDGE_BEGIN:
        return MainWindow::tr("Start Edging");
    case EVENT_EDGE_END:
        return MainWindow::tr("Stop Edging");
    case EVENT_ANAL_VIBRATOR_PULSE:
        return MainWindow::tr("Anal Vibrator Pulse");
    case EVENT_ANAL_VIBRATOR_PULSE_END:
        return MainWindow::tr("Anal Vibrator Pulse End");
    case EVENT_INFLATABLE_BUTT_PLUG_INFLATE:
        return MainWindow::tr("Inflate Inflatable Butt Plug");
    case EVENT_INFLATABLE_BUTT_PLUG_DEFLATE:
        return MainWindow::tr("Deflate Inflatable Butt Plug");
    case EVENT_AIR_PUMP_ON:
        return MainWindow::tr("Turn Vacuum/Air Pump On");
    case EVENT_AIR_PUMP_OFF:
        return MainWindow::tr("Turn Vacuum/Air Pump Off");
    case EVENT_RECORDING_MISTAKE:
        return MainWindow::tr("Marker");
    case EVENT_STROKER_WAYPOINT: //for things like handy, launch etc.
        return MainWindow::tr("Stroker Waypoint");
    default:
        return MainWindow::tr("Undefined");
    }
    return MainWindow::tr("Undefined");
}

void Event::setType(unsigned char newType)
{
    typeData = optional() ?
                 newType | 0x80 :
                 newType & 0x7F;
}

void Event::setTypeRaw(char newType)
{
    typeData = newType;
}

short Event::maxPossibleValue()
{
    switch (type()) {
    case EVENT_EDGE_BEGIN:
    case EVENT_EDGE_END:
        return 1;
    case EVENT_STROKER_WAYPOINT:
        return 100;
    case EVENT_TIMED_WAND_PULSE:
        return __SHRT_MAX__;
    default:
        return 255;
    }

    return 255;
}

bool Event::optional() const
{
    return typeData & 0x80;
}

bool Event::isOptional() const
{
    return optional();
}

void Event::setOptional(bool optionality)
{
    if (optionality)
        typeData |= 0x80;
    else
        typeData &= 0x7F;
}

bool Event::writeAllToXml(QString filename)
{
    //create XML file in memory
    QDomDocument chmlDoc("CockHeroML");
    QDomElement eventsNode = chmlDoc.createElement("events");

    //add each event node to the events node
    for (int i = 0; i < events.size(); ++i)
    {
        QDomElement eventNode = chmlDoc.createElement("event");

        //standard values:
        QDomElement timestamp = chmlDoc.createElement("timestamp");
        QDomText timestampAsText = chmlDoc.createTextNode(QString("%1").arg(events[i].timestamp));
        timestamp.appendChild(timestampAsText);
        eventNode.appendChild(timestamp);
        QDomElement type = chmlDoc.createElement("type");
        QDomText typeAsText = chmlDoc.createTextNode(QString("%1").arg((int)events[i].typeData));
        type.appendChild(typeAsText);
        eventNode.appendChild(type);
        QDomElement value = chmlDoc.createElement("value");
        QDomText valueAsText = chmlDoc.createTextNode(QString("%1").arg(events[i].value));
        value.appendChild(valueAsText);
        eventNode.appendChild(value);

        if (events[i].metadata != NULL)
        {
            //metadata:
            QDomElement metadataNode = chmlDoc.createElement("metadata");

            if (events[i].metadata->tempo != 0)
            {
                QDomElement tempo = chmlDoc.createElement("tempo");
                QDomText tempoAsText = chmlDoc.createTextNode(QString("%1").arg(events[i].metadata->tempo,0,'f',5));
                tempo.appendChild(tempoAsText);
                metadataNode.appendChild(tempo);
            }

            if (events[i].metadata->valueNumerator != 0 &&
                    events[i].metadata->valueNumerator != 0)
            {
                QDomElement numerator = chmlDoc.createElement("numerator");
                QDomText numeratorAsText = chmlDoc.createTextNode(QString("%1").arg(events[i].metadata->valueNumerator));
                numerator.appendChild(numeratorAsText);
                metadataNode.appendChild(numerator);

                QDomElement denominator = chmlDoc.createElement("denominator");
                QDomText denominatorAsText = chmlDoc.createTextNode(QString("%1").arg(events[i].metadata->valueDenominator));
                denominator.appendChild(denominatorAsText);
                metadataNode.appendChild(denominator);
            }

            if (events[i].metadata->startsNewPattern)
            {
                QDomElement startsNewPattern = chmlDoc.createElement("startsNewPattern");
                QDomText startsNewPatternAsText = chmlDoc.createTextNode("true");
                startsNewPattern.appendChild(startsNewPatternAsText);
                metadataNode.appendChild(startsNewPattern);
            }

            if (events[i].metadata->startsNewRound)
            {
                QDomElement startsNewRound = chmlDoc.createElement("startsNewRound");
                QDomText startsNewRoundAsText = chmlDoc.createTextNode("true");
                startsNewRound.appendChild(startsNewRoundAsText);
                metadataNode.appendChild(startsNewRound);
            }

            if (events[i].metadata->endsRound)
            {
                QDomElement endsRound = chmlDoc.createElement("endsRound");
                QDomText endsRoundAsText = chmlDoc.createTextNode("true");
                endsRound.appendChild(endsRoundAsText);
                metadataNode.appendChild(endsRound);
            }

            if (!events[i].metadata->patternName.isEmpty())
            {
                QDomElement pattern = chmlDoc.createElement("pattern");
                pattern.setAttribute("patternIndex", events[i].metadata->patternIndex);
                QDomText patternAsText = chmlDoc.createTextNode(events[i].metadata->patternName);
                pattern.appendChild(patternAsText);
                metadataNode.appendChild(pattern);
            }

            eventNode.appendChild(metadataNode);
        }

        eventsNode.appendChild(eventNode);
    }

    //add the whole events node to the file in memory
    chmlDoc.appendChild(eventsNode);

    //write the file to disk
    QFile outputFile (filename);
    if(!outputFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not open file for writing!";
        return false;
    }
    QTextStream outputStream(&outputFile);
    outputStream << chmlDoc.toString();

    outputFile.close();
    return true;
}
