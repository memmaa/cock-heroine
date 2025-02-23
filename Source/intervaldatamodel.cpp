#include "intervaldatamodel.h"
#include "beatinterval.h"
#include "helperfunctions.h"
#include "globals.h"
#include "QSize"
#include "beatanalysis.h"
#include <QBrush>
#include <QColor>

IntervalDataModel::IntervalDataModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int IntervalDataModel::rowCount ( const QModelIndex &) const
{
    return beatTimestamps.size() - 1;
}

int IntervalDataModel::columnCount ( const QModelIndex & parent) const
{
    if (parent != QModelIndex())
        return 0;
    else
        return 3;
}

QVariant IntervalDataModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            returnValue = QSize(115,20);
            break;
        case 2:
            returnValue = QSize(38,20);
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
            case 0: returnValue = QString("Length (ms)");
                break;
            case 1: returnValue = QString("Value");
                break;
            case 2: returnValue = QString("Accuracy");
                break;
            default: returnValue = section;
            }
        }
    }

    return returnValue;
}

Qt::ItemFlags IntervalDataModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant IntervalDataModel::data ( const QModelIndex & index, int role ) const
{
    QVariant returnValue = QVariant();

    //shouldn't happen, but sometimes does because the code's a bit sloppy around here.
    if (index.row() >= beatIntervals.size())
        return returnValue;

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
            case 0: returnValue = beatIntervals.at(index.row()).getIntLength();
                break;
            case 1: if (beatIntervals.at(index.row()).isKnownBeatValue())
                    {
                        const BeatValue * val = beatIntervals.at(index.row()).getValue();
                        returnValue = val->name;
                    }
                else
                    returnValue = QString("Unknown");
                break;
            case 2: if (beatIntervals.at(index.row()).getValue() == NULL)
                    returnValue = 0;
                else
                {
                    float difference = beatIntervals.at(index.row()).absoluteDifferenceFromNearestKnownBeatValue();
                    if (difference < 1)
                        returnValue = 100;
                    else
                        returnValue = int(100 - (beatIntervals.at(index.row()).deviationFromNearestKnownBeatValue() / BeatAnalysis::Configuration::maxPercentAcceptableBeatError * 100));
                }
                break;
            default:
                break;
        }
    }

    if (role == Qt::BackgroundRole)
    {
        if ( ! beatIntervals.at(index.row()).isKnownBeatValue())
            returnValue = QBrush(QColor("Dark Red"));
        else
        {
            short hue = 120 - (beatIntervals.at(index.row()).deviationFromNearestKnownBeatValue() / BeatAnalysis::Configuration::maxPercentAcceptableBeatError * 120);
            returnValue = QBrush(QColor::fromHsl(hue,255,191));
        }
    }

    if (role == Qt::ForegroundRole &&
        false == beatIntervals.at(index.row()).isKnownBeatValue())
            returnValue = QBrush(QColor("White"));

    return returnValue;
}

bool IntervalDataModel::containsUnknownIntervals()
{
    int i = 0;
    for (i = 0; i < beatIntervals.size(); ++i)
        if ( ! beatIntervals[i].isKnownBeatValue())
            return true;

    return false;
}

bool IntervalDataModel::containsPoorlyMatchedIntervals (float threshold)
{
    int i = 0;
    for (i = 0; i < beatIntervals.size(); ++i)
        if ( 100 - (beatIntervals[i].deviationFromNearestKnownBeatValue() / BeatAnalysis::Configuration::maxPercentAcceptableBeatError * 100) < threshold)
            return true;

    return false;
}
