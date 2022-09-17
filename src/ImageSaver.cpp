#include "ImageSaver.h"

#include <QImageWriter>

SAB::ImageSaver::ImageSaver(const QImage &image, const QString &filename, QObject *parent) : QThread(parent), m_image(image), m_file(filename)
{

}

const QImage &SAB::ImageSaver::image() const
{
	return m_image;
}

QString SAB::ImageSaver::filename() const
{
	return m_file.fileName();
}

void SAB::ImageSaver::run()
{
	if (m_file.open(QIODevice::WriteOnly) == false) {
		emit saveFailed("An error occurred when opening the file for writing: " + m_file.fileName() + "\n" + m_file.errorString());
		return;
	}
	QImageWriter writer(&m_file, m_format);

	writer.setCompression(m_compression_rate);
	writer.setQuality(m_quality);

	bool result;
	if (m_crop) {
		result = writer.write(m_image.copy(m_crop_rect));
	} else {
		result = writer.write(m_image);
	}

	if (result == false) {
		emit saveFailed("Save failed for the file: " + m_file.fileName() + "\n" + writer.errorString());
	}

	m_file.close();
}

bool SAB::ImageSaver::crop() const
{
	return m_crop;
}

void SAB::ImageSaver::setCrop(bool crop)
{
	m_crop = crop;
}

const QRect &SAB::ImageSaver::cropRect() const
{
	return m_crop_rect;
}

void SAB::ImageSaver::setCropRect(const QRect &crop_rect)
{
	m_crop_rect = crop_rect;
}

int SAB::ImageSaver::compressionRate() const
{
	return m_compression_rate;
}

void SAB::ImageSaver::setCompressionRate(int compression_rate)
{
	m_compression_rate = compression_rate;
}

int SAB::ImageSaver::quality() const
{
	return m_quality;
}

void SAB::ImageSaver::setQuality(int quality)
{
	m_quality = quality;
}

const char *SAB::ImageSaver::format() const
{
	return m_format;
}

void SAB::ImageSaver::setFormat(char *format)
{
	m_format = format;
}