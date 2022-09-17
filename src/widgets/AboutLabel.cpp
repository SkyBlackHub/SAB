#include "AboutLabel.h"

#include <QMovie>
#include <QPainter>
#include <QStyle>

SAB::AboutLabel::AboutLabel(QWidget *parent) : QLabel(parent)
{
	this->setScaledContents(true);

	QMovie *movie = new QMovie(":/images/about");
	this->setMovie(movie);
	movie->start();
}

void SAB::AboutLabel::paintEvent(QPaintEvent *)
{
	QStyle *style = QWidget::style();
	QPainter painter(this);
	this->drawFrame(&painter);
	QRect rect = contentsRect();

	QPixmap pixmap = this->movie()->currentPixmap();

	if (pixmap.isNull() == false) {
		qreal aspect = (qreal) pixmap.width() / pixmap.height();
		if ((qreal) rect.width() / rect.height() > aspect) {
			pixmap = pixmap.scaledToHeight(rect.height());
		} else {
			pixmap = pixmap.scaledToWidth(rect.width());
		}
	}

	//painter.fillRect(rect, QColor(24, 18, 51));
	style->drawItemPixmap(&painter, rect, Qt::AlignTop | Qt::AlignHCenter, pixmap);
}