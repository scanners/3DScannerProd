#include "scanInputView.h"
#include "scanController.h"
#include <qapplication.h>
#include <qlabel.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qgridlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>

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
	if (loadDirText->text().toStdString() == "")
	{
		QMessageBox error(QMessageBox::Icon::Critical,
				"No load directory was given!",
				"", 
				QMessageBox::StandardButton::Ok);
		error.setText("Please select the directory containing the calibration XML files.");
		error.setInformativeText("Check the directory and try again.");
		error.setModal(false);
		error.exec();
	}
	else if (saveDirText->text().toStdString() == "")
	{
		QMessageBox error(QMessageBox::Icon::Critical,
				"No output directory was given!",
				"", 
				QMessageBox::StandardButton::Ok);
		error.setText("Please select an output directory for the scan.");
		error.setInformativeText("Check the directory and try again.");
		error.setModal(false);
		error.exec();
	}
	else if (loadXMLSuccess) {
		scanController->resetRegions();
		scanController->createOverlayView();
	} else {
		//Dialog box for errors
		QMessageBox error(QMessageBox::Icon::Critical,
				"Error loading XML file",
				"", 
				QMessageBox::StandardButton::Ok);
		error.setText("The XML file could not be successfully loaded.");
		error.setInformativeText("Please check the file and try again.");
		error.setModal(false);
		error.exec();
	}
}

void ScanInputView::createSaveFileDialog() {
	QString dir = QFileDialog::getExistingDirectory(this, "Scan output directory",
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

ScanInputView::~ScanInputView()
{
	delete saveDirText;
	delete loadDirText;
	delete saveLabel;
	delete loadLabel;
	delete startButton;
	delete exitButton;
	delete saveBrowseButton;
	delete loadBrowseButton;
	delete mainLayout;
}

