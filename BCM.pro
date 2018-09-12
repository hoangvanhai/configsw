#-------------------------------------------------
#
# Project created by QtCreator 2018-06-08T18:07:20
#
#-------------------------------------------------

QT       += core charts gui serialport gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BCM    #battery charger managerment
TEMPLATE = app

CONFIG(release, debug|release) {
    DESTDIR = .
    OBJECTS_DIR = .obj
    MOC_DIR = .moc
    RCC_DIR = .rcc
    UI_DIR = .ui
}



INCLUDEPATH +=  $$PWD \
                $$PWD/ui \
                $$PWD/ui/setting \
                $$PWD/ibc \
                $$PWD/ibc/framing \
                $$PWD/ibc/lib \
                $$PWD/cpp_lib/communication/socket/serialport \
                $$PWD/cpp_lib/communication/socket \
                $$PWD/cpp_lib/communication/dll \
                $$PWD/cpp_lib/pattern \
                $$PWD/cpp_lib/string   \
                $$PWD/cpp_lib/csv

win32 {
    DEFINES += WIN32_LEAN_AND_MEAN  # exclude some APIs Cryptography, DDE, RPC, Shell ...
    DEFINES -= UNICODE              # used for serial port opening with WINAPI
    CONFIG += c++11
    LIBS += -lws2_32
    CONFIG += static
    RC_ICONS = $$PWD\icon\app.ico
}

unix {
    CONFIG += c++11 c++14
    LIBS += -ldl -lpthread
}

DEFINES += XLSX_NO_LIB

SOURCES += main.cpp\
    ui/mainwindow.cpp \
    ui/develop.cpp \
    ui/setting.cpp \
    ui/slidingstackedwidget.cpp \
    ibc/framing/background.cpp \
    ibc/framing/framing.cpp \
    ibc/framing/layer2.cpp \
    ibc/lib/fifo.cpp \
    ibc/lib/crc8.c \
    cpp_lib/csv/csvfile.cpp \
    cpp_lib/communication/socket/serialport/SerialPort_Linux.cpp \
    cpp_lib/communication/socket/serialport/SerialPort_Win.cpp \
    cpp_lib/communication/socket/serialport/SerialSocket.cpp \
    cpp_lib/communication/socket/ISocket.cpp \
    cpp_lib/communication/socket/SocketUtils.cpp \
    cpp_lib/dll/DynamicLinkLibrary.cpp \
    cpp_lib/rand/Random.cpp \
    ibc/framing/layer1.cpp \
    config/config.cpp \
    xlsx/xlsxabstractooxmlfile.cpp \
    xlsx/xlsxabstractsheet.cpp \
    xlsx/xlsxcell.cpp \
    xlsx/xlsxcellformula.cpp \
    xlsx/xlsxcellrange.cpp \
    xlsx/xlsxcellreference.cpp \
    xlsx/xlsxchart.cpp \
    xlsx/xlsxchartsheet.cpp \
    xlsx/xlsxcolor.cpp \
    xlsx/xlsxconditionalformatting.cpp \
    xlsx/xlsxcontenttypes.cpp \
    xlsx/xlsxdatavalidation.cpp \
    xlsx/xlsxdocpropsapp.cpp \
    xlsx/xlsxdocpropscore.cpp \
    xlsx/xlsxdocument.cpp \
    xlsx/xlsxdrawing.cpp \
    xlsx/xlsxdrawinganchor.cpp \
    xlsx/xlsxformat.cpp \
    xlsx/xlsxmediafile.cpp \
    xlsx/xlsxnumformatparser.cpp \
    xlsx/xlsxrelationships.cpp \
    xlsx/xlsxrichstring.cpp \
    xlsx/xlsxsharedstrings.cpp \
    xlsx/xlsxsimpleooxmlfile.cpp \
    xlsx/xlsxstyles.cpp \
    xlsx/xlsxtheme.cpp \
    xlsx/xlsxutility.cpp \
    xlsx/xlsxworkbook.cpp \
    xlsx/xlsxworksheet.cpp \
    xlsx/xlsxzipreader.cpp \
    xlsx/xlsxzipwriter.cpp \
    ui/setting/serialport.cpp \
    ui/setting/hardware.cpp \
    ui/setting/permission.cpp \
    ui/controldock.cpp \
    ui/chargeditor.cpp

HEADERS  += ui/mainwindow.h \
    ui/develop.h \
    ui/mainwindow.h \
    ui/setting.h \
    ui/slidingstackedwidget.h \
    ibc/framing/background.h \
    ibc/framing/framing.h \
    ibc/framing/layer2.h \
    ibc/lib/crc8.h \
    ibc/lib/fifo.h \
    cpp_lib/communication/socket/serialport/SerialSocket.h \
    cpp_lib/communication/socket/ISocket.h \
    cpp_lib/communication/socket/SocketUtils.h \
    cpp_lib/dll/dll_function.h \
    cpp_lib/dll/DynamicLinkLibrary.h \
    cpp_lib/rand/Random.h \
    cpp_lib/string/StringUtils.h \
    cpp_lib/cppframework.h \
    ibc/framing/layer1.h \
    config/config.h \
    cpp_lib/pattern/IChainOfResponsibility.h \
    cpp_lib/pattern/IObserver.h \
    cpp_lib/pattern/ISingleton.h \
    xlsx/xlsxabstractooxmlfile.h \
    xlsx/xlsxabstractooxmlfile_p.h \
    xlsx/xlsxabstractsheet.h \
    xlsx/xlsxabstractsheet_p.h \
    xlsx/xlsxcell.h \
    xlsx/xlsxcell_p.h \
    xlsx/xlsxcellformula.h \
    xlsx/xlsxcellformula_p.h \
    xlsx/xlsxcellrange.h \
    xlsx/xlsxcellreference.h \
    xlsx/xlsxchart.h \
    xlsx/xlsxchart_p.h \
    xlsx/xlsxchartsheet.h \
    xlsx/xlsxchartsheet_p.h \
    xlsx/xlsxcolor_p.h \
    xlsx/xlsxconditionalformatting.h \
    xlsx/xlsxconditionalformatting_p.h \
    xlsx/xlsxcontenttypes_p.h \
    xlsx/xlsxdatavalidation.h \
    xlsx/xlsxdatavalidation_p.h \
    xlsx/xlsxdocpropsapp_p.h \
    xlsx/xlsxdocpropscore_p.h \
    xlsx/xlsxdocument.h \
    xlsx/xlsxdocument_p.h \
    xlsx/xlsxdrawing_p.h \
    xlsx/xlsxdrawinganchor_p.h \
    xlsx/xlsxformat.h \
    xlsx/xlsxformat_p.h \
    xlsx/xlsxglobal.h \
    xlsx/xlsxmediafile_p.h \
    xlsx/xlsxnumformatparser_p.h \
    xlsx/xlsxrelationships_p.h \
    xlsx/xlsxrichstring.h \
    xlsx/xlsxrichstring_p.h \
    xlsx/xlsxsharedstrings_p.h \
    xlsx/xlsxsimpleooxmlfile_p.h \
    xlsx/xlsxstyles_p.h \
    xlsx/xlsxtheme_p.h \
    xlsx/xlsxutility_p.h \
    xlsx/xlsxworkbook.h \
    xlsx/xlsxworkbook_p.h \
    xlsx/xlsxworksheet.h \
    xlsx/xlsxworksheet_p.h \
    xlsx/xlsxzipreader_p.h \
    xlsx/xlsxzipwriter_p.h \
    ui/setting/serialport.h \
    ui/setting/hardware.h \
    ui/setting/permission.h \
    ui/controldock.h \
    ui/chargeditor.h \
    cpp_lib/csv/csvfile.h

RESOURCES += resource.qrc

SUBDIRS += \
    xlsx/xlsx.pro

DISTFILES += \
    xlsx/qtxlsx.pri


