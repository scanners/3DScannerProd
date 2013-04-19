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
#include <cctype>

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

void ScanInputView::connectGlobalMessage(QLabel * text)
{
	globalMessage = text;
}
void ScanInputView::showMessage(QString msg)
{
	globalMessage->setText(msg);
	globalMessage->setVisible(true);
}

void ScanInputView::startScan()
{
	scanController->setSaveDirectory(saveDirText->text().toStdString());
	scanController->setLoadDirectory(loadDirText->text().toStdString());
	bool loadXMLSuccess = scanController->loadXML();
	if (loadDirText->text().toStdString() == "")
	{
		this->showMessage("No load directory was given!");
	}
	else if (saveDirText->text().toStdString() == "")
	{
		this->showMessage("Please specify an output directory for the scan.");
	}
	else if (saveFileNameText->text().toStdString() == "")
	{
		this->showMessage("No output filename was given.");
	}
	else if (!checkFileName(saveFileNameText->text().toStdString()))
	{
		this->showMessage("Only alphanumeric characters (a-Z, 0-9) are allowed in the filename.");
	}
	else if (loadXMLSuccess) {
		scanController->setSaveFileName(saveFileNameText->text().toStdString());
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

/*
Returns true if filename is okay. False otherwise.
*/
bool ScanInputView::checkFileName(string filename)
{
	for (int i = 0; i < filename.length(); i++)
	{
		if (!isalnum(filename[i]))
		{
			return false;
		}
	}
	return true;
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
	saveDirText->setReadOnly(true);
	saveDirText->setPlaceholderText("Directory to save scan");
	loadDirText = new QLineEdit();
	loadDirText->setReadOnly(true);
	loadDirText->setPlaceholderText("Directory to load calibration data");
	saveFileNameLabel = new QLabel("Output File Name:");
	saveFileNameText = new QLineEdit();
	startButton = new QPushButton("Start Scan");
	exitButton = new QPushButton("Exit");
	
	mainLayout = new QGridLayout();
	mainLayout->addWidget(loadLabel, 0, 0);
	mainLayout->addWidget(loadDirText, 0, 1);
	mainLayout->addWidget(loadBrowseButton, 0, 2);
	mainLayout->addWidget(saveLabel, 1, 0);
	mainLayout->addWidget(saveDirText, 1, 1);
	mainLayout->addWidget(saveBrowseButton, 1, 2);
	mainLayout->addWidget(saveFileNameLabel, 2, 0);
	mainLayout->addWidget(saveFileNameText, 2, 1);
	mainLayout->addWidget(startButton, 3, 1);
	mainLayout->addWidget(exitButton, 3, 2);

	this->setStyleSheet(
			"QLabel {"
			"font-weight: bold; }"
			);
	
	setLayout(mainLayout);
	setDefaultStyles();
}

void ScanInputView::setDefaultStyles()
{
	this->saveDirText->setStyleSheet("border: 1px solid gray; padding: 5px;");
	this->loadDirText->setStyleSheet("border: 1px solid gray; padding: 5px;");
	this->saveFileNameText->setStyleSheet("border: 1px solid gray; padding: 5px;");
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

