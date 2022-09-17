#ifndef SAB_LOGWIDGET_H
#define SAB_LOGWIDGET_H

#include <QWidget>
#include <QTableWidget>

namespace SAB {

	class LogWidget : public QWidget
	{
		Q_OBJECT
	public:

		enum MessageType {
			MESSAGE_INFO,
			MESSAGE_NOTICE,
			MESSAGE_WARNING,
			MESSAGE_ERROR,
			MESSAGE_CRITICAL,
		};

		explicit LogWidget(QWidget *parent = nullptr);

	signals:
		void messageAdded(const QString &message, SAB::LogWidget::MessageType type);
		void cleared();

	public slots:
		void add(const QString &message, SAB::LogWidget::MessageType type);
		void clear();

		void info(const QString &message);
		void notice(const QString &message);
		void warning(const QString &message);
		void error(const QString &message);
		void critical(const QString &message);

	private slots:
		void actionTriggered();

	private:
		QTableWidget *m_table;

		QHash<MessageType, QPair<QFont, QBrush> > m_message_styles;
	};

} // namespace SAB

#endif // SAB_LOGWIDGET_H
