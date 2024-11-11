cmake_minimum_required(VERSION 3.10)

project(CockHeroine)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets SerialPort Multimedia MultimediaWidgets Xml Gamepad Network WebSockets REQUIRED)

set(SOURCE_FILES
    Source/main.cpp
    Source/abstractnewbeatvaluewidget.cpp
    Source/adddialog.cpp
    Source/adjustdialog.cpp
    Source/beatmeter/editorgridline.cpp
    Source/beatvaluewidget.cpp
    Source/buttplug/buttplugdevice.cpp
    Source/buttplug/buttplugdevicefeature.cpp
    Source/buttplug/buttpluginterface.cpp
    Source/buttplugdeviceconfigdialog.cpp
    Source/buttplugdispatcher.cpp
    Source/buttplugfeatureparams.cpp
    Source/customeventaction.cpp
    Source/deletedialog.cpp
    Source/enableidentifyintervalsdialog.cpp
    Source/eventdispatcher.cpp
    Source/exportbeatmeterdialog.cpp
    Source/funscriptwriter.cpp
    Source/handycsvwriter.cpp
    Source/mainwindow.cpp
    Source/event.cpp
    Source/newbeatvaluewidget.cpp
    Source/newcustombeatvaluewidget.cpp
    Source/playbackassociatedaction.cpp
    Source/playbackassociatedactions/pregenerateestimsignalaction.cpp
    Source/preplaybackactionmanager.cpp
    Source/preplaybackinfodialog.cpp
    Source/seektotimecodedialog.cpp
    Source/splitdialog.cpp
    Source/stimsignal/dualchannelsignalgenerator.cpp
    Source/stimsignal/estimwavfilewriter.cpp
    Source/stimsignal/modifiers/channelbalancemodifier.cpp
    Source/stimsignal/modifiers/boostfaststrokesmodifier.cpp
    Source/stimsignal/modifiers/breaksoftenermodifier.cpp
    Source/stimsignal/modifiers/fadefromcoldmodifier.cpp
    Source/stimsignal/modifiers/phaseinvertermodifier.cpp
    Source/stimsignal/modifiers/phasesettermodifier.cpp
    Source/stimsignal/modifiers/progressincreasemodifier.cpp
    Source/stimsignal/modifiers/singlechannelbeatproximitymodifier.cpp
    Source/stimsignal/modifiers/triphasebeatproximitymodifier.cpp
    Source/stimsignal/modifiers/triphasemodifier.cpp
    Source/stimsignal/modifiers/waypoint.cpp
    Source/stimsignal/modifiers/waypointfollowermodifier.cpp
    Source/stimsignal/modifiers/waypointlist.cpp
    Source/stimsignal/monostimsignalsample.cpp
    Source/stimsignal/multithreadedsamplepipelineprocessor.cpp
    Source/stimsignal/singlechannelsignalgenerator.cpp
    Source/stimsignal/stereostimsignalsample.cpp
    Source/stimsignal/stimsignalfile.cpp
    Source/stimsignal/stimsignalgenerator.cpp
    Source/stimsignal/stimsignalmodifier.cpp
    Source/stimsignal/stimsignalsample.cpp
    Source/stimsignal/stimsignalsamplefactory.cpp
    Source/stimsignal/stimsignalsource.cpp
    Source/stimsignal/stimsignalworker.cpp
    Source/stimsignal/triphasesignalgenerator.cpp
    Source/syncfilewriter.cpp
    Source/vibratorpulsefeatureparams.cpp
    Source/wavefileexporter.cpp
    Source/midifilewriter.cpp
    Source/globals.cpp
    Source/eventdatamodel.cpp
    Source/eventdataproxymodel.cpp
    Source/eventtabledelegate.cpp
    Source/editorwindow.cpp
    Source/beatdatamodel.cpp
    Source/abstractbeatinterval.cpp
    Source/beatinterval.cpp
    Source/uniquebeatinterval.cpp
    Source/beatvalue.cpp
    Source/helperfunctions.cpp
    Source/beattimestamp.cpp
    Source/intervaldatamodel.cpp
    Source/beatanalysis.cpp
    Source/beatpattern.cpp
    Source/analysisoptionsdialog.cpp
    Source/optimisationoptionsdialog.cpp
    Source/patterndatamodel.cpp
    Source/beatoptimisation.cpp
    Source/valuedatamodel.cpp
    Source/valuetablekeyboardeventhandler.cpp
    Source/eventmetadata.cpp
    Source/chmlhandler.cpp
    Source/midifilereader.cpp
    Source/optionsdialog.cpp
    Source/croppedvideosurface.cpp
    Source/graphicsscenevideodialog.cpp
    Source/keyboardshortcutsdialog.cpp
)

set(UI_FILES
    Source/mainwindow.ui
    Source/adddialog.ui
    Source/adjustdialog.ui
    Source/buttplugdeviceconfigdialog.ui
    Source/deletedialog.ui
    Source/enableidentifyintervalsdialog.ui
    Source/exportbeatmeterdialog.ui
    Source/preplaybackinfodialog.ui
    Source/seektotimecodedialog.ui
    Source/editorwindow.ui
    Source/analysisoptionsdialog.ui
    Source/optimisationoptionsdialog.ui
    Source/optionsdialog.ui
    Source/graphicsscenevideodialog.ui
    Source/keyboardshortcutsdialog.ui
    Source/splitdialog.ui
)

set(HEADER_FILES
    Source/mainwindow.h
    Source/abstractnewbeatvaluewidget.h
    Source/adddialog.h
    Source/adjustdialog.h
    Source/beatmeter/editorgridline.h
    Source/beatvaluewidget.h
    Source/buttplug/buttplugdevice.h
    Source/buttplug/buttplugdevicefeature.h
    Source/buttplug/buttpluginterface.h
    Source/buttplugdeviceconfigdialog.h
    Source/buttplugdispatcher.h
    Source/buttplugfeatureparams.h
    Source/customeventaction.h
    Source/deletedialog.h
    Source/enableidentifyintervalsdialog.h
    Source/event.h
    Source/eventdispatcher.h
    Source/exportbeatmeterdialog.h
    Source/funscriptwriter.h
    Source/handycsvwriter.h
    Source/newbeatvaluewidget.h
    Source/newcustombeatvaluewidget.h
    Source/playbackassociatedaction.h
    Source/playbackassociatedactions/pregenerateestimsignalaction.h
    Source/preplaybackactionmanager.h
    Source/preplaybackinfodialog.h
    Source/seektotimecodedialog.h
    Source/splitdialog.h
    Source/stimsignal/dualchannelsignalgenerator.h
    Source/stimsignal/estimwavfilewriter.h
    Source/stimsignal/modifiers/channelbalancemodifier.h
    Source/stimsignal/modifiers/boostfaststrokesmodifier.h
    Source/stimsignal/modifiers/breaksoftenermodifier.h
    Source/stimsignal/modifiers/fadefromcoldmodifier.h
    Source/stimsignal/modifiers/phaseinvertermodifier.h
    Source/stimsignal/modifiers/phasesettermodifier.h
    Source/stimsignal/modifiers/progressincreasemodifier.h
    Source/stimsignal/modifiers/singlechannelbeatproximitymodifier.h
    Source/stimsignal/modifiers/triphasebeatproximitymodifier.h
    Source/stimsignal/modifiers/triphasemodifier.h
    Source/stimsignal/modifiers/waypoint.h
    Source/stimsignal/modifiers/waypointfollowermodifier.h
    Source/stimsignal/modifiers/waypointlist.h
    Source/stimsignal/monostimsignalsample.h
    Source/stimsignal/multithreadedsamplepipelineprocessor.h
    Source/stimsignal/singlechannelsignalgenerator.h
    Source/stimsignal/singlechannelsignalgenerator.h
    Source/stimsignal/stereostimsignalsample.h
    Source/stimsignal/stimsignalfile.h
    Source/stimsignal/stimsignalgenerator.h
    Source/stimsignal/stimsignalmodifier.h
    Source/stimsignal/stimsignalsample.h
    Source/stimsignal/stimsignalsamplefactory.h
    Source/stimsignal/stimsignalsource.h
    Source/stimsignal/stimsignalworker.h
    Source/stimsignal/triphasesignalgenerator.h
    Source/syncfilewriter.h
    Source/vibratorpulsefeatureparams.h
    Source/wavefileexporter.h
    Source/midifilewriter.h
    Source/globals.h
    Source/eventdatamodel.h
    Source/eventdataproxymodel.h
    Source/eventtabledelegate.h
    Source/editorwindow.h
    Source/beatdatamodel.h
    Source/abstractbeatinterval.h
    Source/beatinterval.h
    Source/uniquebeatinterval.h
    Source/beatvalue.h
    Source/helperfunctions.h
    Source/beattimestamp.h
    Source/intervaldatamodel.h
    Source/config.h
    Source/beatanalysis.h
    Source/beatpattern.h
    Source/analysisoptionsdialog.h
    Source/optimisationoptionsdialog.h
    Source/patterndatamodel.h
    Source/beatoptimisation.h
    Source/valuedatamodel.h
    Source/valuetablekeyboardeventhandler.h
    Source/eventmetadata.h
    Source/chmlhandler.h
    Source/midifilereader.h
    Source/optionsdialog.h
    Source/croppedvideosurface.h
    Source/graphicsscenevideodialog.h
    Source/keyboardshortcutsdialog.h)

add_executable(${PROJECT_NAME} ${UI_FILES} ${HEADER_FILES} ${SOURCE_FILES} Resources/resources.qrc)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::SerialPort Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Xml Qt5::Gamepad Qt5::Network Qt5::WebSockets)

add_subdirectory(Resources)

if (UNIX AND NOT APPLE)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
endif()
