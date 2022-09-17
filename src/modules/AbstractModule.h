#ifndef SAB_ABSTRACTMODULE_H
#define SAB_ABSTRACTMODULE_H

#include <QIcon>

#include "WindowsPlatform.h"

class QClipboard;
class QMenu;
class QSystemTrayIcon;

namespace SAB {

	class Sounds;
	class Windows;

	class AbstractModule : public QObject
	{
		Q_OBJECT
	public:
		explicit AbstractModule(QObject *parent = nullptr);

		virtual QString caption() const = 0;
		virtual QWidget *createForm() = 0;

		virtual inline bool isListenNativeKey() const { return false; }
		virtual inline void processNativeKey(DWORD /*vk_code*/, DWORD /*scan_code*/, DWORD /*flags*/, DWORD /*time*/) { }

		virtual inline bool isStatusSupported() const { return false; }
		virtual inline bool statusDefaultVisibility() const { return false; }
		virtual inline QString statusDefaultToolTip() const { return QString(); }
		virtual inline QIcon statusDefaultIcon() const { return QIcon(); }

		virtual inline bool prepareTray(QMenu */*menu*/, QSystemTrayIcon */*tray_icon*/) { return false; }

	signals:
		void errorOccurred(const QString &message);
		void actionDenied(const QString &message = QString());
		void iconChanged(const QIcon &icon);

		void statusVisibilityChanged(bool visible);
		void statusTextChanged(const QString &text);
		void statusIconChanged(const QIcon &icon);

	public slots:
		virtual inline void statusClick() { }

	protected:
		Sounds &sounds;
		Windows &windows;

		static QClipboard *clipboard();

	private:

	};

} // namespace SAB

#endif // SAB_ABSTRACTMODULE_H
