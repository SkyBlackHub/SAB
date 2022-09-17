#ifndef SAB_MAINWINDOW_H
#define SAB_MAINWINDOW_H

#include <QMainWindow>

class QSystemTrayIcon;

namespace SAB {

	namespace Ui { class MainWindow; }

	class MainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
		MainWindow(QWidget *parent = nullptr);
		~MainWindow();

	protected slots:

	private:
		Ui::MainWindow *ui;

		QSystemTrayIcon *m_tray_icon;
	};

} // namespace SAB
#endif // SAB_MAINWINDOW_H
