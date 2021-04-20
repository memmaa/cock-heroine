#ifndef GRAPHICSSCENEVIDEODIALOG_H
#define GRAPHICSSCENEVIDEODIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>

namespace Ui {
class GraphicsSceneVideoDialog;
}

class GraphicsSceneVideoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GraphicsSceneVideoDialog(QWidget *parent = 0);
    ~GraphicsSceneVideoDialog();
    void takeVideoOutput();
    QGraphicsScene * videoScene;
    QGraphicsVideoItem * videoItem;
    void closeEvent(QCloseEvent *);
    void resizeEvent(QResizeEvent*);
    void handleResize();
    void mousePressEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    bool eventFilter(QObject *obj, QEvent *event);
    void toggleFullscreen();
    void show();

public slots:
    void maskBeatMeter(bool dark = false);
    void setMaskOrNot (bool);

private:
    Ui::GraphicsSceneVideoDialog *ui;
    QGraphicsRectItem * maskingRectangle;
    QGraphicsView * graphicsView;
};

#endif // GRAPHICSSCENEVIDEODIALOG_H
