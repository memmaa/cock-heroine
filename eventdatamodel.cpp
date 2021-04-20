#include "eventdatamodel.h"
#include "globals.h"
#include "event.h"
#include <QSize>

eventDataModel::eventDataModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int eventDataModel::rowCount ( const QModelIndex &) const
{
    return events.size();
}

int eventDataModel::columnCount ( const QModelIndex & parent) const
{
    if (parent != QModelIndex())
        return 0;
    else
        return 5;
}

QVariant eventDataModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            returnValue = QSize(40,20);
            break;
        case 1:
            returnValue = QSize(130,20);
            break;
        case 2:
            returnValue = QSize(40,20);
            break;
        case 3:
            returnValue = QSize(65,20);
            break;
        case 4:
            returnValue = QSize(40,20);
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
            case 0: returnValue = QString("][-->|");
                break;
            case 1: returnValue = QString("Timestamp");
                break;
            case 2: returnValue = QString("Type");
                break;
            case 3: returnValue = QString("Value");
                break;
            case 4: returnValue = QString("Optional");
                break;
            default: returnValue = section;
            }
        }
    }

    return returnValue;
}

Qt::ItemFlags eventDataModel::flags(const QModelIndex &index) const
{
    switch (index.column())
    {
    case 0:
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        break;
    case 1:
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable	| Qt::ItemIsEditable;
        break;
    case 2:
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable	| Qt::ItemIsEditable;
        break;
    case 3:
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable	| Qt::ItemIsEditable;
        break;
    case 4:
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable	| Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
        break;
    default:
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}

QVariant eventDataModel::data ( const QModelIndex & index, int role ) const
{
    QVariant returnValue = QVariant();

    if (role == Qt::CheckStateRole && index.column() == 4)  // this shows the checkbox
    {
        if (events.at(index.row()).isOptional())
            return Qt::Checked;
        else
            return Qt::Unchecked;
    }
    if (role == Qt::DisplayRole)
    {
        QString retStr;
        switch (index.column())
        {
            case 0:
                if (!events.at(index.row()).metadata)
                {
                    retStr += "X";
                    returnValue = retStr;
                    break;
                }
                if (events.at(index.row()).metadata->endsRound)
                    retStr += "]";
                if (events.at(index.row()).metadata->startsNewRound)
                    retStr += "[";
                if (events.at(index.row()).metadata->startsNewPattern)
                    retStr += "-->";
                if (events.at(index.row()).metadata->tempo == 0)
                {
                    returnValue = retStr;
                    break;
                }
                if (events.at(index.row()).metadata->valueNumerator == 0)
                {
                    retStr += "?";
                    //retStr += "X";
                }
                if (events.at(index.row()).metadata->patternIndex == 0)
                    retStr += "|";
                if (events.at(index.row()).metadata->startsNewPattern)
                    retStr += events.at(index.row()).metadata->patternName;
                returnValue = retStr;
                break;
            case 1: returnValue = millisToTimecode(events.at(index.row()).timestamp);
                break;
            case 2: returnValue = events.at(index.row()).type();
                break;
            case 3: returnValue = events.at(index.row()).value;
                break;
            case 4: //returnValue = events.at(index.row()).isOptional(); //no longer needed as we have a checkbox :-)
                break;
            default:
                break;
        }
    }
    if (role == Qt::EditRole)
    {
        switch (index.column())
        {
            //column 0 not editable
            case 1: returnValue = int(events.at(index.row()).timestamp);
                break;
            case 2: returnValue = events.at(index.row()).type();
                break;
            case 3: returnValue = events.at(index.row()).value;
                break;
            case 4: //returnValue = events.at(index.row()).isOptional(); //no longer needed as we have a checkbox :-)
                break;
            default:
                break;
        }
    }

    return returnValue;
}

bool eventDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && index.column() == 4)
    {
        events[index.row()].setOptional(value.toBool());
    }

    if (role == Qt::EditRole)
    {
        switch (index.column())
        {
        //column 0 not editable
        case 1:
            events[index.row()].timestamp = value.toLongLong();
            break;
        case 2:
            events[index.row()].setType(value.toUInt());
            break;
        case 3:
            events[index.row()].value = value.toInt();
            break;
        case 4:
            //events[index.row()].setOptional(value.toBool()); //this is now handled by CheckStateRole
            break;
        default:
            return false;
        }
    }

    emit dataChanged(index,index);

    return true;
}

Event eventDataModel::eventFromIndex(const QModelIndex &index)
{
    return events.at(index.row());
}

void eventDataModel::setEventAtIndex(const QModelIndex & indexToChange, const Event & event)
{
    int rowIndex = indexToChange.row();
    setData(index(rowIndex,1),QVariant::fromValue((qlonglong)event.timestamp),Qt::EditRole);
    setData(index(rowIndex,2),QVariant(event.type()),Qt::EditRole);
    setData(index(rowIndex,3),QVariant(event.value),Qt::EditRole);
    setData(index(rowIndex,4),QVariant(event.isOptional()),Qt::CheckStateRole);
    if (event.metadata)
    {
        events[rowIndex].metadata = event.metadata;
    }
}

void eventDataModel::addEvent(const Event eventToAdd)
{
    int i = 0;
    for ( ; i < events.size(); ++i)
        if (events[i].timestamp > eventToAdd.timestamp)
            break;
    beginInsertRows(QModelIndex(),i,i);
    events.insert(i,eventToAdd);
    endInsertRows();
}

void eventDataModel::removeEvent(int index)
{
    beginRemoveRows(QModelIndex(),index,index);
    events.removeAt(index);
    endRemoveRows();
}

//bool eventDataModel::insertRows(int row, int count, const QModelIndex &parent)
//{
//    bool success = true;

//    beginInsertRows(parent,mainPack.cardsInPack(),mainPack.cardsInPack()+count-1);
//    for (int i = 0;i<count;i++)
//    {
//        flashCard * card = new flashCard;
//        success = success &
//                mainPack.addCard(*card,level_norm);
//        cardAdded(card);
//    }
//    endInsertRows();
//    return success;
//}

bool eventDataModel::removeRows (int row,int count,const QModelIndex &)
{
    for (int i = 0; i < count; ++i)
    {
        removeEvent(row);
    }
    return true;
}
