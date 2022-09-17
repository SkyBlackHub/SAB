#ifndef SAB_SNAPSHOTSFORM_H
#define SAB_SNAPSHOTSFORM_H

#include <QWidget>

namespace SAB {

	namespace Ui {
		class SnapshotsForm;
	}

	class SnapshotsModule;

	class SnapshotsForm : public QWidget
	{
		Q_OBJECT

	public:
		explicit SnapshotsForm(QWidget *parent = nullptr);
		~SnapshotsForm();

		void configure(SnapshotsModule *module);

	private:
		Ui::SnapshotsForm *ui;
	};


} // namespace SAB
#endif // SAB_SNAPSHOTSFORM_H
