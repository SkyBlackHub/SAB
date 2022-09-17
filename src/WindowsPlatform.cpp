#include "WindowsPlatform.h"

#include <Psapi.h>
#include <QDir>

SAB::Windows::Windows()
{

}

bool SAB::Windows::isLegalName(const QString &name, uint max_length)
{
	static const QString illegal_chars = "<>:\"|?*";
	static const QStringList devices = {
		"CON", "PRN", "AUX", "NUL",
		"COM0", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
		"LPT0", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8",  "LPT9"
	};

	if (devices.contains(name)) {
		return false;
	}

  foreach (const QChar &c, name) {
    // Check for control characters
		if (c.toLatin1() >= 0 && c.toLatin1() < 32) {
      return false;
		}
    // Check for illegal characters
		if (illegal_chars.contains(c)) {
      return false;
		}
  }

  // Check for trailing periods or spaces
  if (name.right(1) == "." || name.right(1) == " ")
    return false;

  // Check for pathnames that are too long (disregarding raw pathnames)
  if (name.length() > max_length)
    return false;

  // Exclude raw device names
  if (name.left(4) == "\\\\.\\")
    return false;

  // Since we are checking for a filename, it mustn't be a directory
  if (name.right(1) == "\\")
    return false;

	return true;
}

HANDLE SAB::Windows::openHandle()
{
	HWND foreground = GetForegroundWindow();
	if (foreground == 0) {
		return nullptr;
	}

	return openHandle(foreground);
}

HANDLE SAB::Windows::openHandle(HWND window_handle)
{
	DWORD pid;
	GetWindowThreadProcessId(window_handle, &pid);

	return OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pid);
}

void SAB::Windows::closeHandle(HANDLE handle)
{
	CloseHandle(handle);
}

QString SAB::Windows::getTitle()
{
	return getTitle(GetForegroundWindow());
}

QString SAB::Windows::getTitle(HWND window_handle)
{
	if (window_handle == 0) {
		return QString();
	}
	int length = GetWindowTextLength(window_handle) + 1;
	WCHAR *title = new WCHAR[length];
	GetWindowText(window_handle, title, length);
	return QString::fromWCharArray(title);
	delete[] title;
}

QRect SAB::Windows::getRect()
{
	return getRect(GetForegroundWindow());
}

QRect SAB::Windows::getRect(HWND window_handle)
{
	RECT rect;
	GetClientRect(window_handle, &rect);
	--rect.right;
	--rect.bottom;
	ClientToScreen(window_handle, reinterpret_cast<POINT *>(&rect.left));
	ClientToScreen(window_handle, reinterpret_cast<POINT *>(&rect.right));
	return QRect(QPoint(rect.left, rect.top), QPoint(rect.right, rect.bottom));
}

QString SAB::Windows::getBasename()
{
	HANDLE handle = openHandle();

	QString result = getBasename(handle);

	CloseHandle(handle);

	return result;
}

QString SAB::Windows::getBasename(HWND window_handle)
{
	HANDLE handle = openHandle(window_handle);

	QString result = getBasename(handle);

	CloseHandle(handle);

	return result;
}

QString SAB::Windows::getBasename(HANDLE handle)
{
	if (handle == nullptr) {
		return QString();
	}

	WCHAR buffer[MAX_PATH];
	QString result;
	if (GetModuleBaseName(handle, nullptr, buffer, MAX_PATH)) {
		result = QString::fromWCharArray(buffer);
		if (result.endsWith(".exe", Qt::CaseInsensitive)) {
			result.chop(4);
		}
	}

	return result;
}

QString SAB::Windows::getFolder()
{
	HANDLE handle = openHandle();

	QString result = getFolder(handle);

	CloseHandle(handle);

	return result;
}

QString SAB::Windows::getFolder(HWND window_handle)
{
	HANDLE handle = openHandle(window_handle);

	QString result = getFolder(handle);

	CloseHandle(handle);

	return result;
}

QString SAB::Windows::getFolder(HANDLE handle)
{
	if (handle == nullptr) {
		return QString();
	}

	WCHAR buffer[MAX_PATH];
	if (GetProcessImageFileName(handle, buffer, MAX_PATH)) {
		return QFileInfo(QString::fromWCharArray(buffer)).dir().dirName();
	}
	return QString();
}

QFileInfo SAB::Windows::getFileinfo()
{
	HANDLE handle = openHandle();

	QFileInfo result = getFileinfo(handle);

	CloseHandle(handle);

	return result;
}

QFileInfo SAB::Windows::getFileinfo(HWND window_handle)
{
	HANDLE handle = openHandle(window_handle);

	QFileInfo result = getFileinfo(handle);

	CloseHandle(handle);

	return result;
}

QFileInfo SAB::Windows::getFileinfo(HANDLE handle)
{
	if (handle == nullptr) {
		return QFileInfo();
	}

	WCHAR buffer[MAX_PATH];
	DWORD size = MAX_PATH;
	if (QueryFullProcessImageName(handle, 0, buffer, &size)) {
		return QFileInfo(QString::fromWCharArray(buffer));
	}
	return QFileInfo();
}

SAB::Windows::Window &SAB::Windows::window()
{
	return window(GetForegroundWindow());
}

SAB::Windows::Window &SAB::Windows::window(HWND handle)
{
	Window &foreground = m_foregrounds[handle];
	if (foreground.handle == 0) {
		foreground.handle = handle;
		foreground.title = getTitle(handle).trimmed();
		foreground.fileinfo = getFileinfo(handle);
	}
	return foreground;
}

QString SAB::Windows::windowTitle()
{
	return window().title;
}

QFileInfo SAB::Windows::windowFileinfo()
{
	return window().fileinfo;
}

QRect SAB::Windows::windowRect()
{
	return getRect(GetForegroundWindow());
}
