#ifndef SAB_SETTINGS_H
#define SAB_SETTINGS_H

#include <QSettings>

namespace SAB {

	class Settings : public QSettings
	{
	public:
		Settings();

		void saveQObject(QObject *module);
		void loadQObject(QObject *module);
	};

} // namespace SAB

#endif // SAB_SETTINGS_H
