#ifndef EVENTSTRUCT_H
#define EVENTSTRUCT_H

#include <QString>
#include "eventmetadata.h"
#include "globals.h"

enum e_eventType
{
    EVENT_RESET_TIMECODE,
    EVENT_NORMAL_WAND_PULSE,
    EVENT_NORMAL_WAND_PULSE_END,
    EVENT_TIMED_WAND_PULSE,
    EVENT_TIMED_WAND_PULSE_END,
    EVENT_EDGE_BEGIN,
    EVENT_EDGE_END,
    EVENT_ANAL_VIBRATOR_PULSE,
    EVENT_ANAL_VIBRATOR_PULSE_END,
    EVENT_INFLATABLE_BUTT_PLUG_INFLATE,
    EVENT_INFLATABLE_BUTT_PLUG_DEFLATE,
    EVENT_AIR_PUMP_ON,
    EVENT_AIR_PUMP_OFF,
    EVENT_RECORDING_MISTAKE,
    EVENT_UNUSED,
    EVENT_STROKER_WAYPOINT //for things like handy, launch etc.
};

struct Event
{
public:
    Event();
    Event(long timestamp, unsigned char type, short value);
    Event(long timestamp, unsigned char type, short value, bool optional);
    Event(long timestamp, unsigned char type, short value, EventMetadata metadata);

    bool operator==(const Event &) const;

    QString toAsciiString();
    QString toBinary();

    long timestamp;

    static bool writeAllToXml(QString filename);

private:
    char typeData;
public:
    unsigned char type() const;
    void setType(unsigned char newType);
    void setTypeRaw(char newTypeData);
    short value;
    bool optional() const;
    bool isOptional() const;
    void setOptional(bool optionality);
    EventMetadata * metadata;
};

#endif // EVENTSTRUCT_H
