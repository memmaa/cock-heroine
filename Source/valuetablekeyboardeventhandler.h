#ifndef VALUETABLEKEYBOARDEVENTHANDLER_H
#define VALUETABLEKEYBOARDEVENTHANDLER_H

#include <QKeyEvent>

class ValueTableKeyboardEventHandler : public QObject
{
    Q_OBJECT
public:
    explicit ValueTableKeyboardEventHandler(QObject *parent = 0);
    bool eventFilter(QObject *obj, QEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:

public slots:

};

#endif // VALUETABLEKEYBOARDEVENTHANDLER_H
