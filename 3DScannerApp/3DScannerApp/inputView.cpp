// Other Includes
#include "inputView.h"
#include "takePicView.h"
#include "intrinsicController.h"
#include "extrinsicController.h"
#include "calibrationController.h"
#include "enums.h"
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

	// Check to see what type of input view this is:
	if (calibType == Enums::controllerEnum::INTRINSIC)
	{
		calibrationType = calibType;
		// Constructs the layout based on the calibration type
		constructLayout(calibType);

		// connect the exit button's clicked() signal to the qapplication's quit() slot:
		// (note: qApp is a global pointer to the application object)
		connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
		connect(startButton, SIGNAL(clicked()), this, SLOT(createTakePicView()));

	}
	else if(calibType == Enums::controllerEnum::EXTRINSIC)
	{
		calibrationType = calibType;
		// Constructs the layout based on the calibration type
		constructLayout(calibType);

		connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
		connect(startButton, SIGNAL(clicked()), this, SLOT(createTakePicView()));
		connect(browseButton, SIGNAL(clicked()), this, SLOT(createFileDialog()));

		
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

void InputView::createTakePicView()
{
	//--------------HANDLE ERRORS-----------------------
	calibrationController->setNumCorners(horizontalText->text().toInt(), verticalText->text().toInt());
	calibrationController->createTakePicView();
}

void InputView::createFileDialog()
{
	dir = QFileDialog::getExistingDirectory(this, "Select Calibration Load Directory",
		"C:/", QFileDialog::ShowDirsOnly);
	loadDirText->setText(dir);
}

void InputView::constructLayout(int calibType)
{
	if (calibType == Enums::controllerEnum::INTRINSIC)
	{
		mainLayout = new QGridLayout();
		horizontalLabel = new QLabel("Horizontal Squares:");
		verticalLabel = new QLabel("Vertical Squares:");
		messagesLabel = new QLabel("Messages:" );
		startButton = new QPushButton("Start");
		exitButton = new QPushButton("Exit");
		horizontalText = new QLineEdit();
		horizontalText->setPlaceholderText("Number of Horizontal Squares");
		verticalText = new QLineEdit();
		verticalText->setPlaceholderText("Number of Vertical Squares");

		// Add widgets to layout
		mainLayout->addWidget(horizontalLabel, 1, 0);
		mainLayout->addWidget(horizontalText, 1, 1);
		mainLayout->addWidget(verticalLabel, 3, 0);
		mainLayout->addWidget(verticalText, 3, 1);
		mainLayout->addWidget(startButton, 5, 1);
		mainLayout->addWidget(exitButton, 5, 2);
		mainLayout->addWidget(messagesLabel, 6, 0);
		setLayout(mainLayout);
	}
	else if (calibType == Enums::controllerEnum::EXTRINSIC)
	{
		// create widgets
		mainLayout = new QGridLayout();
		horizontalLabel = new QLabel("Horizontal Squares:");
		verticalLabel = new QLabel("Vertical Squares:");
		messagesLabel = new QLabel("Messages:" ); 
		loadLabel = new QLabel("Load Directory:");
		message = new QLabel(); 
		message->setVisible(false);
		startButton = new QPushButton("Start");
		exitButton = new QPushButton("Exit");
		browseButton = new QPushButton("Browse...");
		horizontalText = new QLineEdit();
		horizontalText->setPlaceholderText("Number of Horizontal Squares");
		verticalText = new QLineEdit();
		verticalText->setPlaceholderText("Number of Vertical Squares");
		loadDirText = new QLineEdit();

		// Add widgets to layout
		mainLayout->addWidget(horizontalLabel, 0, 0);
		mainLayout->addWidget(horizontalText, 0, 1);
		mainLayout->addWidget(verticalLabel, 1, 0);
		mainLayout->addWidget(loadLabel, 2, 0);
		mainLayout->addWidget(verticalText, 1, 1);
		mainLayout->addWidget(loadDirText, 2, 1);
		mainLayout->addWidget(browseButton, 2, 2);
		mainLayout->addWidget(startButton, 4, 1);
		mainLayout->addWidget(exitButton, 4, 2);
		mainLayout->addWidget(messagesLabel, 5, 0);
		setLayout(mainLayout);
	}
}