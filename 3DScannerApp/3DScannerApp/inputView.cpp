// Other Includes
#include "inputView.h"
#include "takePicView.h"
#include "intrinsicController.h"
#include "extrinsicController.h"
#include "calibrationController.h"

#include <qapplication.h>
#include <qlabel.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qgridlayout.h>
#include <qpushbutton.h>

InputView::InputView(int calibType, QWidget *parent) : QWidget(parent)
{
	message = new QLabel(); // this is just used to set error messages
	message->setVisible(false);

	calibrationType = calibType;
	constructLayout();

	// Check to see what type of input view this is:
	if (calibrationType == Enums::controllerEnum::INTRINSIC)
	{
		// connect the exit button's clicked() signal to the qapplication's quit() slot:
		// (note: qApp is a global pointer to the application object)
		connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
		connect(startButton, SIGNAL(clicked()), this, SLOT(startCalibration()));
		connect(saveBrowseButton, SIGNAL(clicked()), this, SLOT(createSaveFileDialog()));
	}
	else if(calibrationType == Enums::controllerEnum::EXTRINSIC)
	{
		connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
		connect(startButton, SIGNAL(clicked()), this, SLOT(startCalibration()));
		connect(saveBrowseButton, SIGNAL(clicked()), this, SLOT(createSaveFileDialog()));
		connect(loadBrowseButton, SIGNAL(clicked()), this, SLOT(createLoadFileDialog()));
	}
	
}

void InputView::setCalibrationController(CalibrationController& calibControl) {
	calibrationController = &calibControl;
}

void InputView::showMessage(QString msg)
{
	// todo
	message->setText(msg);
	message->setVisible(true);
}

void InputView::startCalibration()
{
	//--------------HANDLE ERRORS-----------------------
	calibrationController->setNumCorners(horizontalText->text().toInt(), verticalText->text().toInt());
	calibrationController->setSaveDirectory(saveDirText->text().toStdString());
	if (calibrationType == Enums::controllerEnum::EXTRINSIC) {
		calibrationController->setLoadDirectory(loadDirText->text().toStdString());
		calibrationController->loadXML();
	}
	calibrationController->createTakePicView();
}

void InputView::createSaveFileDialog() {
	dir = QFileDialog::getExistingDirectory(this, "Select Calibration Save Directory",
		"C:/", QFileDialog::ShowDirsOnly);
	saveDirText->setText(dir);
}

void InputView::createLoadFileDialog()
{
	dir = QFileDialog::getExistingDirectory(this, "Select Calibration Load Directory",
		"C:/", QFileDialog::ShowDirsOnly);
	loadDirText->setText(dir);
}

void InputView::constructLayout()
{
	if (calibrationType == Enums::controllerEnum::INTRINSIC)
	{
		mainLayout = new QGridLayout();
		horizontalLabel = new QLabel("Horizontal Squares:");
		verticalLabel = new QLabel("Vertical Squares:");
		messagesLabel = new QLabel("Messages:" );
		saveLabel = new QLabel("Save Directory:");
		startButton = new QPushButton("Start");
		exitButton = new QPushButton("Exit");
		saveBrowseButton = new QPushButton("Browse...");
		horizontalText = new QLineEdit();
		horizontalText->setPlaceholderText("Number of Horizontal Squares");
		verticalText = new QLineEdit();
		verticalText->setPlaceholderText("Number of Vertical Squares");
		saveDirText = new QLineEdit();
		saveDirText->setPlaceholderText("Directory to save intrinsic calibration data");

		// Add widgets to layout
		mainLayout->addWidget(horizontalLabel, 0, 0);
		mainLayout->addWidget(horizontalText, 0, 1);
		mainLayout->addWidget(verticalLabel, 1, 0);
		mainLayout->addWidget(verticalText, 1, 1);
		mainLayout->addWidget(saveLabel, 2, 0);
		mainLayout->addWidget(saveDirText, 2, 1);
		mainLayout->addWidget(saveBrowseButton, 2, 2);
		mainLayout->addWidget(startButton, 4, 1);
		mainLayout->addWidget(exitButton, 4, 2);
		mainLayout->addWidget(messagesLabel, 5, 0);
		setLayout(mainLayout);
	}
	else if (calibrationType == Enums::controllerEnum::EXTRINSIC)
	{
		// create widgets
		mainLayout = new QGridLayout();
		horizontalLabel = new QLabel("Horizontal Squares:");
		verticalLabel = new QLabel("Vertical Squares:");
		messagesLabel = new QLabel("Messages:" ); 
		loadLabel = new QLabel("Load Directory:");
		saveLabel = new QLabel("Save Directory:");
		message = new QLabel(); 
		message->setVisible(false);
		startButton = new QPushButton("Start");
		exitButton = new QPushButton("Exit");
		loadBrowseButton = new QPushButton("Browse...");
		saveBrowseButton = new QPushButton("Browse...");
		horizontalText = new QLineEdit();
		horizontalText->setPlaceholderText("Number of Horizontal Squares");
		verticalText = new QLineEdit();
		verticalText->setPlaceholderText("Number of Vertical Squares");
		saveDirText = new QLineEdit();
		saveDirText->setPlaceholderText("Directory to save extrinsic calibration data");
		loadDirText = new QLineEdit();
		loadDirText->setPlaceholderText("Directory to load intrinsic calibration data");

		// Add widgets to layout
		mainLayout->addWidget(horizontalLabel, 0, 0);
		mainLayout->addWidget(horizontalText, 0, 1);
		mainLayout->addWidget(verticalLabel, 1, 0);
		mainLayout->addWidget(verticalText, 1, 1);
		mainLayout->addWidget(loadLabel, 2, 0);
		mainLayout->addWidget(loadDirText, 2, 1);
		mainLayout->addWidget(loadBrowseButton, 2, 2);
		mainLayout->addWidget(saveLabel, 3, 0);
		mainLayout->addWidget(saveDirText, 3, 1);
		mainLayout->addWidget(saveBrowseButton, 3, 2);
		mainLayout->addWidget(startButton, 5, 1);
		mainLayout->addWidget(exitButton, 5, 2);
		mainLayout->addWidget(messagesLabel, 6, 0);
		setLayout(mainLayout);
	}
}