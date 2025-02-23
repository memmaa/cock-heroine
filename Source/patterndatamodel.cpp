#include "patterndatamodel.h"
#include "globals.h"
#include <QSize>
#include "helperfunctions.h"
#include "beatanalysis.h"
#include <QBrush>

PatternDataModel::PatternDataModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int PatternDataModel::rowCount ( const QModelIndex &) const
{
    return beatPatterns.size();
}

int PatternDataModel::columnCount ( const QModelIndex & parent) const
{
    if (parent != QModelIndex())
        return 0;
    else
        return 6;
}

QVariant PatternDataModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            returnValue = QSize(102,20);
            break;
        case 1:
            returnValue = QSize(55,20);
            break;
        case 2:
            returnValue = QSize(22,20);
            break;
        case 3:
            returnValue = QSize(32,20);
            break;
        case 4:
            returnValue = QSize(38,20);
            break;
        case 5:
            returnValue = QSize(78,20);
            break;
        default:
            returnValue = QSize(20,20); //this doesn't work anyway, so meh.
        }
    }

    if (role == Qt::DisplayRole)
    {
        //this only does top (horizontal) header
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case 0: returnValue = QString("Name");
                break;
            case 1: returnValue = QString("Strokes");
                break;
            case 2: returnValue = QString(QChar(0x00D7));
                break;
            case 3: returnValue = QString("Total");
                break;
            case 4: returnValue = QString("Beats");
                break;
            case 5: returnValue = QString("Total beats");
                break;
            default: returnValue = section;
            }
        }
    }

    return returnValue;
}

Qt::ItemFlags PatternDataModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant PatternDataModel::data ( const QModelIndex & index, int role ) const
{
    QVariant returnValue = QVariant();

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
            case 0: returnValue = beatPatterns[index.row()].name();
                break;
            case 1: returnValue = beatPatterns.at(index.row()).lengthInStrokes();
                break;
            case 2: returnValue = beatPatterns.at(index.row()).repetitions();
                break;
            case 3: returnValue = beatPatterns.at(index.row()).totalStrokesCovered();
                break;
            case 4: if (beatPatterns.at(index.row()).containsUnknownIntervals())
                    returnValue = QString("?");
                else
                    returnValue = beatPatterns.at(index.row()).lengthInBeats();
                break;
            case 5: if (beatPatterns.at(index.row()).containsUnknownIntervals())
                    returnValue = QString("?");
                else
                    returnValue = beatPatterns.at(index.row()).totalBeatsCovered();
                break;
            default:
                break;
        }
    }

    if (role == Qt::BackgroundRole)
    {
        if (beatPatterns.at(index.row()).containsUnknownIntervals())
            return QBrush(QColor("Dark Red"));
            //stop processing and return to avoid weird colour values

        if (index.column() < 4)
        {
            float difficulty = (float)beatPatterns.at(index.row()).lengthInStrokes() / beatPatterns.at(index.row()).lengthInBeats();
            int hue = 0;
            float minimumDifficulty = 1 / BeatAnalysis::Configuration::minNumberOfBeatsToQualifyAsBreak;
            if (difficulty < minimumDifficulty)
                hue = 240;
            else if (difficulty < 1)
                hue = 240 - ((difficulty - minimumDifficulty) * (120 / (1 - minimumDifficulty)));
            else
                hue = 120 / (difficulty * (4.0/3.0));

            returnValue = QBrush(QColor::fromHsl(hue,255,191));
        }
        if (index.column() >= 4 &&
                beatPatterns.at(index.row()).lengthInStrokes() > 1)
        {
            if ( ! isPowerOfTwo(roundToInt(beatPatterns.at(index.row()).lengthInBeats())))
                returnValue = QBrush(QColor("Yellow"));
            if ( ! isWholeNumber(beatPatterns.at(index.row()).lengthInBeats()))
                returnValue = QBrush(QColor("Dark Red"));
        }
    }

    if (role == Qt::ForegroundRole)
    {
        if (beatPatterns.at(index.row()).containsUnknownIntervals())
            returnValue = QBrush(QColor("White"));
        if (index.column() >= 4 &&
            beatPatterns.at(index.row()).lengthInStrokes() > 1 &&
            ! isWholeNumber(beatPatterns.at(index.row()).lengthInBeats()))
                returnValue = QBrush(QColor("White"));
    }

    return returnValue;
}
