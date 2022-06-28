#include "vibratorpulsefeatureparams.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSpacerItem>
#include "mainwindow.h"
#include "helperfunctions.h"
#include <QSettings>

QHash<ButtplugDeviceFeature *, QTimer *> VibratorPulseFeatureParams::endTimers;

VibratorPulseFeatureParams::VibratorPulseFeatureParams(QObject *parent)
    :
    ButtplugFeatureParams(parent, ButtplugDeviceFeatureType::VibratorMotor, ButtplugDeviceParameter::VibratorPulse)
{
    //nothing yet...
}

VibratorPulseFeatureParams::VibratorPulseFeatureParams(QObject *parent, ButtplugDeviceFeature *feature)
    :
    ButtplugFeatureParams(parent, feature, ButtplugDeviceParameter::VibratorPulse)
{
    //Also nothing
}

void VibratorPulseFeatureParams::createUi(QWidget * parentWidget, QBoxLayout *parentLayout)
{
    QLabel *startLabel;
    QGridLayout *startParamsGridLayout;
    QLabel *startIntensityLabel;
    QGridLayout *startIntensityGridLayout;
    QLabel *startIntensityLowLabel;
    QLabel *startIntensityHighLabel;
    QLabel *startDurationLabel;
    QGridLayout *startDurationGridLayout;
    QLabel *startDurationShortLabel;
    QLabel *startDurationLongLabel;
    QLabel *endLabel;
    QGridLayout *endParamsGridLayout;
    QLabel *endIntensityLabel;
    QGridLayout *endIntensityGridLayout;
    QLabel *endIntensityLowLabel;
    QLabel *endIntensityHighLabel;
    QLabel *endDurationLabel;
    QGridLayout *endDurationGridLayout;
    QLabel *endDurationShortLabel;
    QLabel *endDurationLongLabel;

    ButtplugFeatureParams::createUi(parentWidget, parentLayout);
    startLabel = new QLabel(parentWidget);
    startLabel->setObjectName(QString::fromUtf8("startLabel"));

    layout->addWidget(startLabel);

    startParamsGridLayout = new QGridLayout();
    startParamsGridLayout->setObjectName(QString::fromUtf8("startParamsGridLayout"));
    startIntensityLabel = new QLabel(parentWidget);
    startIntensityLabel->setObjectName(QString::fromUtf8("startIntensityLabel"));

    startParamsGridLayout->addWidget(startIntensityLabel, 0, 0, 1, 1);

    startIntensityGridLayout = new QGridLayout();
    startIntensityGridLayout->setObjectName(QString::fromUtf8("startIntensityGridLayout"));
    startIntensityHighSlider = new QSlider(parentWidget);
    startIntensityHighSlider->setObjectName(QString::fromUtf8("startIntensityHighSlider"));
    startIntensityHighSlider->setMaximum(10000);
    startIntensityHighSlider->setSingleStep(100);
    startIntensityHighSlider->setPageStep(1000);
    startIntensityHighSlider->setValue(7500);
    startIntensityHighSlider->setOrientation(Qt::Horizontal);

    startIntensityGridLayout->addWidget(startIntensityHighSlider, 0, 1, 1, 1);

    startIntensityLowLabel = new QLabel(parentWidget);
    startIntensityLowLabel->setObjectName(QString::fromUtf8("startIntensityLowLabel"));

    startIntensityGridLayout->addWidget(startIntensityLowLabel, 1, 0, 1, 1);

    startIntensityHighSpinBox = new QDoubleSpinBox(parentWidget);
    startIntensityHighSpinBox->setObjectName(QString::fromUtf8("startIntensityHighSpinBox"));
    startIntensityHighSpinBox->setMaximum(100.000000000000000);
    startIntensityHighSpinBox->setValue(75.000000000000000);

    startIntensityGridLayout->addWidget(startIntensityHighSpinBox, 0, 2, 1, 1);

    startIntensityHighLabel = new QLabel(parentWidget);
    startIntensityHighLabel->setObjectName(QString::fromUtf8("startIntensityHighLabel"));

    startIntensityGridLayout->addWidget(startIntensityHighLabel, 0, 0, 1, 1);

    startIntensityLowSpinBox = new QDoubleSpinBox(parentWidget);
    startIntensityLowSpinBox->setObjectName(QString::fromUtf8("startIntensityLowSpinBox"));
    startIntensityLowSpinBox->setMaximum(100.000000000000000);
    startIntensityLowSpinBox->setValue(5.000000000000000);

    startIntensityGridLayout->addWidget(startIntensityLowSpinBox, 1, 2, 1, 1);

    startIntensityLowSlider = new QSlider(parentWidget);
    startIntensityLowSlider->setObjectName(QString::fromUtf8("startIntensityLowSlider"));
    startIntensityLowSlider->setMaximum(10000);
    startIntensityLowSlider->setSingleStep(100);
    startIntensityLowSlider->setPageStep(1000);
    startIntensityLowSlider->setValue(500);
    startIntensityLowSlider->setOrientation(Qt::Horizontal);

    startIntensityGridLayout->addWidget(startIntensityLowSlider, 1, 1, 1, 1);


    startParamsGridLayout->addLayout(startIntensityGridLayout, 0, 1, 1, 1);

    startDurationLabel = new QLabel(parentWidget);
    startDurationLabel->setObjectName(QString::fromUtf8("startDurationLabel"));

    startParamsGridLayout->addWidget(startDurationLabel, 1, 0, 1, 1);

    startDurationGridLayout = new QGridLayout();
    startDurationGridLayout->setObjectName(QString::fromUtf8("startDurationGridLayout"));
    startDurationShortSlider = new QSlider(parentWidget);
    startDurationShortSlider->setObjectName(QString::fromUtf8("startDurationShortSlider"));
    startDurationShortSlider->setMinimum(1);
    startDurationShortSlider->setMaximum(500);
    startDurationShortSlider->setValue(100);
    startDurationShortSlider->setOrientation(Qt::Horizontal);

    startDurationGridLayout->addWidget(startDurationShortSlider, 0, 1, 1, 1);

    startDurationShortLabel = new QLabel(parentWidget);
    startDurationShortLabel->setObjectName(QString::fromUtf8("startDurationShortLabel"));

    startDurationGridLayout->addWidget(startDurationShortLabel, 0, 0, 1, 1);

    startDurationLongLabel = new QLabel(parentWidget);
    startDurationLongLabel->setObjectName(QString::fromUtf8("startDurationLongLabel"));

    startDurationGridLayout->addWidget(startDurationLongLabel, 1, 0, 1, 1);

    startDurationShortSpinBox = new QSpinBox(parentWidget);
    startDurationShortSpinBox->setObjectName(QString::fromUtf8("startDurationShortSpinBox"));
    startDurationShortSpinBox->setMinimum(1);
    startDurationShortSpinBox->setMaximum(500);
    startDurationShortSpinBox->setValue(100);

    startDurationGridLayout->addWidget(startDurationShortSpinBox, 0, 2, 1, 1);

    startDurationLongSpinBox = new QSpinBox(parentWidget);
    startDurationLongSpinBox->setObjectName(QString::fromUtf8("startDurationLongSpinBox"));
    startDurationLongSpinBox->setMinimum(1);
    startDurationLongSpinBox->setMaximum(500);
    startDurationLongSpinBox->setValue(200);

    startDurationGridLayout->addWidget(startDurationLongSpinBox, 1, 2, 1, 1);

    startDurationLongSlider = new QSlider(parentWidget);
    startDurationLongSlider->setObjectName(QString::fromUtf8("startDurationLongSlider"));
    startDurationLongSlider->setMinimum(1);
    startDurationLongSlider->setMaximum(500);
    startDurationLongSlider->setValue(200);
    startDurationLongSlider->setOrientation(Qt::Horizontal);

    startDurationGridLayout->addWidget(startDurationLongSlider, 1, 1, 1, 1);


    startParamsGridLayout->addLayout(startDurationGridLayout, 1, 1, 1, 1);


    layout->addLayout(startParamsGridLayout);

    endLabel = new QLabel(parentWidget);
    endLabel->setObjectName(QString::fromUtf8("endLabel"));

    layout->addWidget(endLabel);

    endParamsGridLayout = new QGridLayout();
    endParamsGridLayout->setObjectName(QString::fromUtf8("endParamsGridLayout"));
    endIntensityLabel = new QLabel(parentWidget);
    endIntensityLabel->setObjectName(QString::fromUtf8("endIntensityLabel"));

    endParamsGridLayout->addWidget(endIntensityLabel, 0, 0, 1, 1);

    endIntensityGridLayout = new QGridLayout();
    endIntensityGridLayout->setObjectName(QString::fromUtf8("endIntensityGridLayout"));
    endIntensityHighSlider = new QSlider(parentWidget);
    endIntensityHighSlider->setObjectName(QString::fromUtf8("endIntensityHighSlider"));
    endIntensityHighSlider->setMaximum(10000);
    endIntensityHighSlider->setSingleStep(100);
    endIntensityHighSlider->setPageStep(1000);
    endIntensityHighSlider->setValue(10000);
    endIntensityHighSlider->setOrientation(Qt::Horizontal);

    endIntensityGridLayout->addWidget(endIntensityHighSlider, 0, 1, 1, 1);

    endIntensityLowLabel = new QLabel(parentWidget);
    endIntensityLowLabel->setObjectName(QString::fromUtf8("endIntensityLowLabel"));

    endIntensityGridLayout->addWidget(endIntensityLowLabel, 1, 0, 1, 1);

    endIntensityHighSpinBox = new QDoubleSpinBox(parentWidget);
    endIntensityHighSpinBox->setObjectName(QString::fromUtf8("endIntensityHighSpinBox"));
    endIntensityHighSpinBox->setMaximum(100.000000000000000);
    endIntensityHighSpinBox->setValue(100.000000000000000);

    endIntensityGridLayout->addWidget(endIntensityHighSpinBox, 0, 2, 1, 1);

    endIntensityHighLabel = new QLabel(parentWidget);
    endIntensityHighLabel->setObjectName(QString::fromUtf8("endIntensityHighLabel"));

    endIntensityGridLayout->addWidget(endIntensityHighLabel, 0, 0, 1, 1);

    endIntensityLowSpinBox = new QDoubleSpinBox(parentWidget);
    endIntensityLowSpinBox->setObjectName(QString::fromUtf8("endIntensityLowSpinBox"));
    endIntensityLowSpinBox->setMaximum(100.000000000000000);
    endIntensityLowSpinBox->setValue(50.000000000000000);

    endIntensityGridLayout->addWidget(endIntensityLowSpinBox, 1, 2, 1, 1);

    endIntensityLowSlider = new QSlider(parentWidget);
    endIntensityLowSlider->setObjectName(QString::fromUtf8("endIntensityLowSlider"));
    endIntensityLowSlider->setMaximum(10000);
    endIntensityLowSlider->setSingleStep(100);
    endIntensityLowSlider->setPageStep(1000);
    endIntensityLowSlider->setValue(5000);
    endIntensityLowSlider->setOrientation(Qt::Horizontal);

    endIntensityGridLayout->addWidget(endIntensityLowSlider, 1, 1, 1, 1);


    endParamsGridLayout->addLayout(endIntensityGridLayout, 0, 1, 1, 1);

    endDurationLabel = new QLabel(parentWidget);
    endDurationLabel->setObjectName(QString::fromUtf8("endDurationLabel"));

    endParamsGridLayout->addWidget(endDurationLabel, 1, 0, 1, 1);

    endDurationGridLayout = new QGridLayout();
    endDurationGridLayout->setObjectName(QString::fromUtf8("endDurationGridLayout"));
    endDurationShortSlider = new QSlider(parentWidget);
    endDurationShortSlider->setObjectName(QString::fromUtf8("endDurationShortSlider"));
    endDurationShortSlider->setMinimum(1);
    endDurationShortSlider->setMaximum(500);
    endDurationShortSlider->setValue(150);
    endDurationShortSlider->setOrientation(Qt::Horizontal);

    endDurationGridLayout->addWidget(endDurationShortSlider, 0, 1, 1, 1);

    endDurationShortLabel = new QLabel(parentWidget);
    endDurationShortLabel->setObjectName(QString::fromUtf8("endDurationShortLabel"));

    endDurationGridLayout->addWidget(endDurationShortLabel, 0, 0, 1, 1);

    endDurationLongLabel = new QLabel(parentWidget);
    endDurationLongLabel->setObjectName(QString::fromUtf8("endDurationLongLabel"));

    endDurationGridLayout->addWidget(endDurationLongLabel, 1, 0, 1, 1);

    endDurationShortSpinBox = new QSpinBox(parentWidget);
    endDurationShortSpinBox->setObjectName(QString::fromUtf8("endDurationShortSpinBox"));
    endDurationShortSpinBox->setMinimum(1);
    endDurationShortSpinBox->setMaximum(500);
    endDurationShortSpinBox->setValue(150);

    endDurationGridLayout->addWidget(endDurationShortSpinBox, 0, 2, 1, 1);

    endDurationLongSpinBox = new QSpinBox(parentWidget);
    endDurationLongSpinBox->setObjectName(QString::fromUtf8("endDurationLongSpinBox"));
    endDurationLongSpinBox->setMinimum(1);
    endDurationLongSpinBox->setMaximum(500);
    endDurationLongSpinBox->setValue(250);

    endDurationGridLayout->addWidget(endDurationLongSpinBox, 1, 2, 1, 1);

    endDurationLongSlider = new QSlider(parentWidget);
    endDurationLongSlider->setObjectName(QString::fromUtf8("endDurationLongSlider"));
    endDurationLongSlider->setMinimum(1);
    endDurationLongSlider->setMaximum(500);
    endDurationLongSlider->setValue(250);
    endDurationLongSlider->setOrientation(Qt::Horizontal);

    endDurationGridLayout->addWidget(endDurationLongSlider, 1, 1, 1, 1);


    endParamsGridLayout->addLayout(endDurationGridLayout, 1, 1, 1, 1);


    layout->addLayout(endParamsGridLayout);

    addDeleteButton(parentWidget, layout);

//    parentLayout->addLayout(layout);

    startLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "Start", nullptr));
    startIntensityLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "Intensity", nullptr));
    startIntensityLowLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "Low", nullptr));
    startIntensityHighSpinBox->setSuffix(QApplication::translate("ButtplugDeviceConfigDialog", "%", nullptr));
    startIntensityHighLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "High", nullptr));
    startIntensityLowSpinBox->setSuffix(QApplication::translate("ButtplugDeviceConfigDialog", "%", nullptr));
    startDurationLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "Duration", nullptr));
    startDurationShortLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "Short", nullptr));
    startDurationLongLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "Long", nullptr));
    startDurationShortSpinBox->setSuffix(QApplication::translate("ButtplugDeviceConfigDialog", "ms", nullptr));
    startDurationLongSpinBox->setSuffix(QApplication::translate("ButtplugDeviceConfigDialog", "ms", nullptr));
    endLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "End", nullptr));
    endIntensityLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "Intensity", nullptr));
    endIntensityLowLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "Low", nullptr));
    endIntensityHighSpinBox->setSuffix(QApplication::translate("ButtplugDeviceConfigDialog", "%", nullptr));
    endIntensityHighLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "High", nullptr));
    endIntensityLowSpinBox->setSuffix(QApplication::translate("ButtplugDeviceConfigDialog", "%", nullptr));
    endDurationLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "Duration", nullptr));
    endDurationShortLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "Short", nullptr));
    endDurationLongLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "Long", nullptr));
    endDurationShortSpinBox->setSuffix(QApplication::translate("ButtplugDeviceConfigDialog", "ms", nullptr));
    endDurationLongSpinBox->setSuffix(QApplication::translate("ButtplugDeviceConfigDialog", "ms", nullptr));
}

void  VibratorPulseFeatureParams::adoptUi( QHBoxLayout *layout,
                                    QCheckBox *enableCheckBox,
                                    QComboBox *eventTypeComboBox,
                                    QSpinBox *syncAdjustSpinBox,
                                    QSlider *startIntensityHighSlider,
                                    QDoubleSpinBox *startIntensityHighSpinBox,
                                    QDoubleSpinBox *startIntensityLowSpinBox,
                                    QSlider *startIntensityLowSlider,
                                    QSlider *startDurationShortSlider,
                                    QSpinBox *startDurationShortSpinBox,
                                    QSpinBox *startDurationLongSpinBox,
                                    QSlider *startDurationLongSlider,
                                    QSlider *endIntensityHighSlider,
                                    QDoubleSpinBox *endIntensityHighSpinBox,
                                    QDoubleSpinBox *endIntensityLowSpinBox,
                                    QSlider *endIntensityLowSlider,
                                    QSlider *endDurationShortSlider,
                                    QSpinBox *endDurationShortSpinBox,
                                    QSpinBox *endDurationLongSpinBox,
                                    QSlider *endDurationLongSlider,
                                    QToolButton *deleteButton )
{
    ButtplugFeatureParams::adoptUi(layout, enableCheckBox, eventTypeComboBox, syncAdjustSpinBox);
    this->startIntensityHighSlider = startIntensityHighSlider;
    this->startIntensityHighSpinBox = startIntensityHighSpinBox;
    this->startIntensityLowSpinBox = startIntensityLowSpinBox;
    this->startIntensityLowSlider = startIntensityLowSlider;
    this->startDurationShortSlider = startDurationShortSlider;
    this->startDurationShortSpinBox = startDurationShortSpinBox;
    this->startDurationLongSpinBox = startDurationLongSpinBox;
    this->startDurationLongSlider = startDurationLongSlider;
    this->endIntensityHighSlider = endIntensityHighSlider;
    this->endIntensityHighSpinBox = endIntensityHighSpinBox;
    this->endIntensityLowSpinBox = endIntensityLowSpinBox;
    this->endIntensityLowSlider = endIntensityLowSlider;
    this->endDurationShortSlider = endDurationShortSlider;
    this->endDurationShortSpinBox = endDurationShortSpinBox;
    this->endDurationLongSpinBox = endDurationLongSpinBox;
    this->endDurationLongSlider = endDurationLongSlider;
    this->deleteButton = deleteButton;

//    startingIntensityHigh = startIntensityHighSpinBox->value();
//    startingIntensityLow = startIntensityLowSpinBox->value();
//    startingDurationLong = startDurationLongSpinBox->value();
//    startingDurationShort = startDurationShortSpinBox->value();
//    endingIntensityHigh = endIntensityHighSpinBox->value();
//    endingIntensityLow = endIntensityLowSpinBox->value();
//    endingDurationLong = endDurationLongSpinBox->value();
//    endingDurationShort = endDurationShortSpinBox->value();
}

void VibratorPulseFeatureParams::adoptUiValues()
{
    ButtplugFeatureParams::adoptUiValues();
    startingIntensityLow = startIntensityLowSpinBox->value();
    startingIntensityHigh = startIntensityHighSpinBox->value();
    endingIntensityLow = endIntensityLowSpinBox->value();
    endingIntensityHigh = endIntensityHighSpinBox->value();
    startingDurationShort = startDurationShortSpinBox->value();
    startingDurationLong = startDurationLongSpinBox->value();
    endingDurationShort = endDurationShortSpinBox->value();
    endingDurationLong = endDurationLongSpinBox->value();
}

void VibratorPulseFeatureParams::connectWidgetSignals()
{
    ButtplugFeatureParams::connectWidgetSignals();
    connect(startIntensityLowSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_StartingIntensityLowChanged(double)));
    connect(startIntensityLowSlider, SIGNAL(valueChanged(int)), this, SLOT(on_StartingIntensityLowChanged(int)));
    connect(startIntensityHighSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_StartingIntensityHighChanged(double)));
    connect(startIntensityHighSlider, SIGNAL(valueChanged(int)), this, SLOT(on_StartingIntensityHighChanged(int)));

    connect(startDurationShortSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_StartingDurationShortChanged(int)));
    connect(startDurationShortSlider, SIGNAL(valueChanged(int)), this, SLOT(on_StartingDurationShortChanged(int)));
    connect(startDurationLongSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_StartingDurationLongChanged(int)));
    connect(startDurationLongSlider, SIGNAL(valueChanged(int)), this, SLOT(on_StartingDurationLongChanged(int)));

    connect(endIntensityLowSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_EndingIntensityLowChanged(double)));
    connect(endIntensityLowSlider, SIGNAL(valueChanged(int)), this, SLOT(on_EndingIntensityLowChanged(int)));
    connect(endIntensityHighSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_EndingIntensityHighChanged(double)));
    connect(endIntensityHighSlider, SIGNAL(valueChanged(int)), this, SLOT(on_EndingIntensityHighChanged(int)));

    connect(endDurationShortSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_EndingDurationShortChanged(int)));
    connect(endDurationShortSlider, SIGNAL(valueChanged(int)), this, SLOT(on_EndingDurationShortChanged(int)));
    connect(endDurationLongSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_EndingDurationLongChanged(int)));
    connect(endDurationLongSlider, SIGNAL(valueChanged(int)), this, SLOT(on_EndingDurationLongChanged(int)));
}

void VibratorPulseFeatureParams::updateUiFromData()
{
    ButtplugFeatureParams::updateUiFromData();
    setStartingIntensityLow(startingIntensityLow);
    setStartingIntensityHigh(startingIntensityHigh);
    setEndingIntensityLow(endingIntensityLow);
    setEndingIntensityHigh(endingIntensityHigh);
    setStartingDurationShort(startingDurationShort);
    setStartingDurationLong(startingDurationLong);
    setEndingDurationShort(endingDurationShort);
    setEndingDurationLong(endingDurationLong);
}

void VibratorPulseFeatureParams::copyParamsFrom(ButtplugFeatureParams *other)
{
    ButtplugFeatureParams::copyParamsFrom(other);
    VibratorPulseFeatureParams * otherVibPulseParams = dynamic_cast<VibratorPulseFeatureParams *>(other);
    if (otherVibPulseParams != nullptr)
    {
        startingIntensityLow = otherVibPulseParams->startingIntensityLow;
        startingIntensityHigh = otherVibPulseParams->startingIntensityHigh;
        endingIntensityLow = otherVibPulseParams->endingIntensityLow;
        endingIntensityHigh = otherVibPulseParams->endingIntensityHigh;
        startingDurationShort = otherVibPulseParams->startingDurationShort;
        startingDurationLong = otherVibPulseParams->startingDurationLong;
        endingDurationShort = otherVibPulseParams->endingDurationShort;
        endingDurationLong = otherVibPulseParams->endingDurationLong;
    }
}

#define CONVERSION_SCALE 100

void VibratorPulseFeatureParams::on_StartingIntensityLowChanged(double newValue)
{
    setStartingIntensityLow(newValue);
}

void VibratorPulseFeatureParams::on_StartingIntensityLowChanged(int newValue)
{
    setStartingIntensityLow((double) newValue / CONVERSION_SCALE);
}

void VibratorPulseFeatureParams::setStartingIntensityLow(double value)
{
    startingIntensityLow = value;
    startIntensityLowSpinBox->setValue(value);
    startIntensityLowSlider->setValue(value * CONVERSION_SCALE);
    if (value > getStartingIntensityHigh())
        setStartingIntensityHigh(value);
}

double VibratorPulseFeatureParams::getStartingIntensityLow()
{
    return startingIntensityLow;
}

void VibratorPulseFeatureParams::on_StartingIntensityHighChanged(double newValue)
{
    setStartingIntensityHigh(newValue);
}

void VibratorPulseFeatureParams::on_StartingIntensityHighChanged(int newValue)
{
    setStartingIntensityHigh((double) newValue / CONVERSION_SCALE);
}

void VibratorPulseFeatureParams::setStartingIntensityHigh(double value)
{
    startingIntensityHigh = value;
    startIntensityHighSpinBox->setValue(value);
    startIntensityHighSlider->setValue(value * CONVERSION_SCALE);
    if (value < getStartingIntensityLow())
        setStartingIntensityLow(value);
}

double VibratorPulseFeatureParams::getStartingIntensityHigh()
{
    return startingIntensityHigh;
}

void VibratorPulseFeatureParams::on_StartingDurationShortChanged(int newValue)
{
    setStartingDurationShort(newValue);
}

void VibratorPulseFeatureParams::setStartingDurationShort(int value)
{
    startingDurationShort = value;
    startDurationShortSlider->setValue(value);
    startDurationShortSpinBox->setValue(value);
    if (value > getStartingDurationLong())
        setStartingDurationLong(value);
}

int VibratorPulseFeatureParams::getStartingDurationShort()
{
    return startingDurationShort;
}

void VibratorPulseFeatureParams::on_StartingDurationLongChanged(int newValue)
{
    setStartingDurationLong(newValue);
}

void VibratorPulseFeatureParams::setStartingDurationLong(int value)
{
    startingDurationLong = value;
    startDurationLongSlider->setValue(value);
    startDurationLongSpinBox->setValue(value);
    if (value < getStartingDurationShort())
        setStartingDurationShort(value);
}

int VibratorPulseFeatureParams::getStartingDurationLong()
{
    return startingDurationLong;
}

void VibratorPulseFeatureParams::on_EndingIntensityLowChanged(double newValue)
{
    setEndingIntensityLow(newValue);
}

void VibratorPulseFeatureParams::on_EndingIntensityLowChanged(int newValue)
{
    setEndingIntensityLow((double) newValue / CONVERSION_SCALE);
}

void VibratorPulseFeatureParams::setEndingIntensityLow(double value)
{
    endingIntensityLow = value;
    endIntensityLowSpinBox->setValue(value);
    endIntensityLowSlider->setValue(value * CONVERSION_SCALE);
    if (value > getEndingIntensityHigh())
        setEndingIntensityHigh(value);
}

double VibratorPulseFeatureParams::getEndingIntensityLow()
{
    return endingIntensityLow;
}

void VibratorPulseFeatureParams::on_EndingIntensityHighChanged(double newValue)
{
    setEndingIntensityHigh(newValue);
}

void VibratorPulseFeatureParams::on_EndingIntensityHighChanged(int newValue)
{
    setEndingIntensityHigh((double) newValue / CONVERSION_SCALE);
}

void VibratorPulseFeatureParams::setEndingIntensityHigh(double value)
{
    endingIntensityHigh = value;
    endIntensityHighSpinBox->setValue(value);
    endIntensityHighSlider->setValue(value * CONVERSION_SCALE);
    if (value < getEndingIntensityLow())
        setEndingIntensityLow(value);
}

double VibratorPulseFeatureParams::getEndingIntensityHigh()
{
    return endingIntensityHigh;
}

void VibratorPulseFeatureParams::on_EndingDurationShortChanged(int newValue)
{
    setEndingDurationShort(newValue);
}

void VibratorPulseFeatureParams::setEndingDurationShort(int value)
{
    endingDurationShort = value;
    endDurationShortSlider->setValue(value);
    endDurationShortSpinBox->setValue(value);
    if (value > getEndingDurationLong())
        setEndingDurationLong(value);
}

int VibratorPulseFeatureParams::getEndingDurationShort()
{
    return endingDurationShort;
}

void VibratorPulseFeatureParams::on_EndingDurationLongChanged(int newValue)
{
    setEndingDurationLong(newValue);
}

void VibratorPulseFeatureParams::setEndingDurationLong(int value)
{
    endingDurationLong = value;
    endDurationLongSlider->setValue(value);
    endDurationLongSpinBox->setValue(value);
    if (value < getEndingDurationShort())
        setEndingDurationShort(value);
}

int VibratorPulseFeatureParams::getEndingDurationLong()
{
    return endingDurationLong;
}

double VibratorPulseFeatureParams::getRequestIntensity(Event event)
{
    //! How heavily should the intensity be weighted towards the 'ending value'?
    double endProportion = mainWindow->progressThroughGame(event.timestamp);
    //! How heavily should the intensity be weighted towards the 'starting value'?
    double beginningProportion = 1.0 - endProportion;

    //based on the current progress through the game, work out the maximum and minimum possible intensities for this event
    double highValue = (beginningProportion * getStartingIntensityHigh()) + (endProportion * getEndingIntensityHigh());
    double lowValue = (beginningProportion * getStartingIntensityLow()) + (endProportion * getEndingIntensityLow());

    //use the value of the event to choose an intensity between these two extremes
    double highProportion = (double) event.value / event.maxPossibleValue();
    double lowProportion = 1.0 - highProportion;
    return (highProportion * highValue) + (lowProportion * lowValue);
}

int VibratorPulseFeatureParams::getPulseLength(Event event)
{
    //! How heavily should the length be weighted towards the 'ending value'?
    double endProportion = mainWindow->progressThroughGame(event.timestamp);
    //! How heavily should the length be weighted towards the 'starting value'?
    double beginningProportion = 1.0 - endProportion;
//    qDebug() << "startProp:" << beginningProportion << ", endProp: " << endProportion;

    //based on the current progress through the game, work out the maximum and minimum possible length for this event
    double longValue = (beginningProportion * getStartingDurationLong()) + (endProportion * getEndingDurationLong());
    double shortValue = (beginningProportion * getStartingDurationShort()) + (endProportion * getEndingDurationShort());
//    qDebug() << "shortVal:" << shortValue << ", longVal: " << longValue;

    //use the value of the event to choose a length between these two extremes
    double longProportion = (double) event.value / event.maxPossibleValue();
    double shortProportion = 1.0 - longProportion;
//    qDebug() << "shortProp:" << shortProportion << ", longProp: " << longProportion;
    return roundToInt((longProportion * longValue) + (shortProportion * shortValue));
}

void VibratorPulseFeatureParams::handleEvent(Event event, ButtplugDeviceFeature *feature)
{
    long triggerTime = event.timestamp + syncAdjust - ButtplugDevice::eventCollationTime;
    long timeUntilTrigger = triggerTime - currentTimecode();
    if (timeUntilTrigger <= 0)
        startPulseNow(event, feature);
    else if (currentlyPlaying)
    {
        QTimer::singleShot(timeUntilTrigger, Qt::PreciseTimer, this, [this, event, feature] ()
        {
            handleEvent(event, feature);
        });
    }
}

QTimer *VibratorPulseFeatureParams::getEndTimerFor(ButtplugDeviceFeature *feature)
{
    if (endTimers.contains(feature))
        return endTimers[feature];

    QTimer * timer = new QTimer(mainWindow->getButtplugInterface());
    timer->setTimerType(Qt::PreciseTimer);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this, feature](){stopPulseNow(feature);});
    endTimers.insert(feature, timer);
    return timer;
}

VibratorPulseFeatureParams *VibratorPulseFeatureParams::fromSettingsGroup(QSettings & settings)
{
    VibratorPulseFeatureParams * retVal = new VibratorPulseFeatureParams(mainWindow->getButtplugInterface());
    retVal->readSettingsGroup(settings);
    return retVal;
}

void VibratorPulseFeatureParams::writeSettingsGroup(QSettings &settings)
{
    settings.beginGroup(featureName + " Vibrator Pulse");
    ButtplugFeatureParams::writeSettingsGroup(settings);
    settings.setValue(PULSE_INTENSITIES_SETTING, getPulseIntensitySettings());
    settings.setValue(PULSE_LENGTHS_SETTING, getPulseLengthSettings());
    settings.endGroup();
}

QVariantList VibratorPulseFeatureParams::getPulseIntensitySettings()
{
    QVariantList retVal{};
    retVal.append(getStartingIntensityLow());
    retVal.append(getStartingIntensityHigh());
    retVal.append(getEndingIntensityLow());
    retVal.append(getEndingIntensityHigh());
    return retVal;
}

void VibratorPulseFeatureParams::setPulseIntensitySettings(QVariantList intensities)
{
    if (intensities.length() != 4)
        return;

    startingIntensityLow = intensities[0].toDouble();
    startingIntensityHigh = intensities[1].toDouble();
    endingIntensityLow = intensities[2].toDouble();
    endingIntensityHigh = intensities[3].toDouble();
}

QVariantList VibratorPulseFeatureParams::getPulseLengthSettings()
{
    QVariantList retVal{};
    retVal.append(getStartingDurationShort());
    retVal.append(getStartingDurationLong());
    retVal.append(getEndingDurationShort());
    retVal.append(getEndingDurationLong());
    return retVal;
}

void VibratorPulseFeatureParams::setPulseLengthSettings(QVariantList lengths)
{
    if (lengths.length() != 4)
        return;

    startingDurationShort = lengths[0].toInt();
    startingDurationLong = lengths[1].toInt();
    endingDurationShort = lengths[2].toInt();
    endingDurationLong = lengths[3].toInt();
}

void VibratorPulseFeatureParams::readSettingsGroup(QSettings &settings)
{
    ButtplugFeatureParams::readSettingsGroup(settings);
    QVariantList intensities = settings.value(PULSE_INTENSITIES_SETTING).value<QVariantList>();
    setPulseIntensitySettings(intensities);
    QVariantList lengths = settings.value(PULSE_LENGTHS_SETTING).value<QVariantList>();
    setPulseLengthSettings(lengths);
}

void VibratorPulseFeatureParams::setUiEnabled(bool enabled)
{
    startIntensityLowSpinBox->setEnabled(enabled);
    startIntensityLowSlider->setEnabled(enabled);
    startIntensityHighSpinBox->setEnabled(enabled);
    startIntensityHighSlider->setEnabled(enabled);

    startDurationShortSpinBox->setEnabled(enabled);
    startDurationShortSlider->setEnabled(enabled);
    startDurationLongSpinBox->setEnabled(enabled);
    startDurationLongSlider->setEnabled(enabled);

    endIntensityLowSpinBox->setEnabled(enabled);
    endIntensityLowSlider->setEnabled(enabled);
    endIntensityHighSpinBox->setEnabled(enabled);
    endIntensityHighSlider->setEnabled(enabled);

    endDurationShortSpinBox->setEnabled(enabled);
    endDurationShortSlider->setEnabled(enabled);
    endDurationLongSpinBox->setEnabled(enabled);
    endDurationLongSlider->setEnabled(enabled);
}

#define STUTTER_LENGTH 50

void VibratorPulseFeatureParams::startPulseNow(Event event, ButtplugDeviceFeature * feature)
{
    QTimer * endTimer = getEndTimerFor(feature);
    if (endTimer->isActive())
    {
        endTimer->stop();
        stopPulseNow(feature);
        QTimer::singleShot(STUTTER_LENGTH, Qt::PreciseTimer, this, [this, event, feature] ()
        {
            startPulseNow(event, feature);
        });
        return;
    }

    double intensity = getRequestIntensity(event) / 100; //convert percentage to 0..1
    int length = getPulseLength(event);
//    qDebug() << "Starting " << length << "ms pulse with intensity " << intensity << " on feature " << feature->getName();

    feature->setVibration(intensity);

    endTimer->start(length - ButtplugDevice::eventCollationTime);
}

void VibratorPulseFeatureParams::stopPulseNow(ButtplugDeviceFeature *feature)
{
//    qDebug() << "Stopping pulse on feature " << feature->getName();
    feature->setVibration(0);
}
