#include "forms/MainWindow.h"

#include "Application.h"

HHOOK keyboardHook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT) lParam;
	if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && nCode == HC_ACTION) {
		qApp->processNativeKey(key->vkCode, key->scanCode, key->flags, key->time);
	}
	return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

int main(int argc, char *argv[])
{
	QApplication::setStyle(QLatin1String("fusion"));

	SAB::Application application(argc, argv);

	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, NULL);

	application.sounds().load();

	SAB::MainWindow window;

	application.load();

	window.show();

	int result = application.exec();
	UnhookWindowsHookEx(keyboardHook);
	return result;
}