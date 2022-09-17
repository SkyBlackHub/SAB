VERSION = 0.1.0

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

RC_ICONS += $${PWD}/../resources/icons/icon.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# AUTODEPLOY BEGIN (Windows .exe only)
CONFIG(release, debug|release) {
	PROJECT_DIR = $${PWD}/..
	RUNTIME_DIR = $${PROJECT_DIR}/runtime
	DEPLOY_DIR = $${PROJECT_DIR}/deploy

	SOURCE_TARGET  = $${OUT_PWD}/release/$${TARGET}.exe
	DEPLOY_TARGET = $${DEPLOY_DIR}/$${TARGET}.exe

	# fix slashes
	PROJECT_DIR ~= s,/,\\,g
	RUNTIME_DIR ~= s,/,\\,g
	DEPLOY_DIR ~= s,/,\\,g

	SOURCE_TARGET ~= s,/,\\,g
	DEPLOY_TARGET ~= s,/,\\,g

	create.commands = $(CHK_DIR_EXISTS) $${DEPLOY_DIR} $(MKDIR) $${DEPLOY_DIR}
	QMAKE_EXTRA_TARGETS += create
	POST_TARGETDEPS += create

	QMAKE_POST_LINK += $$quote($(COPY) $${SOURCE_TARGET} $${DEPLOY_TARGET}$$escape_expand(\n\t))

	QMAKE_POST_LINK += $$quote($$(QTDIR)/bin/windeployqt --dir $${DEPLOY_DIR} --no-translations --plugindir $${DEPLOY_DIR}/plugins $${DEPLOY_TARGET}$$escape_expand(\n\t))

	QMAKE_POST_LINK += $$quote($(COPY_DIR) $${RUNTIME_DIR} $${DEPLOY_DIR}$$escape_expand(\n\t))
}
# AUTODEPLOY END

SOURCES += \
    Application.cpp \
    ImageSaver.cpp \
    Settings.cpp \
    Sounds.cpp \
    StateSettings.cpp \
    WindowsPlatform.cpp \
    forms/SnapshotsForm.cpp \
    forms/SoundsForm.cpp \
    main.cpp \
    forms/MainWindow.cpp \
    modules/AbstractModule.cpp \
    modules/SnapshotsModule.cpp \
    widgets/AboutLabel.cpp \
    widgets/LogWidget.cpp

HEADERS += \
    Application.h \
    Duplex.h \
    ImageSaver.h \
    Settings.h \
    Sounds.h \
    StateSettings.h \
    WindowsPlatform.h \
    forms/MainWindow.h \
    forms/SnapshotsForm.h \
    forms/SoundsForm.h \
    modules/AbstractModule.h \
    modules/SnapshotsModule.h \
    widgets/AboutLabel.h \
    widgets/LogWidget.h

FORMS += \
    forms/MainWindow.ui \
    forms/SnapshotsForm.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
  ../resources/icons.qrc
