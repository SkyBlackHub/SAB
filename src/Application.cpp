#include "Application.h"

#include "modules/SnapshotsModule.h"

#include "Settings.h"

SAB::Application::Application(int &argc, char **argv) : QApplication(argc, argv)
{
	this->setApplicationName("Snap Assistant Bot");
	this->setOrganizationName("Sky Black");
	this->setApplicationVersion(APP_VERSION);
	this->setOrganizationDomain("skyblack.org");

	connect(this, &Application::errorOccurred, &m_sounds, &Sounds::error);
	connect(this, &Application::warningOccurred, &m_sounds, &Sounds::warning);

	m_modules[Module::Clipboard] = new SnapshotsModule(this);

	foreach (AbstractModule *module, m_modules) {
		connect(module, &AbstractModule::errorOccurred, this, &Application::errorOccurred);
		connect(module, &AbstractModule::actionDenied, this, &Application::warningOccurred);
	}
}

SAB::Application::~Application()
{
	this->save();
}

void SAB::Application::save()
{
	Settings settings;
	foreach (AbstractModule *module, m_modules) {
		settings.saveQObject(module);
	}
}

void SAB::Application::load()
{
	Settings settings;
	foreach (AbstractModule *module, m_modules) {
		settings.loadQObject(module);
	}
}

void SAB::Application::processNativeKey(DWORD vk_code, DWORD scan_code, DWORD flags, DWORD time)
{
	foreach (AbstractModule *module, m_modules) {
		if (module->isListenNativeKey()) {
			module->processNativeKey(vk_code, scan_code, flags, time);
		}
	}
}

SAB::Sounds &SAB::Application::sounds()
{
	return m_sounds;
}

SAB::Windows &SAB::Application::windows()
{
	return m_windows;
}

const QHash<SAB::Module, SAB::AbstractModule *> &SAB::Application::modules() const
{
	return m_modules;
}