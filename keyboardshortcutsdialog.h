#ifndef KEYBOARDSHORTCUTSDIALOG_H
#define KEYBOARDSHORTCUTSDIALOG_H

#include <QDialog>
class QKeySequenceEdit;

namespace Ui {
class KeyboardShortcutsDialog;
}

class KeyboardShortcutsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeyboardShortcutsDialog(QWidget *parent = 0);
    ~KeyboardShortcutsDialog();
    void applyShortcutPrefsFromUi();
    static void applyActionShortcutsFromPrefs();
    void accept();

public slots:
    void on_addCustomShortcutButtonClicked();

private:
    Ui::KeyboardShortcutsDialog *ui;
    QList<QAction *> * allActions;
    QList<QKeySequenceEdit *> allPrimaryShortcuts;
    QList<QKeySequenceEdit *> allSecondaryShortcuts;
    static QList<QAction *> *createActionsList();
    void createWidgetList();
    static QString getActionID(QAction * action);
    static QString getActionContext(QAction * action);
    static QString getActionPrimaryShortcutID(QAction * action);
    static QString getActionSecondaryShortcutID(QAction * action);
};

#endif // KEYBOARDSHORTCUTSDIALOG_H
