// Qt Includes
#include <qwidget.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgridlayout.h>
#include <qlineedit.h>
// Other Includes
#include <string>
#include "inputView.h"
#include "enums.h"
#include <qapplication.h>
#include <iostream>

InputView::InputView(int calibType, QWidget *parent) : QWidget(parent)
{
	message = new QLabel(); // this is just used to set error messages
	message->setVisible(false);

	// Check to see what type of input view this is:
	if (calibType == Enums::controllerEnum::INTRINSIC)
	{
		calibrationType = calibType;
		// create intrinsic buttons, etc for intrinsic view
		QGridLayout * mainLayout = new QGridLayout();
		QLabel * horizontalLabel = new QLabel("Horizontal Squares:");
		QLabel * verticalLabel = new QLabel("Vertical Squares:");
		QLabel * messagesLabel = new QLabel("Messages:" );
		QPushButton * startButton = new QPushButton("Start");
		QPushButton * exitButton = new QPushButton("Exit");

		// connect the exit button's clicked() signal to the qapplication's quit() slot:
		// (note: qApp is a global pointer to the application object)
		connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
		connect(startButton, SIGNAL(clicked()), this, SLOT(createTakePicView()));

		QLineEdit * horizontalText = new QLineEdit();
		horizontalText->setPlaceholderText("Number of Horizontal Squares");
		QLineEdit * verticalText = new QLineEdit();
		verticalText->setPlaceholderText("Number of Vertical Squares");
		mainLayout->addWidget(horizontalLabel, 1, 0);
		mainLayout->addWidget(horizontalText, 1, 1);
		mainLayout->addWidget(verticalLabel, 3, 0);
		mainLayout->addWidget(verticalText, 3, 1);
		mainLayout->addWidget(startButton, 5, 1);
		mainLayout->addWidget(exitButton, 5, 2);
		mainLayout->addWidget(messagesLabel, 6, 0);
		setLayout(mainLayout);
				
	}
	else if(calibType == Enums::controllerEnum::EXTRINSIC)
	{
		calibrationType = calibType;
		// create extrinsic buttons, etc for extrinsic view
		QGridLayout * mainLayout = new QGridLayout();
		QLabel * horizontalLabel = new QLabel("Horizontal Squares:");
		QLabel * verticalLabel = new QLabel("Vertical Squares:");
		QLabel * messagesLabel = new QLabel("Messages:" ); 
		QLabel * loadLabel = new QLabel("Load Directory:");
		message = new QLabel(); // this is just used to set error messages
		message->setVisible(false);
		QPushButton * startButton = new QPushButton("Start");
		QPushButton * exitButton = new QPushButton("Exit");
		QPushButton * browseButton = new QPushButton("Browse...");
		QLineEdit * horizontalText = new QLineEdit();
		horizontalText->setPlaceholderText("Number of Horizontal Squares");
		QLineEdit * verticalText = new QLineEdit();
		verticalText->setPlaceholderText("Number of Vertical Squares");
		loadDirText = new QLineEdit();

		connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
		connect(startButton, SIGNAL(clicked()), this, SLOT(createTakePicView()));
		connect(browseButton, SIGNAL(clicked()), this, SLOT(createFileDialog()));

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

void InputView::showMessage(QString msg)
{
	// todo
	message->setText(msg);
	message->setVisible(true);
}

void InputView::createTakePicView()
{
	using std::cout;
	using std:: endl;
	cout << "TEST" << endl;
	calibPicView = new TakePicView(calibrationType);
	calibPicView->setModal(true);
	calibPicView->show();
}

void InputView::createFileDialog()
{
	dir = QFileDialog::getExistingDirectory(this, "Select Calibration Load Directory",
		"C:/", QFileDialog::ShowDirsOnly);
	loadDirText->setText(dir);
}

/*
void InputView::setCalibrationController(CalibrationController& calibControl)
{
	// todo
}
*/