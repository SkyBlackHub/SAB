#ifndef SAB_SOUNDS_H
#define SAB_SOUNDS_H

#include <QObject>
#include <QSoundEffect>
#include <QHash>

namespace SAB {

	enum Sound {
		Snapshot,
		Saved,
		Error,
		Warning
	};

	class Sounds : public QObject
	{
		Q_OBJECT
	public:
		explicit Sounds(QObject *parent = nullptr);
		~Sounds();

		struct Definition
		{
			SAB::Sound sound;
			QString key;
			QString caption;
			QString default_source;
			float default_volume;
		};

		void play(Sound sound);

		QSoundEffect *sound(Sound sound) const;

		static const QList<Definition> &definitions();

		void save();
		void load();

	signals:

	public slots:
		void snapshot();
		void saved();
		void error();
		void warning();

		void setSound(SAB::Sound sound, const QString &filename, float volume = 1.0);

		void setVolume(SAB::Sound sound, float volume);
		void setMuted(SAB::Sound sound, bool muted);

	private:
		QHash<Sound, QSoundEffect *> m_effects;
	};

} // namespace SAB

#endif // SAB_SOUNDS_H
