#include "valuetablekeyboardeventhandler.h"
#include "globals.h"
#include "editorwindow.h"
#include "ui_editorwindow.h"
#include "valuedatamodel.h"

ValueTableKeyboardEventHandler::ValueTableKeyboardEventHandler(QObject *parent) :
    QObject(parent)
{
}

bool ValueTableKeyboardEventHandler::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Delete)
        {
            keyPressEvent(keyEvent);
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

void ValueTableKeyboardEventHandler::keyPressEvent(QKeyEvent *event)
{
     if ( event->key() == Qt::Key_Delete )
         editor->valueModel->removeRow(editor->ui->beatValuesTable->selectionModel()->selectedRows(1).first().row());
     else
         event->ignore();
}
