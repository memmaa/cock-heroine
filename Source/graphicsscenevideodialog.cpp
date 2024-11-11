#include "graphicsscenevideodialog.h"
#include "ui_graphicsscenevideodialog.h"
#include "globals.h"
#include <QMediaPlayer>
#include "mainwindow.h"
#include <QKeyEvent>
#include <QtWidgets>

GraphicsSceneVideoDialog::GraphicsSceneVideoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphicsSceneVideoDialog),
    maskingRectangle(NULL)
{
    ui->setupUi(this);
    graphicsView = ui->graphicsView;
    graphicsView->installEventFilter(this);
    videoScene = new QGraphicsScene(this);
    ui->graphicsView->setScene(videoScene);
    videoItem = new QGraphicsVideoItem();
    videoScene->addItem(videoItem);
    maskBeatMeter();
    connect(mainWindow,SIGNAL(obscureMeterToggled(bool)),this,SLOT(setMaskOrNot(bool)));
}

GraphicsSceneVideoDialog::~GraphicsSceneVideoDialog()
{
    delete ui;
}

void GraphicsSceneVideoDialog::takeVideoOutput()
{
    videoPlayer->stop();
    videoPlayer->setVideoOutput(videoItem);
    //videoItem->setSize(videoItem->nativeSize());
    videoPlayer->pause();
    handleResize();
}

void GraphicsSceneVideoDialog::closeEvent(QCloseEvent *)
{
    mainWindow->takeVideoOutput();
}

void GraphicsSceneVideoDialog::resizeEvent(QResizeEvent *)
{
    handleResize();
}

void GraphicsSceneVideoDialog::handleResize()
{
    if (graphicsView->parent() == NULL)
    {
        const QRect screenGeometry = QApplication::desktop()->screenGeometry(this);
        graphicsView->setFrameRect(screenGeometry);
        graphicsView->resize(QApplication::desktop()->size());
    }
    videoItem->setSize(graphicsView->size());
    videoScene->setSceneRect(videoItem->boundingRect());
    maskBeatMeter();
}

void GraphicsSceneVideoDialog::show()
{
    //videoItem->setSize(videoItem->nativeSize());
    QDialog::show();
    maskBeatMeter();
}

void GraphicsSceneVideoDialog::maskBeatMeter(bool dark)
{
    QSettings settings;
    double heightPercentage = settings.value("Beat Meter Width").toDouble();

    qreal width = videoScene->width();
    qreal totalHeight = videoScene->height();
    qreal height = (totalHeight * heightPercentage) / 100.0;
    //QGraphicsRectItem * oldRect = NULL;
    if (maskingRectangle != NULL)
    {
        videoScene->removeItem(maskingRectangle);
        //delete maskingRectangle;
        //oldRect = maskingRectangle;
    }

    if (settings.value("Obscure Beat Meter").toBool())
    {
        QColor color = dark ? QColor(Qt::black) : QColor(Qt::white);
        maskingRectangle = new QGraphicsRectItem(videoItem->boundingRect().left(),(videoItem->boundingRect().top() + totalHeight) - height,width,height,videoItem);
        QPen pen(color);
        pen.setWidth(1);
        maskingRectangle->setPen(pen);
        QBrush brush(color);
        maskingRectangle->setBrush(brush);
        maskingRectangle->setRect(videoItem->boundingRect().left(),(videoItem->boundingRect().top() + totalHeight) - height,width,height);
    }

    /*if (oldRect != NULL)
    {
        delete oldRect;
    }*/
}

void GraphicsSceneVideoDialog::setMaskOrNot(bool) {
    maskBeatMeter();
}

void GraphicsSceneVideoDialog::mousePressEvent(QMouseEvent *)
{
    toggleFullscreen();
}

bool GraphicsSceneVideoDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_F ||
                keyEvent->key() == Qt::Key_Escape ||
                keyEvent->key() == Qt::Key_Space ||
                keyEvent->key() == Qt::Key_Left ||
                keyEvent->key() == Qt::Key_Right )
        {
            keyPressEvent(keyEvent);
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

void GraphicsSceneVideoDialog::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_F ||
            (event->key() == Qt::Key_Escape && graphicsView->isFullScreen()))
    {
        toggleFullscreen();
    }
    else if (event->key() == Qt::Key_Space)
    {
        mainWindow->pressPlay();
    }
    else if (event->key() == Qt::Key_Left)
    {
        mainWindow->skipBackward();
    }
    else if (event->key() == Qt::Key_Right)
    {
        mainWindow->skipForward();
    }
}

void GraphicsSceneVideoDialog::toggleFullscreen()
{
    if (graphicsView->isFullScreen())
    {
        ui->verticalLayout->addWidget(graphicsView);
        //graphicsView->setParent(ui->verticalLayout);
        graphicsView->showNormal();
        handleResize();
        while (QApplication::overrideCursor())
            QApplication::restoreOverrideCursor();
    }
    else
    {
        graphicsView->setParent(NULL);
        graphicsView->showFullScreen();
        handleResize();
        maskBeatMeter(true);
        QApplication::setOverrideCursor(Qt::BlankCursor);
        if (!currentlyPlaying)
            mainWindow->pressPlay();
    }
}
