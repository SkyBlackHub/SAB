#ifndef SAB_STATESETTINGS_H
#define SAB_STATESETTINGS_H

#include <QSettings>

namespace SAB {

	class StateSettings : public QSettings
	{
	public:
		StateSettings();

		void saveWindow(QWidget *window);
		void loadWindow(QWidget *window);
	};

} // namespace SAB

#endif // SAB_STATESETTINGS_H
