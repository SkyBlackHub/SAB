#include "LogWidget.h"

#include <QBoxLayout>
#include <QTime>
#include <QHeaderView>
#include <QScrollBar>
#include <QAction>
#include <QClipboard>
#include <QApplication>

enum {
	ACTION_TAG_COPY,
	ACTION_TAG_COPY_ROW,
	ACTION_TAG_REMOVE_ROW,
	ACTION_TAG_CLEAR
};

SAB::LogWidget::LogWidget(QWidget *parent) : QWidget(parent)
{
	this->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
	this->layout()->setAlignment(Qt::AlignTop);
	this->layout()->setContentsMargins(0, 0, 0, 0);

	QTableWidget *table = new QTableWidget();

	table->setShowGrid(false);
	table->setCornerButtonEnabled(false);
	table->setWordWrap(true);
	table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);

	table->verticalHeader()->setDefaultSectionSize(20);
	table->verticalHeader()->setVisible(false);

	table->setColumnCount(2);

	table->horizontalHeader()->setVisible(false);
	table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

	this->layout()->addWidget(table);
	m_table = table;

	QFont font = this->font();

	m_message_styles[MESSAGE_INFO]     = {font, QBrush(Qt::white)};
	m_message_styles[MESSAGE_NOTICE]   = {font, QBrush(QColor(230, 250, 255))};
	m_message_styles[MESSAGE_WARNING]  = {font, QBrush(QColor(255, 240, 240))};
	m_message_styles[MESSAGE_ERROR]    = {font, QBrush(QColor(250, 220, 220))};

	font.setBold(true);
	m_message_styles[MESSAGE_CRITICAL] = {font, QBrush(QColor(255, 210, 210))};


	auto createAction = [this](const QString &caption, const QVariant &data) {
		QAction *action = new QAction(this);
		action->setText(caption);
		action->setData(data);
		connect(action, SIGNAL(triggered()), this, SLOT(actionTriggered()));
		this->addAction(action);
		return action;
	};

	createAction("Copy", ACTION_TAG_COPY);
	createAction("Copy Row", ACTION_TAG_COPY_ROW);
	createAction("Remove Row", ACTION_TAG_REMOVE_ROW);
	createAction("Clear", ACTION_TAG_CLEAR);

	this->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void SAB::LogWidget::add(const QString &message, SAB::LogWidget::MessageType type)
{
	QTableWidget *table = m_table;

	table->setUpdatesEnabled(false);

	int row = table->rowCount();
	table->insertRow(row);

	const QPair<QFont, QBrush> &style = m_message_styles[type];

	for (int i = 0; i < table->columnCount(); ++i) {
		QTableWidgetItem *item = new QTableWidgetItem();

		item->setBackground(style.second);
		//item->setTextAlignment(Qt::AlignTop);

		switch (i) {
			case 0:
				item->setText(QTime::currentTime().toString("hh:mm:ss.zzz"));
				break;

			case 1:
				item->setFont(style.first);
				item->setText(message);
				break;
		}

		table->setItem(row, i, item);
	}

	table->resizeRowToContents(row);
	table->scrollToBottom();

	table->setUpdatesEnabled(true);

	emit messageAdded(message, type);
}

void SAB::LogWidget::clear()
{
	m_table->setRowCount(0);
	emit cleared();
}

void SAB::LogWidget::info(const QString &message)
{
	this->add(message, MESSAGE_INFO);
}

void SAB::LogWidget::notice(const QString &message)
{
	this->add(message, MESSAGE_NOTICE);
}

void SAB::LogWidget::warning(const QString &message)
{
	this->add(message, MESSAGE_WARNING);
}

void SAB::LogWidget::error(const QString &message)
{
	this->add(message, MESSAGE_ERROR);
}

void SAB::LogWidget::critical(const QString &message)
{
	this->add(message, MESSAGE_CRITICAL);
}

void SAB::LogWidget::actionTriggered()
{
	switch (static_cast<QAction *>(sender())->data().toInt()) {
		case ACTION_TAG_COPY:
			if (m_table->currentRow() >= 0) {
				QApplication::clipboard()->setText(m_table->item(m_table->currentRow(), m_table->currentColumn())->text());
			}
			break;

		case ACTION_TAG_COPY_ROW:
			if (m_table->currentRow() >= 0) {
				QStringList text;
				for (int i = 0; i < m_table->columnCount(); ++i) {
					text.append(m_table->item(m_table->currentRow(), i)->text());
				}
				QApplication::clipboard()->setText(text.join("\t"));
			}
			break;

		case ACTION_TAG_REMOVE_ROW:
			if (m_table->currentRow() >= 0) {
				m_table->removeRow(m_table->currentRow());
				if (m_table->rowCount() == 0) {
					emit cleared();
				}
			}
			break;

		case ACTION_TAG_CLEAR:
			this->clear();
			break;
	}
}