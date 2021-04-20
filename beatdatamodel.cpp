#include "beatdatamodel.h"
#include "globals.h"
#include "beattimestamp.h"
#include "eventdataproxymodel.h"
#include <QSize>
#include <QtAlgorithms>
#include "beatoptimisation.h"
#include "beatinterval.h"
#include "mainwindow.h"

BeatDataModel::BeatDataModel(QObject *parent) :
    QAbstractTableModel(parent),
    originModel(NULL)
{

}

int BeatDataModel::rowCount ( const QModelIndex &) const
{
    return beatTimestamps.size();
}

int BeatDataModel::columnCount ( const QModelIndex & parent) const
{
    if (parent != QModelIndex())
        return 0;
    else
        return 4;
}

QVariant BeatDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant returnValue = QVariant();

    //range check
    if (section >= columnCount())
        return returnValue;

    if (role == Qt::SizeHintRole)
    {
        switch (section)
        {
        case 0:
            returnValue = QSize(80,20);
            break;
        case 1:
            returnValue = QSize(38,20);
            break;
        case 2:
            returnValue = QSize(45,20);
            break;
        case 3:
            returnValue = QSize(30,20);
            break;
        default:
            returnValue = QSize(20,20);
        }
    }

    if (role == Qt::DisplayRole)
    {
        //this only does top (horizontal) header
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case 0: returnValue = QString("Timestamp");
                break;
            case 1: returnValue = QString("Type");
                break;
            case 2: returnValue = QString("Value");
                break;
            case 3: returnValue = QString("Optional");
                break;
            default: returnValue = section;
            }
        }
    }

    return returnValue;
}

Qt::ItemFlags BeatDataModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant BeatDataModel::data ( const QModelIndex & index, int role ) const
{
    QVariant returnValue = QVariant();

    if (role != Qt::DisplayRole)
        return returnValue;

    switch (index.column())
    {
        case 0: returnValue = int(beatTimestamps.at(index.row()).eventData.timestamp);
                break;
        case 1: returnValue = beatTimestamps.at(index.row()).eventData.type();
                break;
        case 2: returnValue = beatTimestamps.at(index.row()).eventData.value;
                break;
        case 3: returnValue = beatTimestamps.at(index.row()).eventData.isOptional();
                break;
        default:
                break;
    }

    return returnValue;
}

void BeatDataModel::addEvent(const Event &newEvent, int index)
{
    BeatTimestamp newTimestamp(index,newEvent);
    if (beatTimestamps.isEmpty())
    {
        beginInsertRows(QModelIndex(),0,0);
        beatTimestamps.append(newTimestamp);
        endInsertRows();
    }
    else
    {
        //check for duplicates
        int i = 0;
        if (index >= 0)
            for (i = 0; i < beatTimestamps.size(); ++i)
                if (index == beatTimestamps[i].originalRowIndex)
                    //do nothing for duplicate indexes
                    return;

        if (newTimestamp.eventData.timestamp >= beatTimestamps.last().eventData.timestamp)
        {
            beginInsertRows(QModelIndex(),0,0);
            beatTimestamps.append(newTimestamp);
            endInsertRows();
        }
        else
        {
            int insertAt = 0;
            while (newTimestamp.eventData.timestamp >= beatTimestamps[insertAt].eventData.timestamp)
                ++insertAt;

            beginInsertRows(QModelIndex(),insertAt,insertAt);
            beatTimestamps.insert(insertAt,newTimestamp);
            endInsertRows();
        }
    }
}

void BeatDataModel::addBeats(EventDataProxyModel * model, const QModelIndexList &indexList)
{
    originModel = model;

    int i = 0;
    for (i = 0; i < indexList.size(); ++i)
    {
        addBeat(indexList[i]);
    }
}

void BeatDataModel::addBeat(const QModelIndex &index)
{
    int newRowIndex = index.row();
    addEvent(originModel->eventFromIndex(index),newRowIndex);
}

Event BeatDataModel::eventFromRow(int row)
{
    return beatTimestamps.at(row).eventData;
}

void BeatDataModel::changeEventAt(int indexToChange, const Event & event)
{
    beatTimestamps[indexToChange].eventData = event;
    emit dataChanged(index(indexToChange,0),index(indexToChange,columnCount()));
}

void BeatDataModel::changeTimestampAt(int indexToChange, long newTimestamp)
{
    beatTimestamps[indexToChange].eventData.timestamp = newTimestamp;
    emit dataChanged(index(indexToChange,0),index(indexToChange,0));
}

void BeatDataModel::removeBeat(int index)
{
    beginRemoveRows(QModelIndex(),index,index);
    if (beatTimestamps[index].originalRowIndex >= 0)
        indexesToDelete.append(beatTimestamps[index].originalRowIndex);
    beatTimestamps.removeAt(index);
    endRemoveRows();
}

bool BeatDataModel::removeRows (int row,int count,const QModelIndex &)
{
    for (int i = 0; i < count; ++i)
        removeBeat(row);

    return true;
}

BeatTimestamp & BeatDataModel::operator[](int index)
{
    return beatTimestamps[index];
}

void BeatDataModel::cancelQueuedDeletions()
{
    indexesToDelete.clear();
}

void BeatDataModel::writeChangesToOriginalModel()
{
    //set up metadata for events
    for (int i = 0; i < beatIntervals.size(); ++i)
    {
        if (beatTimestamps[i].eventData.metadata == NULL)
            beatTimestamps[i].eventData.metadata = new EventMetadata;

        EventMetadata * thisMetadata = beatTimestamps[i].eventData.metadata;
        thisMetadata->tempo = (60 * 1000) / BeatOptimisation::improvedTempoInterval;
        if (beatIntervals.at(i).isKnownBeatValue())
        {
            thisMetadata->valueNumerator = beatIntervals.at(i).getValue()->numerator;
            thisMetadata->valueDenominator = beatIntervals.at(i).getValue()->denominator;
        }
    }
    for (int i = 0; i < beatPatterns.size(); ++i)
    {
        int firstBeatOfPattern = beatPatterns[i].startsAtBeat();
        beatTimestamps[firstBeatOfPattern].eventData.metadata->startsNewPattern = true;
        int lastBeatOfPattern = firstBeatOfPattern + beatPatterns[i].totalStrokesCovered();
        for (int j = firstBeatOfPattern, k = 0; j <= lastBeatOfPattern; ++j, ++k)
        {
            if (beatTimestamps[j].eventData.metadata == NULL)
                beatTimestamps[j].eventData.metadata = new EventMetadata;
            beatTimestamps[j].eventData.metadata->patternName = beatPatterns[i].name();
            beatTimestamps[j].eventData.metadata->patternIndex = k % beatPatterns[i].lengthInStrokes();
        }
    }

    //start by making changes to items that exist in both models
    int i = 0;
    for (i = 0; i < beatTimestamps.size(); ++i)
    {
        if (beatTimestamps[i].originalRowIndex >= 0)
            originModel->setEventAtIndex(originModel->index(beatTimestamps[i].originalRowIndex,0),beatTimestamps[i].eventData);
    }

    //then delete any events that have been deleted in the editor (reverse order to avoid index issues)
    qSort(indexesToDelete);
    for (i = indexesToDelete.size(); i > 0; --i)
        originModel->removeEvent(indexesToDelete[i-1]);
    indexesToDelete.clear();

    //then add any new events
    for (i = 0; i < beatTimestamps.size(); ++i)
    {
        if (beatTimestamps[i].originalRowIndex < 0)
            originModel->addEvent(beatTimestamps[i].eventData);
    }

    mainWindow->registerUnsyncedChanges();
}
