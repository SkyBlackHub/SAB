#include "StateSettings.h"

#include <QStandardPaths>
#include <QMainWindow>

SAB::StateSettings::StateSettings() : QSettings(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/states.ini", QSettings::IniFormat)
{

}

void SAB::StateSettings::saveWindow(QWidget *window)
{
	this->beginGroup(window->objectName());

	this->setValue("_geometry", window->saveGeometry());
	if (const QMainWindow *main_window = qobject_cast<const QMainWindow *>(window)) {
		this->setValue("_state", main_window->saveState());
	}

	this->endGroup();
}

void SAB::StateSettings::loadWindow(QWidget *window)
{
	this->beginGroup(window->objectName());

	if (this->contains("_geometry")) {
		window->restoreGeometry(this->value("_geometry").toByteArray());
	}
	if (QMainWindow *main_window = qobject_cast<QMainWindow *>(window)) {
		if (this->contains("_state")) {
			main_window->restoreState(this->value("_state").toByteArray());
		}
	}
	this->endGroup();
}