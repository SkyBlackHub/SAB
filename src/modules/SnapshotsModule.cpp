#include "SnapshotsModule.h"

#include <QClipboard>
#include <QDateTime>
#include <QMimeData>
#include <QDir>
#include <QtMath>
#include <QMenu>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QSystemTrayIcon>

#include "Sounds.h"

#include "forms/SnapshotsForm.h"
#include "Duplex.h"

SAB::SnapshotsModule::SnapshotsModule(QObject *parent) : AbstractModule(parent)
{
	this->setObjectName("Snapshots");

	connect(this, &SnapshotsModule::snapshotTaken, &sounds, &Sounds::snapshot);
	connect(this, &SnapshotsModule::snapshotSaved, &sounds, &Sounds::saved);
}

SAB::SnapshotsModule::~SnapshotsModule()
{
	this->abortSavers();
}

QString SAB::SnapshotsModule::caption() const
{
	return QStringLiteral("Snapshots");
}

QWidget *SAB::SnapshotsModule::createForm()
{
	SnapshotsForm *form = new SnapshotsForm();
	form->configure(this);
	return form;
}

bool SAB::SnapshotsModule::isListenNativeKey() const
{
	return m_use_trigger;
}

void SAB::SnapshotsModule::processNativeKey(DWORD vk_code, DWORD /*scan_code*/, DWORD flags, DWORD /*time*/)
{
	if (vk_code == VK_SNAPSHOT) {
		this->trigger(flags & LLKHF_ALTDOWN);
	}
}

bool SAB::SnapshotsModule::isStatusSupported() const
{
	return true;
}

QIcon SAB::SnapshotsModule::statusDefaultIcon() const
{
	return QIcon(":/icons/download.png");
}

QString SAB::SnapshotsModule::statusDefaultToolTip() const
{
	return QStringLiteral("Now saving / queue of postponed.\nClick to abort.");
}

bool SAB::SnapshotsModule::prepareTray(QMenu *menu, QSystemTrayIcon *tray_icon)
{
	QAction *toggle_action = new QAction("Monitor clipboard", tray_icon);
	toggle_action->setCheckable(true);
	toggle_action->setChecked(this->monitorClipboard());

	duplex(toggle_action, this, &SAB::SnapshotsModule::monitorClipboardChanged, &SAB::SnapshotsModule::setMonitorClipboard, this->monitorClipboard());

	menu->addAction(toggle_action);

	return true;
}

void SAB::SnapshotsModule::startMonitor()
{
	if (m_monitor_clipboard) {
		return;
	}
	m_monitor_clipboard = true;

	QClipboard *clipboard = this->clipboard();
	if (clipboard) {
		connect(clipboard, &QClipboard::dataChanged, this, &SnapshotsModule::dataChanged);
	}

	emit monitorClipboardChanged(m_monitor_clipboard);
	emit iconChanged(QIcon(":/icons/eye.png"));
}

void SAB::SnapshotsModule::stopMonitor()
{
	if (m_monitor_clipboard == false) {
		return;
	}
	m_monitor_clipboard = false;

	QClipboard *clipboard = this->clipboard();
	if (clipboard) {
		disconnect(clipboard, &QClipboard::dataChanged, this, &SnapshotsModule::dataChanged);
	}

	emit monitorClipboardChanged(m_monitor_clipboard);
	emit iconChanged(QIcon());
}

void SAB::SnapshotsModule::abortSavers()
{
	foreach (ImageSaver *saver, m_queue) {
		delete saver;
	}
	m_queue.clear();
	foreach (ImageSaver *saver, m_savers) {
		saver->terminate();
		saver->wait();
		saver->deleteLater();
	}
	emit queueCountChanged(0);
	emit saversCountChanged(0);
	this->updateStatus();
}

void SAB::SnapshotsModule::statusClick()
{
	this->abortSavers();
}

void SAB::SnapshotsModule::dataChanged()
{
	if (m_use_trigger && m_triggered == false) {
		return;
	}

	this->snap();

	m_triggered = false;
	m_alt = false;
}

void SAB::SnapshotsModule::snap()
{
	QClipboard *clipboard = this->clipboard();
	if (clipboard == nullptr) {
		emit actionDenied("No buffer instance found.");
		return;
	}

	QImage image = clipboard->image();
	if (image.isNull()) {
		emit actionDenied("Buffer doesn't contain an image.");
		return;
	}

	Windows::Window &window = windows.window();

	WindowStatus status = m_window_statuses.value(window.handle);
	if (status == WindowStatus::Blacklist) {
		return;
	}

	QFileInfo executable = window.fileinfo;

	if (status == WindowStatus::None) {
		if (this->testFilters(executable) == false) {
			m_window_statuses[window.handle] = WindowStatus::Blacklist;
			return;
		}
		m_window_statuses[window.handle] = WindowStatus::Whitelist;
	}

	QString stamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss-zzz");

	QString title = window.title;
	QString folder = executable.dir().dirName();
	QString name = executable.completeBaseName();

	QString auto_name = title;

	if (title.isEmpty() || windows.isLegalName(title, 100) == false) {
		auto_name = folder;
	}

	QString filename = QString(m_pattern)
		.replace(QStringLiteral("%auto%"), auto_name)
		.replace(QStringLiteral("%title%"), title)
		.replace(QStringLiteral("%folder%"), folder)
		.replace(QStringLiteral("%name%"), name)
		.replace(QStringLiteral("%stamp%"), stamp) ;

	QDir path = QDir(m_path.isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) : m_path);
	filename = path.absoluteFilePath(filename) + this->formatExtension();

	QString root = QFileInfo(filename).absolutePath();
	if (QDir().mkpath(root) == false)  {
		emit errorOccurred("Can't create a folder: " + root);
		return;
	}

	emit snapshotTaken(image);

	ImageSaver *saver = new ImageSaver(image, filename, this);

	saver->setCompressionRate(m_compression_rate);
	saver->setQuality(m_quality);

	if (m_crop_window && m_alt == false) {
		saver->setCrop(true);
		saver->setCropRect(windows.windowRect());
	}

	connect(saver, &ImageSaver::saveFailed, this, &AbstractModule::errorOccurred);
	connect(saver, &ImageSaver::finished, this, [saver, this] () {
		emit snapshotSaved(saver->image(), saver->filename());

		m_savers.remove(saver);
		emit saversCountChanged(m_savers.count());
		saver->deleteLater();

		if (m_queue.size()) {
			ImageSaver *saver = m_queue.dequeue();
			emit queueCountChanged(m_queue.count());

			m_savers.insert(saver);
			saver->start();
			emit saversCountChanged(m_savers.count());
		}

		this->updateStatus();
	});

	if (m_savers.count() < m_maximum_savers)	{
		m_savers.insert(saver);
		saver->start();
		emit saversCountChanged(m_savers.count());
	} else {
		if (m_queue.count() >= m_maximum_queue) {
			emit actionDenied("The queue is overflowed, the image was discarded.");
			saver->deleteLater();
			return;
		}
		m_queue.enqueue(saver);
		emit queueCountChanged(m_queue.count());
	}

	this->updateStatus();
}

void SAB::SnapshotsModule::trigger(bool alt)
{
	m_triggered = true;
	m_alt = alt;
}

void SAB::SnapshotsModule::updateStatus()
{
	int savers_count = m_savers.count();
	int queue_count = m_queue.count();
	if (savers_count == 0) {
		emit statusVisibilityChanged(false);
		return;
	}
	QString text;
	if (queue_count) {
		text = QString("   %1 / %2").arg(savers_count).arg(queue_count);
	} else {
		text = QString("   %1").arg(savers_count);
	}
	emit statusTextChanged(text);
	emit statusVisibilityChanged(true);
}

bool SAB::SnapshotsModule::cropWindow() const
{
	return m_crop_window;
}

void SAB::SnapshotsModule::setCropWindow(bool crop_window)
{
	if (m_crop_window != crop_window) {
		m_crop_window = crop_window;
		emit cropWindowChanged(m_crop_window);
	}
}

const QString &SAB::SnapshotsModule::path() const
{
	return m_path;
}

void SAB::SnapshotsModule::setPath(const QString &path)
{
	QString new_path = path.trimmed();
	if (m_path != new_path) {
		m_path = new_path;
		emit pathChanged(new_path);
	}
}

const QString &SAB::SnapshotsModule::pattern() const
{
	return m_pattern;
}

void SAB::SnapshotsModule::setPattern(const QString &pattern)
{
	if (m_pattern != pattern) {
		m_pattern = pattern;
		emit patternChanged(m_pattern);
	}
}

bool SAB::SnapshotsModule::useTrigger() const
{
	return m_use_trigger;
}

void SAB::SnapshotsModule::setUseTrigger(bool use_trigger)
{
	if (m_use_trigger != use_trigger) {
		m_use_trigger = use_trigger;
		emit useTriggerChanged(m_use_trigger);
	}
}

bool SAB::SnapshotsModule::monitorClipboard() const
{
	return m_monitor_clipboard;
}

void SAB::SnapshotsModule::setMonitorClipboard(bool monitor_clipboard)
{
	if (m_monitor_clipboard != monitor_clipboard) {
		if (monitor_clipboard) {
			this->startMonitor();
		} else {
			this->stopMonitor();
		}
	}
}

int SAB::SnapshotsModule::maximumSavers() const
{
	return m_maximum_savers;
}

void SAB::SnapshotsModule::setMaximumSavers(int maximum_savers)
{
	maximum_savers = qBound(1, maximum_savers, 20);
	if (m_maximum_savers != maximum_savers) {
		m_maximum_savers = maximum_savers;
		emit maximumSaversChanged(m_maximum_savers);
	}
}

int SAB::SnapshotsModule::maximumQueue() const
{
	return m_maximum_queue;
}

void SAB::SnapshotsModule::setMaximumQueue(int maximum_queue)
{
	maximum_queue = qBound(1, maximum_queue, 20);
	if (m_maximum_queue != maximum_queue) {
		m_maximum_queue = maximum_queue;
		emit maximumQueueChanged(m_maximum_queue);
	}
}

SAB::SnapshotsModule::ImageFormat SAB::SnapshotsModule::format() const
{
	return m_format;
}

void SAB::SnapshotsModule::setFormat(SAB::SnapshotsModule::ImageFormat format)
{
	if (m_format != format) {
		m_format = format;
		emit formatChanged(m_format);
	}
}

QString SAB::SnapshotsModule::formatCode() const
{
	switch (m_format) {
		case ImageFormat::BMP:
			return QStringLiteral("BMP");

		case ImageFormat::PNG:
			return QStringLiteral("PNG");

		case ImageFormat::JPEG:
			return QStringLiteral("JPEG");
	}
	return QString();
}

void SAB::SnapshotsModule::setFormatCode(const QString &format)
{
	if (format == "BMP") {
		this->setFormat(ImageFormat::BMP);
	} else if (format == "PNG") {
		this->setFormat(ImageFormat::PNG);
	}	else if (format == "JPEG") {
		this->setFormat(ImageFormat::JPEG);
	}
}

QString SAB::SnapshotsModule::formatExtension() const
{
	switch (m_format) {
		case ImageFormat::BMP:
			return QStringLiteral(".bmp");

		case ImageFormat::PNG:
			return QStringLiteral(".png");

		case ImageFormat::JPEG:
			return QStringLiteral(".jpg");
	}
	return QString();
}

bool SAB::SnapshotsModule::isPNG() const
{
	return m_format == ImageFormat::PNG;
}

bool SAB::SnapshotsModule::isJPEG() const
{
	return m_format == ImageFormat::JPEG;
}

bool SAB::SnapshotsModule::isBMP() const
{
	return m_format == ImageFormat::BMP;
}

int SAB::SnapshotsModule::compressionRate() const
{
	return m_compression_rate;
}

void SAB::SnapshotsModule::setCompressionRate(int compression_rate)
{
	compression_rate = qBound(0, compression_rate, 100);
	if (m_compression_rate != compression_rate) {
		m_compression_rate = compression_rate;
		emit compressionRateChanged(m_compression_rate);
	}
}

int SAB::SnapshotsModule::quality() const
{
	return m_quality;
}

void SAB::SnapshotsModule::setQuality(int quality)
{
	if (m_quality != quality) {
		m_quality = quality;
		emit qualityChanged(m_quality);
	}
}

static void assignFilters(const QStringList &list, QList<QPair<QString, QRegularExpression> > &filters)
{
	filters.clear();
	foreach (const QString &line, list) {
		QString pattern = line.trimmed();
		if (pattern.isEmpty() || pattern.startsWith('#')) {
			continue;
		}
		QRegularExpression expression = QRegularExpression::fromWildcard(pattern);
		QString compiled_pattern = expression.pattern();
		compiled_pattern.replace("[^/\\\\]*", ".*");
		expression.setPattern(compiled_pattern);
		filters.append({pattern, expression});
	}
}

const QStringList &SAB::SnapshotsModule::exclude() const
{
	return m_exclude;
}

void SAB::SnapshotsModule::setExclude(const QStringList &exclude)
{
	if (m_exclude != exclude) {
		m_exclude = exclude;
		assignFilters(m_exclude, this->m_exclude_filters);
		m_window_statuses.clear();
		emit excludeChanged(m_exclude);
	}
}

const QStringList &SAB::SnapshotsModule::include() const
{
	return m_include;
}

void SAB::SnapshotsModule::setInclude(const QStringList &include)
{
	if (m_include != include) {
		m_include = include;
		assignFilters(m_include, this->m_include_filters);
		m_window_statuses.clear();
		emit includeChanged(m_include);
	}
}

static QString _testFilters(const QFileInfo &fileinfo, const QList<QPair<QString, QRegularExpression> > &filters)
{
	QString result;
	foreach (const auto &filter, filters) {
		if (filter.second.match(fileinfo.absoluteFilePath()).hasMatch()) {
			result = filter.first;
			break;
		}
		if (filter.second.match(fileinfo.fileName()).hasMatch()) {
			result = filter.first;
			break;
		}
	}
	return result;
}

bool SAB::SnapshotsModule::testFilters(const QFileInfo &fileinfo, QString *filter)
{
	if (m_exclude.size() == 0 && m_include.size() == 0) {
		if (filter) {
			*filter = QString();
		}
		return true;
	}

	QString excluded_by = _testFilters(fileinfo, m_exclude_filters);
	QString included_by = _testFilters(fileinfo, m_include_filters);

	if (included_by.isNull() == false) {
		if (filter) {
			*filter = included_by;
		}
		return true;
	} else if (excluded_by.isNull() == false) {
		if (filter) {
			*filter = excluded_by;
		}
		return false;
	}

	if (filter) {
		*filter = QString();
	}
	return !this->hasIncludeFilters();
}

bool SAB::SnapshotsModule::hasExcludeFilters() const
{
	return m_exclude_filters.size() > 0;
}

bool SAB::SnapshotsModule::hasIncludeFilters() const
{
	return m_include_filters.size() > 0;
}