#ifndef EXPORTBEATMETERDIALOG_H
#define EXPORTBEATMETERDIALOG_H

#include <QDialog>
#include "globals.h"

class QGraphicsScene;
class QAbstractGraphicsShapeItem;

namespace Ui {
class ExportBeatMeterDialog;
}

class ExportBeatMeterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportBeatMeterDialog(QWidget *parent = nullptr);
    ~ExportBeatMeterDialog();

private slots:
    void on_updateButton_clicked();

    void on_exportButton_clicked();

private:
    Ui::ExportBeatMeterDialog *ui;
    QGraphicsScene * scene;
    QVector<QAbstractGraphicsShapeItem *> strokeMarkers;
    QColor getColor(Event event);

    void updateScene();
};

#endif // EXPORTBEATMETERDIALOG_H
