#ifndef SAB_IMAGESAVER_H
#define SAB_IMAGESAVER_H

#include <QThread>
#include <QImage>
#include <QFile>

namespace SAB {

	class ImageSaver : public QThread
	{
		Q_OBJECT
	public:
		ImageSaver(const QImage &image, const QString &filename, QObject *parent = nullptr);

		const QImage &image() const;
		QString filename() const;

		int quality() const;
		void setQuality(int quality);

		int compressionRate() const;
		void setCompressionRate(int compression_rate);

		const char *format() const;
		void setFormat(char *format);

		bool crop() const;
		void setCrop(bool crop);

		const QRect &cropRect() const;
		void setCropRect(const QRect &crop_rect);

	signals:
		void saveFailed(const QString &message);

	protected:
		virtual void run() override;

	private:
		QImage m_image;
		QFile m_file;
		int m_compression_rate = -1;
		int m_quality = -1;
		QByteArray m_format;
		bool m_crop = false;
		QRect m_crop_rect;
	};

} // namespace SAB

#endif // SAB_IMAGESAVER_H
