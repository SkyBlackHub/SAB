#include "Sounds.h"

#include "Settings.h"

SAB::Sounds::Sounds(QObject *parent) : QObject{parent}
{
	foreach (const Definition &definition, definitions()) {
		this->setSound(definition.sound, definition.default_source, definition.default_volume);
	}
}

SAB::Sounds::~Sounds()
{

	this->save();
}

void SAB::Sounds::play(Sound sound)
{
	if (QSoundEffect *effect = m_effects.value(sound)) {
		effect->play();
	}
}

QSoundEffect *SAB::Sounds::sound(Sound sound) const
{
	return m_effects.value(sound);
}

const QList<SAB::Sounds::Definition> &SAB::Sounds::definitions()
{
	static QList<Definition> definitions = {
		{SAB::Sound::Snapshot, "snapshot", "Snapshot Triggered", "sounds/snapshot.wav", 0.5f},
		{SAB::Sound::Saved,    "saved",    "File Saved",         "sounds/saved.wav",    0.5f},
		{SAB::Sound::Error,    "error",    "Error Occurred",     "sounds/error.wav",    0.5f},
		{SAB::Sound::Warning,  "warning",  "Warning Occurred",   "sounds/warning.wav",  0.5f}
	};
	return definitions;
}

void SAB::Sounds::save()
{
	Settings settings;
	foreach (const Definition &definition, definitions()) {
		QSoundEffect *sound = this->sound(definition.sound);
		if (sound == nullptr) {
			continue;
		}

		settings.beginGroup("Sounds-" + definition.key);

		settings.setValue("source", sound->source());
		settings.setValue("muted", sound->isMuted());
		settings.setValue("volume", sound->volume());

		settings.endGroup();
	}
}

void SAB::Sounds::load()
{
	Settings settings;
	foreach (const Definition &definition, definitions()) {
		QSoundEffect *sound = this->sound(definition.sound);
		if (sound == nullptr) {
			continue;
		}

		settings.beginGroup("Sounds-" + definition.key);

		if (settings.contains("source")) {
			sound->setSource(settings.value("source").toUrl());
		}
		if (settings.contains("muted")) {
			sound->setMuted(settings.value("muted").toBool());
		}
		if (settings.contains("volume")) {
			sound->setVolume(settings.value("volume").toFloat());
		}

		settings.endGroup();
	}
}

void SAB::Sounds::snapshot()
{
	this->play(Sound::Snapshot);
}

void SAB::Sounds::saved()
{
	this->play(Sound::Saved);
}

void SAB::Sounds::error()
{
	this->play(Sound::Error);
}

void SAB::Sounds::warning()
{
	this->play(Sound::Warning);
}

void SAB::Sounds::setSound(Sound sound, const QString &filename, float volume)
{
	QSoundEffect *effect = m_effects.value(sound);
	if (effect == nullptr) {
		m_effects[sound] = effect = new QSoundEffect(this);
	}

	effect->setSource(QUrl::fromLocalFile(filename));
	effect->setVolume(volume);
}

void SAB::Sounds::setVolume(Sound sound, float volume)
{
	if (QSoundEffect *effect = m_effects.value(sound)) {
		effect->setVolume(volume);
	}
}

void SAB::Sounds::setMuted(Sound sound, bool muted)
{
	if (QSoundEffect *effect = m_effects.value(sound)) {
		effect->setMuted(muted);
	}
}