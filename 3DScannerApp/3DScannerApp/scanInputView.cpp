#include "scanInputView.h"
#include "scanController.h"
#include <qapplication.h>
#include <qlabel.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qgridlayout.h>
#include <qpushbutton.h>


ScanInputView::ScanInputView(QWidget *parent) : QWidget(parent)
{
	constructLayout();

	connect(startButton, SIGNAL(clicked()), this, SLOT(startScan()));
	connect(saveBrowseButton, SIGNAL(clicked()), this, SLOT(createSaveFileDialog()));
	connect(loadBrowseButton, SIGNAL(clicked()), this, SLOT(createLoadFileDialog()));
	connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
}

void ScanInputView::setScanController(ScanController& scanControl) {
	scanController = &scanControl;
}

void ScanInputView::startScan()
{
	scanController->setSaveDirectory(saveDirText->text().toStdString());
	scanController->setLoadDirectory(loadDirText->text().toStdString());
	bool loadXMLSuccess = scanController->loadXML();
	if (loadXMLSuccess) {
		scanController->createScanningView();
	} else {
		//Dialog box for errors
	}
}

void ScanInputView::createSaveFileDialog() {
	QString dir = QFileDialog::getExistingDirectory(this, "Select Calibration Save Directory",
		"C:/", QFileDialog::ShowDirsOnly);
	saveDirText->setText(dir);
}

void ScanInputView::createLoadFileDialog()
{
	QString dir = QFileDialog::getExistingDirectory(this, "Select Calibration Load Directory",
		"C:/", QFileDialog::ShowDirsOnly);
	loadDirText->setText(dir);
}

void ScanInputView::constructLayout()
{
	loadLabel = new QLabel("Load Directory:");
	saveLabel = new QLabel("Save Directory:");
	loadBrowseButton = new QPushButton("Browse...");
	saveBrowseButton = new QPushButton("Browse...");
	saveDirText = new QLineEdit();
	saveDirText->setPlaceholderText("Directory to save scan");
	loadDirText = new QLineEdit();
	loadDirText->setPlaceholderText("Directory to load calibration data");
	startButton = new QPushButton("Start Scan");
	exitButton = new QPushButton("Exit");
	
	mainLayout = new QGridLayout();
	mainLayout->addWidget(loadLabel, 0, 0);
	mainLayout->addWidget(loadDirText, 0, 1);
	mainLayout->addWidget(loadBrowseButton, 0, 2);
	mainLayout->addWidget(saveLabel, 1, 0);
	mainLayout->addWidget(saveDirText, 1, 1);
	mainLayout->addWidget(saveBrowseButton, 1, 2);
	mainLayout->addWidget(startButton, 3, 1);
	mainLayout->addWidget(exitButton, 3, 2);
	setLayout(mainLayout);
}



