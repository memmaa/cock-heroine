#ifndef PLAYBACKASSOCIATEDACTION_H
#define PLAYBACKASSOCIATEDACTION_H

#include <QObject>

class PlaybackAssociatedAction : public QObject
{
    Q_OBJECT
public:
    explicit PlaybackAssociatedAction(QObject *parent = nullptr);

signals:

};

#endif // PLAYBACKASSOCIATEDACTION_H
