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
    abstractnewbeatvaluewidget.cpp \
    adddialog.cpp \
    adjustdialog.cpp \
    beatmeter/editorgridline.cpp \
    beatvaluewidget.cpp \
    buttplug/buttplugdevice.cpp \
    buttplug/buttplugdevicefeature.cpp \
    buttplug/buttpluginterface.cpp \
    buttplugdeviceconfigdialog.cpp \
    buttplugdispatcher.cpp \
    buttplugfeatureparams.cpp \
    customeventaction.cpp \
    deletedialog.cpp \
    enableidentifyintervalsdialog.cpp \
    eventdispatcher.cpp \
    exportbeatmeterdialog.cpp \
    funscriptwriter.cpp \
    handycsvwriter.cpp \
        mainwindow.cpp \
    event.cpp \
    newbeatvaluewidget.cpp \
    newcustombeatvaluewidget.cpp \
    playbackassociatedaction.cpp \
    playbackassociatedactions/pregenerateestimsignalaction.cpp \
    preplaybackactionmanager.cpp \
    preplaybackinfodialog.cpp \
    seektotimecodedialog.cpp \
    splitdialog.cpp \
    stimsignal/dualchannelsignalgenerator.cpp \
    stimsignal/estimwavfilewriter.cpp \
    stimsignal/modifiers/channelbalancemodifier.cpp \
    stimsignal/modifiers/boostfaststrokesmodifier.cpp \
    stimsignal/modifiers/breaksoftenermodifier.cpp \
    stimsignal/modifiers/fadefromcoldmodifier.cpp \
    stimsignal/modifiers/phaseinvertermodifier.cpp \
    stimsignal/modifiers/phasesettermodifier.cpp \
    stimsignal/modifiers/progressincreasemodifier.cpp \
    stimsignal/modifiers/singlechannelbeatproximitymodifier.cpp \
    stimsignal/modifiers/triphasebeatproximitymodifier.cpp \
    stimsignal/modifiers/triphasemodifier.cpp \
    stimsignal/modifiers/waypoint.cpp \
    stimsignal/modifiers/waypointfollowermodifier.cpp \
    stimsignal/modifiers/waypointlist.cpp \
    stimsignal/monostimsignalsample.cpp \
    stimsignal/multithreadedsamplepipelineprocessor.cpp \
    stimsignal/singlechannelsignalgenerator.cpp \
    stimsignal/stereostimsignalsample.cpp \
    stimsignal/stimsignalfile.cpp \
    stimsignal/stimsignalgenerator.cpp \
    stimsignal/stimsignalmodifier.cpp \
    stimsignal/stimsignalsample.cpp \
    stimsignal/stimsignalsamplefactory.cpp \
    stimsignal/stimsignalsource.cpp \
    stimsignal/stimsignalworker.cpp \
    stimsignal/triphasesignalgenerator.cpp \
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
    abstractnewbeatvaluewidget.h \
    adddialog.h \
    adjustdialog.h \
    beatmeter/editorgridline.h \
    beatvaluewidget.h \
    buttplug/buttplugdevice.h \
    buttplug/buttplugdevicefeature.h \
    buttplug/buttpluginterface.h \
    buttplugdeviceconfigdialog.h \
    buttplugdispatcher.h \
    buttplugfeatureparams.h \
    customeventaction.h \
    deletedialog.h \
    enableidentifyintervalsdialog.h \
    event.h \
    eventdispatcher.h \
    exportbeatmeterdialog.h \
    funscriptwriter.h \
    handycsvwriter.h \
    newbeatvaluewidget.h \
    newcustombeatvaluewidget.h \
    playbackassociatedaction.h \
    playbackassociatedactions/pregenerateestimsignalaction.h \
    preplaybackactionmanager.h \
    preplaybackinfodialog.h \
    seektotimecodedialog.h \
    splitdialog.h \
    stimsignal/dualchannelsignalgenerator.h \
    stimsignal/estimwavfilewriter.h \
    stimsignal/modifiers/channelbalancemodifier.h \
    stimsignal/modifiers/boostfaststrokesmodifier.h \
    stimsignal/modifiers/breaksoftenermodifier.h \
    stimsignal/modifiers/fadefromcoldmodifier.h \
    stimsignal/modifiers/phaseinvertermodifier.h \
    stimsignal/modifiers/phasesettermodifier.h \
    stimsignal/modifiers/progressincreasemodifier.h \
    stimsignal/modifiers/singlechannelbeatproximitymodifier.h \
    stimsignal/modifiers/triphasebeatproximitymodifier.h \
    stimsignal/modifiers/triphasemodifier.h \
    stimsignal/modifiers/waypoint.h \
    stimsignal/modifiers/waypointfollowermodifier.h \
    stimsignal/modifiers/waypointlist.h \
    stimsignal/monostimsignalsample.h \
    stimsignal/multithreadedsamplepipelineprocessor.h \
    stimsignal/singlechannelsignalgenerator.h \
    stimsignal/stereostimsignalsample.h \
    stimsignal/stimsignalfile.h \
    stimsignal/stimsignalgenerator.h \
    stimsignal/stimsignalmodifier.h \
    stimsignal/stimsignalsample.h \
    stimsignal/stimsignalsamplefactory.h \
    stimsignal/stimsignalsource.h \
    stimsignal/stimsignalworker.h \
    stimsignal/triphasesignalgenerator.h \
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
    enableidentifyintervalsdialog.ui \
    exportbeatmeterdialog.ui \
    preplaybackinfodialog.ui \
    seektotimecodedialog.ui \
    editorwindow.ui \
    analysisoptionsdialog.ui \
    optimisationoptionsdialog.ui \
    optionsdialog.ui \
    graphicsscenevideodialog.ui \
    keyboardshortcutsdialog.ui \
    splitdialog.ui

RESOURCES   = resources.qrc
