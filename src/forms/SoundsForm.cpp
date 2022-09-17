#include "SoundsForm.h"

#include "Sounds.h"

#include <Application.h>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QFileDialog>

SAB::SoundsForm::SoundsForm(QWidget *parent) :	QWidget(parent)
{
	QGridLayout *layout = new QGridLayout(this);
	layout->setVerticalSpacing(20);

	Sounds &sounds = qApp->sounds();

	int row = -1;

	foreach (const Sounds::Definition &definition, Sounds::definitions()) {
		++row;
		int column = -1;

		QSoundEffect *sound = sounds.sound(definition.sound);
		if (sound == nullptr) {
			continue;
		}

		QLabel *name = new QLabel(definition.caption + ":", this);

		layout->addWidget(name, row, ++column);

		QLineEdit *path = new QLineEdit(this);
		path->setText(sound->source().path());

		connect(path, &QLineEdit::textChanged, sound, [sound] (const QString &value) {
			sound->setSource(QUrl::fromLocalFile(value));
		});

		layout->addWidget(path, row, ++column);

		QPushButton *path_browse = new QPushButton(this);
		{
			QIcon icon;
			icon.addFile(":/icons/folder_classic.png", QSize(), QIcon::Normal, QIcon::Off);
			path_browse->setIcon(icon);
		}
		path_browse->setToolTip("Browse...");

		connect(path_browse, &QPushButton::clicked, this, [this, path] () {
			QFileDialog dialog(this);
			dialog.setNameFilter("Sound files (*.wav)");
			if (dialog.exec()) {
				path->setText(dialog.selectedFiles().constFirst());
			}
		});

		layout->addWidget(path_browse, row, ++column);

		QPushButton *play = new QPushButton(this);
		{
			QIcon icon;
			icon.addFile(":/icons/media_controls_play_small.png", QSize(), QIcon::Normal, QIcon::Off);
			play->setIcon(icon);
		}
		play->setToolTip("Play sound");

		connect(play, &QPushButton::clicked, sound, &QSoundEffect::play);

		layout->addWidget(play, row, ++column);

		QCheckBox *mute = new QCheckBox("Mute", this);
		mute->setChecked(sound->isMuted());

		connect(mute, &QCheckBox::toggled, sound, &QSoundEffect::setMuted);

		layout->addWidget(mute, row, ++column);

		QSlider *volume = new QSlider(this);
		volume->setOrientation(Qt::Horizontal);
		volume->setMaximum(100);
		volume->setValue(sound->volume() * 100);

		connect(volume, &QSlider::valueChanged, sound, [sound] (int value) {
			sound->setVolume((float) value / 100);
		});

		layout->addWidget(volume, row, ++column);
	}

	layout->setRowStretch(row + 1, 1);
}

SAB::SoundsForm::~SoundsForm()
{

}
