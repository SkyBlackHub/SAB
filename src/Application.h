#ifndef SAB_APPLICATION_H
#define SAB_APPLICATION_H

#include <QApplication>

#include "Sounds.h"
#include "WindowsPlatform.h"

#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<SAB::Application *>(QCoreApplication::instance()))

namespace SAB {

	enum class Module
	{
		Clipboard
	};

	class ImageSaver;

	class AbstractModule;

	class Application : public QApplication
	{
	Q_OBJECT
	public:
		Application(int &argc, char **argv);
		virtual ~Application();

		void save();
		void load();

		void processNativeKey(DWORD vk_code, DWORD scan_code, DWORD flags, DWORD time);

		Sounds &sounds();
		Windows &windows();

		const QHash<Module, AbstractModule *> &modules() const;

	signals:
		void errorOccurred(const QString &message = QString());
		void warningOccurred(const QString &message = QString());

	public slots:

	private:
		QHash<Module, AbstractModule *> m_modules;

		Sounds m_sounds;
		Windows m_windows;
	};

} // namespace SAB

#endif // SAB_APPLICATION_H
