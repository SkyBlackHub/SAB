#include "SnapshotsForm.h"
#include "ui_SnapshotsForm.h"

#include <QFileDialog>
#include <QMessageBox>

#include "modules/SnapshotsModule.h"

#include "Duplex.h"

SAB::SnapshotsForm::SnapshotsForm(QWidget *parent) : QWidget(parent),	ui(new Ui::SnapshotsForm)
{
	ui->setupUi(this);

	QLabel *label = new QLabel("Filters: ", this);
	label->setContentsMargins(QMargins(0, 5, 5, 5));
	ui->FiltersTabs->setCornerWidget(label, Qt::TopLeftCorner);
}

SAB::SnapshotsForm::~SnapshotsForm()
{
	delete ui;
}

void SAB::SnapshotsForm::configure(SnapshotsModule *module)
{
	duplex(ui->Path, module, &SnapshotsModule::pathChanged, &SnapshotsModule::setPath, module->path());
	duplex(ui->Pattern, module, &SnapshotsModule::patternChanged, &SnapshotsModule::setPattern, module->pattern());

	duplex(ui->UseTrigger, module, &SnapshotsModule::useTriggerChanged, &SnapshotsModule::setUseTrigger, module->useTrigger());
	duplex(ui->CropWindow, module, &SnapshotsModule::cropWindowChanged, &SnapshotsModule::setCropWindow, module->cropWindow());

	duplex(ui->Start, ui->Stop, module, &SnapshotsModule::monitorClipboardChanged, &SnapshotsModule::setMonitorClipboard, module->monitorClipboard());

	duplex(ui->MaximumSavers, module, &SnapshotsModule::maximumSaversChanged, &SnapshotsModule::setMaximumSavers, module->maximumSavers());

	duplex(ui->MaximumQueue, module, &SnapshotsModule::maximumQueueChanged, &SnapshotsModule::setMaximumQueue, module->maximumQueue());

	duplex(ui->Compression, module, &SnapshotsModule::compressionRateChanged, &SnapshotsModule::setCompressionRate, module->compressionRate());
	duplex(ui->Quality, module, &SnapshotsModule::qualityChanged, &SnapshotsModule::setQuality, module->quality());

	duplex(ui->Exclude, module, &SnapshotsModule::excludeChanged, &SnapshotsModule::setExclude, module->exclude());
	duplex(ui->Include, module, &SnapshotsModule::includeChanged, &SnapshotsModule::setInclude, module->include());

	connect(ui->FormatPNG, &QRadioButton::toggled, ui->Compression, &QSpinBox::setEnabled);
	connect(ui->FormatJPEG, &QRadioButton::toggled, ui->Quality, &QSpinBox::setEnabled);
	// emulate switching to invoke the connections
	ui->FormatJPEG->setChecked(true);
	ui->FormatPNG->setChecked(true);

	QHash<SnapshotsModule::ImageFormat, QRadioButton *> formats = {
		{SnapshotsModule::ImageFormat::BMP, ui->FormatBMP},
		{SnapshotsModule::ImageFormat::PNG, ui->FormatPNG},
		{SnapshotsModule::ImageFormat::JPEG, ui->FormatJPEG},
	};

	duplex(formats, module, &SnapshotsModule::formatChanged, &SnapshotsModule::setFormat, module->format());

	connect(ui->PathBrowse, &QPushButton::clicked, this, [this] () {
		QFileDialog dialog(this);
		dialog.setFileMode(QFileDialog::Directory);
		dialog.setOption(QFileDialog::ShowDirsOnly, true);
		if (dialog.exec()) {
			ui->Path->setText(dialog.selectedFiles().constFirst());
		}
	});

	connect(ui->Snap, &QPushButton::clicked, module, &SnapshotsModule::snap);

	connect(ui->FiltersTest, &QPushButton::clicked, this, [this, module] () {
		QString subject = ui->FiltersSubject->text().trimmed();
		if (subject.isEmpty()) {
			return;
		}
		QString filter;
		bool result = module->testFilters(QFileInfo(subject), &filter);
		QMessageBox box;
		box.setWindowTitle("Filters test result");
		box.setStandardButtons(QMessageBox::Ok);

		if (filter.isNull()) {
			box.setText(result ? "Passed" : (module->hasIncludeFilters() ? "Rejected by lack of inclusion" : "Rejected"));
		} else {
			box.setText(result ? "Passed by the inclusion filter" : "Rejected by the exclusion filter");
			box.setInformativeText(filter);
		}

		box.exec();
	});
}