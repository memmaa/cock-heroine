#-------------------------------------------------
#
# Project created by QtCreator 2015-04-04T01:19:46
#
#-------------------------------------------------

QT       += core gui serialport multimediawidgets xml gamepad network websockets multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CockHeroine
TEMPLATE = app


SOURCES += main.cpp\
    adddialog.cpp \
    adjustdialog.cpp \
    beatvaluewidget.cpp \
    buttplug/buttplugdevice.cpp \
    buttplug/buttplugdevicefeature.cpp \
    buttplug/buttpluginterface.cpp \
    buttplugdeviceconfigdialog.cpp \
    buttplugdispatcher.cpp \
    buttplugfeatureparams.cpp \
    customeventaction.cpp \
    deletedialog.cpp \
    eventdispatcher.cpp \
    exportbeatmeterdialog.cpp \
    funscriptwriter.cpp \
    handycsvwriter.cpp \
        mainwindow.cpp \
    event.cpp \
    seektotimecodedialog.cpp \
    splitdialog.cpp \
    stimsignal/modifiers/beatproximitymodifier.cpp \
    stimsignal/modifiers/triphasemodifier.cpp \
    stimsignal/stimsignalgenerator.cpp \
    stimsignal/stimsignalmodifier.cpp \
    stimsignal/stimsignalsample.cpp \
    syncfilewriter.cpp \
    vibratorpulsefeatureparams.cpp \
    wavefileexporter.cpp \
    midifilewriter.cpp \
    globals.cpp \
    eventdatamodel.cpp \
    eventdataproxymodel.cpp \
    eventtabledelegate.cpp \
    editorwindow.cpp \
    beatdatamodel.cpp \
    abstractbeatinterval.cpp \
    beatinterval.cpp \
    uniquebeatinterval.cpp \
    beatvalue.cpp \
    helperfunctions.cpp \
    beattimestamp.cpp \
    intervaldatamodel.cpp \
    beatanalysis.cpp \
    beatpattern.cpp \
    analysisoptionsdialog.cpp \
    optimisationoptionsdialog.cpp \
    patterndatamodel.cpp \
    beatoptimisation.cpp \
    valuedatamodel.cpp \
    valuetablekeyboardeventhandler.cpp \
    eventmetadata.cpp \
    chmlhandler.cpp \
    midifilereader.cpp \
    optionsdialog.cpp \
    croppedvideosurface.cpp \
    graphicsscenevideodialog.cpp \
    keyboardshortcutsdialog.cpp

HEADERS  += mainwindow.h \
    adddialog.h \
    adjustdialog.h \
    beatvaluewidget.h \
    buttplug/buttplugdevice.h \
    buttplug/buttplugdevicefeature.h \
    buttplug/buttpluginterface.h \
    buttplugdeviceconfigdialog.h \
    buttplugdispatcher.h \
    buttplugfeatureparams.h \
    customeventaction.h \
    deletedialog.h \
    event.h \
    eventdispatcher.h \
    exportbeatmeterdialog.h \
    funscriptwriter.h \
    handycsvwriter.h \
    seektotimecodedialog.h \
    splitdialog.h \
    stimsignal/modifiers/beatproximitymodifier.h \
    stimsignal/modifiers/triphasemodifier.h \
    stimsignal/stimsignalgenerator.h \
    stimsignal/stimsignalmodifier.h \
    stimsignal/stimsignalsample.h \
    syncfilewriter.h \
    vibratorpulsefeatureparams.h \
    wavefileexporter.h \
    midifilewriter.h \
    globals.h \
    eventdatamodel.h \
    eventdataproxymodel.h \
    eventtabledelegate.h \
    editorwindow.h \
    beatdatamodel.h \
    abstractbeatinterval.h \
    beatinterval.h \
    uniquebeatinterval.h \
    beatvalue.h \
    helperfunctions.h \
    beattimestamp.h \
    intervaldatamodel.h \
    config.h \
    beatanalysis.h \
    beatpattern.h \
    analysisoptionsdialog.h \
    optimisationoptionsdialog.h \
    patterndatamodel.h \
    beatoptimisation.h \
    valuedatamodel.h \
    valuetablekeyboardeventhandler.h \
    eventmetadata.h \
    chmlhandler.h \
    midifilereader.h \
    optionsdialog.h \
    croppedvideosurface.h \
    graphicsscenevideodialog.h \
    keyboardshortcutsdialog.h

FORMS    += mainwindow.ui \
    adddialog.ui \
    adjustdialog.ui \
    buttplugdeviceconfigdialog.ui \
    deletedialog.ui \
    exportbeatmeterdialog.ui \
    seektotimecodedialog.ui \
    editorwindow.ui \
    analysisoptionsdialog.ui \
    optimisationoptionsdialog.ui \
    optionsdialog.ui \
    graphicsscenevideodialog.ui \
    keyboardshortcutsdialog.ui \
    splitdialog.ui

RESOURCES   = resources.qrc
