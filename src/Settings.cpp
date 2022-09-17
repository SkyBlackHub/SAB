#include "Settings.h"

#include <QStandardPaths>
#include <QMetaProperty>

SAB::Settings::Settings() : QSettings(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/settings.ini", QSettings::IniFormat)
{

}

void SAB::Settings::saveQObject(QObject *module)
{
	this->beginGroup(module->objectName());

	const QMetaObject *meta_object = module->metaObject();
	int count = meta_object->propertyCount();
	for (int i = 1; i < count; ++i) {
		QMetaProperty property = meta_object->property(i);
		if (property.isReadable() == false || property.isStored() == false) {
			continue;
		}
		this->setValue(property.name(), property.read(module));
	}

	this->endGroup();
}

void SAB::Settings::loadQObject(QObject *module)
{
	this->beginGroup(module->objectName());

	const QMetaObject *meta_object = module->metaObject();
	int count = meta_object->propertyCount();
	for (int i = 1; i < count; ++i) {
		QMetaProperty property = meta_object->property(i);
		if (property.isWritable() == false || property.isStored() == false) {
			continue;
		}
		if (this->contains(property.name())) {
			property.write(module, this->value(property.name()));
		}
	}

	this->endGroup();
}

