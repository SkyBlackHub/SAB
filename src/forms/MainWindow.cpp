#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Application.h"
#include "modules/AbstractModule.h"
#include "StateSettings.h"
#include "forms/SoundsForm.h"

#include <QTabBar>
#include <QSystemTrayIcon>
#include <QToolButton>

static const int initial_year = 2022;

SAB::MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	this->setWindowTitle(qApp->applicationName());

	int index = 0;
	foreach (AbstractModule *module, qApp->modules()) {
		ui->MainTabs->insertTab(index, module->createForm(), module->caption());
		connect(module, &AbstractModule::iconChanged, this, [this, index] (const QIcon &icon) {
			ui->MainTabs->setTabIcon(index, icon);
		});
		if (module->isStatusSupported()) {
			QPushButton *button = new QPushButton();
			button->setFlat(true);
			button->setIcon(module->statusDefaultIcon());
			button->setToolTip(module->statusDefaultToolTip());
			button->setVisible(module->statusDefaultVisibility());
			ui->StatusBar->addWidget(button);
			connect(button, &QPushButton::clicked, module, &AbstractModule::statusClick);
			connect(module, &AbstractModule::statusTextChanged, button, &QPushButton::setText);
			connect(module, &AbstractModule::statusIconChanged, button, &QPushButton::setIcon);
			connect(module, &AbstractModule::statusVisibilityChanged, button, &QPushButton::setVisible);
		}
		++index;
	}

	ui->MainTabs->insertTab(index, new SoundsForm(), "Sounds");

	//ui->ModulesTabs->tabBar()->moveTab(0, ui->ModulesTabs->count() - 1);
	ui->MainTabs->setCurrentIndex(0);

	connect(qApp, &Application::errorOccurred, ui->Log, &SAB::LogWidget::error);
	connect(qApp, &Application::warningOccurred, ui->Log, &SAB::LogWidget::warning);

	connect(ui->LogClear, &QPushButton::clicked, ui->Log, &SAB::LogWidget::clear);

	connect(ui->Log, &SAB::LogWidget::cleared, this, [this] () {
		int index = ui->MainTabs->indexOf(ui->LogTab);
		ui->MainTabs->setTabIcon(index, QIcon());
	});

	connect(ui->Log, &SAB::LogWidget::messageAdded, this, [this] () {
		int index = ui->MainTabs->indexOf(ui->LogTab);
		ui->MainTabs->setTabIcon(index, QIcon(":/icons/warning_triangle.png"));
	});

	m_tray_icon = new QSystemTrayIcon(QIcon(":/icons/icon"), this);
	m_tray_icon->setToolTip(qApp->applicationName());

	connect(m_tray_icon, &QSystemTrayIcon::activated, this, [this] (QSystemTrayIcon::ActivationReason reason) {
		if (reason == QSystemTrayIcon::Trigger) {
			m_tray_icon->hide();
			this->show();
		}
	});

	QAction *quit_action = new QAction("Exit", m_tray_icon);
	connect(quit_action, &QAction::triggered, qApp, &QApplication::quit);

	QAction *show_action = new QAction("Show", m_tray_icon);
	connect(show_action, &QAction::triggered, this, [this] () {
		m_tray_icon->hide();
		this->show();
	});

	QMenu *tray_icon_menu = new QMenu;
	tray_icon_menu->addAction(show_action);
	tray_icon_menu->addSeparator();
	foreach (AbstractModule *module, qApp->modules()) {
		if (module->prepareTray(tray_icon_menu, m_tray_icon)) {
			tray_icon_menu->addSeparator();
		}
	}
	tray_icon_menu->addAction(quit_action);

	m_tray_icon->setContextMenu(tray_icon_menu);

	QToolButton *hide_button = new QToolButton();
	hide_button->setText("Hide");
	hide_button->setToolTip("Minimize to the system tray");
	hide_button->setStyleSheet("QToolButton { margin-bottom: 5px; }");
	//hide_button->setFlat(true);
	ui->MainTabs->setCornerWidget(hide_button, Qt::TopRightCorner);
	connect(hide_button, &QPushButton::clicked, this, [this] () {
		this->hide();
		m_tray_icon->show();
	});

	ui->AboutName->setText(qApp->applicationName());
	ui->AboutVersion->setText(qApp->applicationVersion());

	QString text("© ");
	text += QCoreApplication::organizationName() + " ";
	int year = QDate::currentDate().year();
	if (year > initial_year) {
		text += QString::number(initial_year) + " - " + QString::number(year);
	} else {
		text += QString::number(initial_year);
	}
	ui->AboutCopyright->setText(text);

	ui->AboutTab->setStyleSheet("background-color: #181233; color: #448899;");

	StateSettings().loadWindow(this);
}

SAB::MainWindow::~MainWindow()
{
	StateSettings().saveWindow(this);
	delete ui;
}