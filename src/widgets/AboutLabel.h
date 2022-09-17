#ifndef SAB_ABOUTLABEL_H
#define SAB_ABOUTLABEL_H

#include <QLabel>

namespace SAB {

	class AboutLabel : public QLabel
	{
		Q_OBJECT
	public:
		explicit AboutLabel(QWidget *parent);

	protected:
		void paintEvent(QPaintEvent *) override;

	private:
	};

} // namespace SAB

#endif // SAB_ABOUTLABEL_H