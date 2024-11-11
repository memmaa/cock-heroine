#include "exportbeatmeterdialog.h"
#include "ui_exportbeatmeterdialog.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include "event.h"
#include "optionsdialog.h"
#include <QFileDialog>

ExportBeatMeterDialog::ExportBeatMeterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportBeatMeterDialog),
    scene(new QGraphicsScene(this))
{
    ui->setupUi(this);
    ui->strokeMeter->setScene(scene);
}

ExportBeatMeterDialog::~ExportBeatMeterDialog()
{
    delete ui;
}

void ExportBeatMeterDialog::on_updateButton_clicked()
{
    updateScene();
}

void ExportBeatMeterDialog::updateScene()
{
    while (!strokeMarkers.isEmpty())
    {
        QAbstractGraphicsShapeItem * removeMe = strokeMarkers.last();
        scene->removeItem(removeMe);
        strokeMarkers.removeLast();
        delete removeMe;
    }

    int markerHeight = OptionsDialog::getBeatMarkerHeight();
    int markerWidth = OptionsDialog::getBeatMarkerWidth();
    for (Event event : events) {
        int hOffset = ((event.timestamp * OptionsDialog::getBeatMeterSpeed()) / 1000) - markerWidth / 2;//offset by half the width so the markers are centered horizontally on their timestamps
        int vOffset = 0 - (markerHeight / 2); //offset by half the height so the markers are centered vertically on 0
        QRectF rectLocation(hOffset,vOffset,markerWidth,markerHeight);
        QAbstractGraphicsShapeItem * marker;
        if (OptionsDialog::useSquareBeatMarkers())
            marker = new QGraphicsRectItem(rectLocation);
        else
            marker = new QGraphicsEllipseItem(rectLocation);
        QPen pen;
        pen.setWidth(2);
        marker->setPen(pen);
        QColor color = getColor(event);
        QBrush brush(color);
        marker->setBrush(brush);
        strokeMarkers.append(marker);
        scene->addItem(marker);
    }

    int width = OptionsDialog::getBeatMeterWidth();
    QRectF bounds = scene->itemsBoundingRect();
    bounds.setLeft(bounds.left() - width);
    bounds.setRight(bounds.right() + width);
    scene->setSceneRect(bounds);
    scene->update();
}

QColor ExportBeatMeterDialog::getColor(Event)
{
    //TODO: Implement some fun colouring here
    return QColor(Qt::lightGray);
}

void ExportBeatMeterDialog::on_exportButton_clicked()
{
    QString exportFilenamePrefix = QFileDialog::getSaveFileName(this, tr("Select image sequence base filename"),
                                                "~/Pictures",
                                                tr("Portable Network Graphics (*.png)"));
    if (exportFilenamePrefix.isEmpty())
        return; //save cancelled
    if (exportFilenamePrefix.endsWith(".png"))
    {
        exportFilenamePrefix.chop(4);
    }
    int height = OptionsDialog::getBeatMeterHeight();
    int width = OptionsDialog::getBeatMeterWidth();
    int pixelsPerSecond = OptionsDialog::getBeatMeterSpeed();
    double fps = OptionsDialog::getBeatMeterFrameRate();
    long frameCounter = 0;
    qreal beginning = scene->itemsBoundingRect().left() - width;
    qreal end = scene->itemsBoundingRect().right();
    qreal top = 0 - (float) height / 2;
    qreal hPos = beginning;

    while (hPos < end)
    {
        ++frameCounter;
        hPos = beginning + (frameCounter * pixelsPerSecond) / fps;
        scene->setSceneRect(QRectF(hPos, top, width, height));
        QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
        image.fill(Qt::transparent);                                              // Start all pixels transparent

        QPainter painter(&image);
        scene->render(&painter);
        image.save(QString("%1_%2.png").arg(exportFilenamePrefix).arg(frameCounter));
    }
}
