#ifndef SAB_WINDOWS_H
#define SAB_WINDOWS_H

#include <qt_windows.h>
#pragma comment(lib,"user32.lib")

#include <QHash>
#include <QRect>
#include <QFileInfo>

namespace SAB {

	class Windows
	{
	public:
		Windows();

		struct Window
		{
			HWND handle = 0;
			QString title;
			QFileInfo fileinfo;
		};

		static bool isLegalName(const QString &name, uint max_length = 260);

		static QString getTitle();
		static QString getTitle(HWND window_handle);

		static QRect getRect();
		static QRect getRect(HWND window_handle);

		static QString getBasename();
		static QString getBasename(HWND window_handle);
		static QString getBasename(HANDLE handle);

		static QString getFolder();
		static QString getFolder(HWND window_handle);
		static QString getFolder(HANDLE handle);

		static QFileInfo getFileinfo();
		static QFileInfo getFileinfo(HWND window_handle);
		static QFileInfo getFileinfo(HANDLE handle);

		Window &window();
		Window &window(HWND handle);

		QString windowTitle();
		QFileInfo windowFileinfo();
		QRect windowRect();

	protected:
		static HANDLE openHandle();
		static HANDLE openHandle(HWND window_handle);
		static void closeHandle(HANDLE handle);

	private:
		QHash<HWND, Window> m_foregrounds;
	};

} // namespace SAB

#endif // SAB_WINDOWS_H
