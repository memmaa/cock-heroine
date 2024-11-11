#include "funscriptwriter.h"
#include "optionsdialog.h"

FunscriptWriter::FunscriptWriter(QFile & file, QObject * parent)
    :
    SyncFileWriter(file, parent)
{

}

void FunscriptWriter::writeHeader()
{
    //TODO: This whole class could be rewritten using proper JSON handling, but hey, it works...
    int topOfRange = OptionsDialog::getRangeTop();
    int bottomOfRange = OptionsDialog::getRangeBottom();
    int rangeAnchor = OptionsDialog::getRangeAnchor();
    int proportionalLength = OptionsDialog::getStrokeLengthProportion();
    QString fileHeader = QString("{\"export_comment\":\"Exported from Cock Heroine using range %1%% to %2%%, %3%% anchor point and %4%% proportional stroke length. "
                                 "Exporting tempo, beat values and non-standard stroke events such as edging and butt plug control events not supported at time of writing.\","
                                 "\"version\":\"1.0\",\"inverted\":false,\"range\":100,\"actions\":[")
                                                                                                .arg(bottomOfRange)
                                                                                                        .arg(topOfRange)
                                                                                                              .arg(rangeAnchor)
                                                                                                                                    .arg(proportionalLength);
    file.write(fileHeader.toLatin1());
    needPreceedingComma = false;
}

void FunscriptWriter::addEvent(long timestamp, int position)
{
    if (needPreceedingComma)
        file.write(",");
    QString eventString = funscriptEntryString(timestamp, position);
    file.write(eventString.toLatin1());
    needPreceedingComma = true;
}

void FunscriptWriter::writeFooter()
{
    file.write("]}");
}

QString FunscriptWriter::funscriptEntryString(long timestamp, int targetLocation)
{
    return QString("{\"pos\":%1,\"at\":%2}").arg(targetLocation).arg(timestamp);
}
