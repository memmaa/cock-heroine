#include "valuedatamodel.h"
#include <QSize>
#include "globals.h"
#include "beatinterval.h"
#include "beatanalysis.h"
#include <QBrush>

ValueDataModel::ValueDataModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int ValueDataModel::rowCount ( const QModelIndex &) const
{
    return beatValues.size();
}

int ValueDataModel::columnCount ( const QModelIndex & parent) const
{
    if (parent != QModelIndex())
        return 0;
    else
        return 7;
}

QVariant ValueDataModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            returnValue = QSize(18,20);
            break;
        case 1:
            returnValue = QSize(100,20);
            break;
        case 2:
            returnValue = QSize(20,20);
            break;
        case 3:
            returnValue = QSize(8,20);
            break;
        case 4:
            returnValue = QSize(20,20);
            break;
        case 5:
            returnValue = QSize(54,20);
            break;
        case 6:
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
            case 0: returnValue = QString(""); //just a checkbox
                break;
            case 1: returnValue = QString("Name");
                break;
            case 2: returnValue = QString("Va");
                break;
            case 3: returnValue = QString("l");
                break;
            case 4: returnValue = QString("ue");
                break;
            case 5: returnValue = QString("Matches");
                break;
            case 6: returnValue = QString("Beats");
                break;
            default: returnValue = section;
            }
        }
    }

    return returnValue;
}

Qt::ItemFlags ValueDataModel::flags(const QModelIndex & index) const
{
    Qt::ItemFlags returnValue = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == 0)
        returnValue |= Qt::ItemIsUserCheckable;
    if (index.column() == 1)
        returnValue |= Qt::ItemIsEditable;
    if (beatValues[index.row()].preset == false &&
            (index.column() == 2 ||
             index.column() == 4 ) )
        returnValue |= Qt::ItemIsEditable;

    return returnValue;
}

QVariant ValueDataModel::data ( const QModelIndex & index, int role ) const
{
    QVariant returnValue = QVariant();

    if (counts.size() <= index.row())
        counts.resize(index.row()+1);
        //recalculateCounts(); //can't because const :-/

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
            case 0: if (beatValues[index.row()].active)
                        returnValue = QString("o"); //just a checkbox
                    else
                        returnValue = QString("x"); //just a checkbox
                break;
            case 1: returnValue = beatValues[index.row()].name;
                break;
            case 2: returnValue = beatValues.at(index.row()).numerator;
                break;
            case 3: returnValue = QString('/');
                break;
            case 4: returnValue = beatValues.at(index.row()).denominator;
                break;
            case 5: returnValue = counts.at(index.row());
                break;
            case 6: returnValue = beatValues.at(index.row()).value();
                break;
            default:
                break;
        }
    }

    if (role == Qt::CheckStateRole && index.column() == 0)  // this shows the checkbox
    {
        if (beatValues[index.row()].active)
            return Qt::Checked;
        else
            return Qt::Unchecked;
    }

    if (role == Qt::ForegroundRole)
    {
        if (/*index.column() == 3 ||*/
                (beatValues[index.row()].preset &&
                  (index.column() == 2 ||
                   index.column() == 3 ||
                   index.column() == 4)))
            returnValue = QBrush(QColor("darkGray"));
    }

    if (role == Qt::BackgroundRole)
    {
        if (counts[index.row()] > 0 &&
                beatValues[index.row()].value() < BeatAnalysis::Configuration::minNumberOfBeatsToQualifyAsBreak &&
                ( counts[index.row()] <= 4 ||
                  (float)counts[index.row()] / (float)beatIntervals.size() < 0.01) )
        {
            int lum = 255, proportionLum = 128, quantityLum = 127;
            proportionLum += ((float)counts[index.row()] / (float)beatIntervals.size()) * 12700;
            quantityLum += 32 * (counts[index.row()] - 1);
            lum = proportionLum < quantityLum ?
                        proportionLum :
                        quantityLum;
            returnValue = QBrush(QColor::fromHsl(0,255,lum));
        }
    }

    return returnValue;
}

bool ValueDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && index.column() == 0)
    {
        beatValues[index.row()].active = value.toBool();
        emit dataChanged(index,index);
        return true;
    }

    if (role == Qt::EditRole)
    {
        switch (index.column())
        {
        case 1:
            beatValues[index.row()].name = value.toString();
            break;
        case 2:
            beatValues[index.row()].numerator = value.toInt();
            break;
        case 4:
            beatValues[index.row()].denominator = value.toInt();
            break;
        default:
            return false;
        }
        emit dataChanged(index,index);
    }



    return false;
}

void ValueDataModel::addValue(const BeatValue & valueToAdd)
{
    beginInsertRows(QModelIndex(),rowCount(),rowCount());
    beatValues.append(valueToAdd);
    endInsertRows();
}

bool ValueDataModel::removeRow (int row)
{
    beginRemoveRows(QModelIndex(),row,row);
    beatValues.removeAt(row);
    endRemoveRows();
    return true;
}

void ValueDataModel::recalculateCounts()
{
    counts.resize(beatValues.size());
    int i = 0, j = 0;
    for (i = 0; i < beatValues.size(); ++i)
    {
        counts[i] = 0;
        for (j = 0; j < beatIntervals.size(); ++j)
        {
            if (beatIntervals[j].getValue() == &beatValues[i])
                ++counts[i];
        }
    }
    emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
}
