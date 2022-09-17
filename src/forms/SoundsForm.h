#ifndef SAB_SOUNDSFORM_H
#define SAB_SOUNDSFORM_H

#include <QWidget>

namespace SAB {

	class SoundsForm : public QWidget
	{
		Q_OBJECT

	public:
		explicit SoundsForm(QWidget *parent = nullptr);
		~SoundsForm();

	private:

	};

} // namespace SAB

#endif // SAB_SOUNDSFORM_H
