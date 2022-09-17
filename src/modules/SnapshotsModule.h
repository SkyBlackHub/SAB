#ifndef SAB_SNAPSHOTSMODULE_H
#define SAB_SNAPSHOTSMODULE_H

#include <QQueue>
#include <QSet>

#include "AbstractModule.h"
#include "ImageSaver.h"

namespace SAB {

	class SnapshotsModule : public AbstractModule
	{
		Q_OBJECT
		Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
		Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)

		Q_PROPERTY(bool use_trigger READ useTrigger WRITE setUseTrigger NOTIFY useTriggerChanged)
		Q_PROPERTY(bool crop_window READ cropWindow WRITE setCropWindow NOTIFY cropWindowChanged)
		Q_PROPERTY(bool monitor_clipboard READ monitorClipboard WRITE setMonitorClipboard NOTIFY monitorClipboardChanged)

		Q_PROPERTY(int maximum_savers READ maximumSavers WRITE setMaximumSavers NOTIFY maximumSaversChanged)
		Q_PROPERTY(int maximum_queue READ maximumQueue WRITE setMaximumQueue NOTIFY maximumQueueChanged)

		Q_PROPERTY(QString format READ formatCode WRITE setFormatCode NOTIFY formatChanged)
		Q_PROPERTY(int compression_rate READ compressionRate WRITE setCompressionRate NOTIFY compressionRateChanged)
		Q_PROPERTY(int quality READ quality WRITE setQuality NOTIFY qualityChanged)

		Q_PROPERTY(QStringList exclude READ exclude WRITE setExclude NOTIFY excludeChanged)
		Q_PROPERTY(QStringList include READ include WRITE setInclude NOTIFY includeChanged)
	public:

		enum class ImageFormat
		{
			PNG,
			BMP,
			JPEG
		};
		Q_ENUM(ImageFormat)

		explicit SnapshotsModule(QObject *parent = nullptr);
		virtual ~SnapshotsModule() override;

		virtual QString caption() const override;
		virtual QWidget *createForm() override;

		virtual bool isListenNativeKey() const override;
		virtual void processNativeKey(DWORD /*vk_code*/, DWORD /*scan_code*/, DWORD /*flags*/, DWORD /*time*/) override;

		virtual bool isStatusSupported() const override;
		virtual QIcon statusDefaultIcon() const override;
		virtual QString statusDefaultToolTip() const override;

		virtual bool prepareTray(QMenu *menu, QSystemTrayIcon *tray_icon) override;

		const QString &path() const;
		void setPath(const QString &path);

		const QString &pattern() const;
		void setPattern(const QString &pattern);

		bool useTrigger() const;
		void setUseTrigger(bool use_trigger);

		bool monitorClipboard() const;
		void setMonitorClipboard(bool monitor_clipboard);

		int maximumSavers() const;
		void setMaximumSavers(int maximum_savers);

		int maximumQueue() const;
		void setMaximumQueue(int maximum_queue);

		ImageFormat format() const;
		void setFormat(ImageFormat format);
		QString formatCode() const;
		void setFormatCode(const QString &format);
		QString formatExtension() const;

		bool isPNG() const;
		bool isJPEG() const;
		bool isBMP() const;

		int compressionRate() const;
		void setCompressionRate(int compression_rate);

		int quality() const;
		void setQuality(int quality);

		const QStringList &exclude() const;
		void setExclude(const QStringList &exclude);

		const QStringList &include() const;
		void setInclude(const QStringList &include);

		bool testFilters(const QFileInfo &fileinfo, QString *filter = nullptr);
		bool hasExcludeFilters() const;
		bool hasIncludeFilters() const;

		bool cropWindow() const;
		void setCropWindow(bool crop_window);

	signals:
		void snapshotTaken(const QImage &image);
		void snapshotSaved(const QImage &image, const QString &filename);

		void pathChanged(const QString &path);
		void patternChanged(const QString &pattern);

		void useTriggerChanged(bool use_trigger);
		void monitorClipboardChanged(bool monitor_clipboard);

		void maximumSaversChanged(uint maximum_savers);

		void saversCountChanged(int count);
		void queueCountChanged(int count);

		void qualityChanged(int quality);

		void excludeChanged(const QStringList &exclude);
		void includeChanged(const QStringList &include);

		void compressionRateChanged(int compression_rate);

		void maximumQueueChanged(int maximum_queue);

		void formatChanged(SAB::SnapshotsModule::ImageFormat format);

		void cropWindowChanged(bool crop_window);

	public slots:
		void startMonitor();
		void stopMonitor();

		void snap();
		void abortSavers();

		virtual void statusClick() override;

	protected slots:
		void dataChanged();

	protected:
		void trigger(bool alt);

		void updateStatus();

	private:
		enum WindowStatus {
			None,
			Whitelist,
			Blacklist
		};

		QHash<HWND, WindowStatus> m_window_statuses;

		QString m_path;
		QString m_pattern = QStringLiteral("%name%/%stamp%");

		bool m_use_trigger = true;
		bool m_crop_window = false;

		bool m_triggered = false;
		bool m_alt = false;

		bool m_monitor_clipboard = false;

		int m_maximum_savers = 4;
		int m_maximum_queue = 12;

		ImageFormat m_format = ImageFormat::PNG;
		int m_compression_rate = 50;
		int m_quality = 80;

		QStringList m_exclude;
		QStringList m_include;

		QList<QPair<QString, QRegularExpression> > m_exclude_filters;
		QList<QPair<QString, QRegularExpression> > m_include_filters;

		QQueue<ImageSaver *> m_queue;
		QSet<ImageSaver *> m_savers;
	};

} // namespace SAB

#endif // SAB_SNAPSHOTSMODULE_H
