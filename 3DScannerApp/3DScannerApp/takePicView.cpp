#include "takePicView.h"


#include "qpushbutton.h"
#include "qboxlayout.h"
#include "qgridlayout.h"
#include <qwidget.h>
#include <qlabel.h>
#include <qtimer.h>

TakePicView::TakePicView(int calibType, QWidget *parent) : QDialog(parent)
{
	
	if (calibType == Enums::controllerEnum::INTRINSIC)
	{
		// create intrinsic calib stuff
		videoLabel = new QLabel(this);
		videoLabel->setMinimumWidth(640);
		videoLabel->setMinimumHeight(480);
		titleLabel = new QLabel("Intrinsic Calibration");
		picProgressLabel = new QLabel("Picture 1 of 20");
		messageLabel = new QLabel("Messages: ");
		messages = new QLabel("<b>Messages Go Here</b>");
		messages->setStyleSheet("color: red; font-weight: bold;");
		takePicButton = new QPushButton("Take Picture");
		takePicButton->setEnabled(false);
		cancelButton = new QPushButton("Cancel");

		connect(takePicButton, SIGNAL(clicked()), this, SLOT(takePicture()));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(stopVideo()));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

		// layout code
		mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
		mainLayout->addWidget(titleLabel);
		mainLayout->addWidget(videoLabel);
		mainLayout->addWidget(picProgressLabel);
		mainLayout->addWidget(messageLabel);
		mainLayout->addWidget(messages);
		buttonLayout = new QGridLayout();
		buttonLayout->addWidget(takePicButton, 0, 0);
		buttonLayout->addWidget(cancelButton, 1, 0);
		mainLayout->addLayout(buttonLayout);
		setLayout(mainLayout);
	}
	else if (calibType == Enums::controllerEnum::EXTRINSIC)
	{
		// create extrin calib stuff
		videoLabel = new QLabel(this);
		videoLabel->setMinimumWidth(640);
		videoLabel->setMinimumHeight(480);
		titleLabel = new QLabel("Extrinsic Calibration");
		picProgressLabel = new QLabel("Picture 1 of 4");
		messageLabel = new QLabel("Messages: ");
		messages = new QLabel("<b>Messages Go Here</b>");
		messages->setStyleSheet("color: red; font-weight: bold;");
		takePicButton = new QPushButton("Take Picture");
		cancelButton = new QPushButton("Cancel");

		connect(takePicButton, SIGNAL(clicked()), this, SLOT(takePicture()));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(stopVideo()));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

		// layout code
		mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
		mainLayout->addWidget(titleLabel);
		mainLayout->addWidget(videoLabel);
		mainLayout->addWidget(picProgressLabel);
		mainLayout->addWidget(messageLabel);
		mainLayout->addWidget(messages);
		buttonLayout = new QGridLayout();
		buttonLayout->addWidget(takePicButton, 0, 0);
		buttonLayout->addWidget(cancelButton, 1, 0);
		mainLayout->addLayout(buttonLayout);
		setLayout(mainLayout);
	}
	capture.open(0);
	if (capture.isOpened())
	{
		// if the Video Capture Stream is open, set button and create timer
		takePicButton->setEnabled(true);
		timer = new QTimer(this);
		// slot for displaying video every 20ms
		connect(timer, SIGNAL(timeout()), this, SLOT(displayVideoFrame()));
		timer->start(20);
	}
	else
	{
		videoLabel->setPixmap(QPixmap("noCamera.png"));
		takePicButton->setEnabled(false);
		messages->setStyleSheet("color: red; font-weight: bold;");
		messages->setText("No camera is detected! Please check your connection!");
	}
}

void TakePicView::displayVideoFrame()
{
	Mat displayImage;
	capture.read(image);
	cvtColor(image, displayImage, CV_BGR2RGB);
	QImage qimg((uchar*)displayImage.data, displayImage.cols, displayImage.rows, displayImage.step, QImage::Format_RGB888);
	videoLabel->setPixmap(QPixmap::fromImage(qimg));
}


// releases the video input and stops the timer.
// we might need to do some extra clean up here.
// doesn't work when the window is closed with the "X" button.
void TakePicView::stopVideo()
{
	if (capture.isOpened())
	{
		capture.read(image);
		capture.release();
		timer->stop();
	}
}

// this is for the "X" Button event
void TakePicView::closeEvent(QCloseEvent * event)
{
	this->stopVideo();
	//***probably should free up memory here
}

void TakePicView::takePicture()
{
	calibrationController->findCorners(image);
}

void TakePicView::setCalibrationController(CalibrationController& calibControl){
	calibrationController = &calibControl;
}

void TakePicView::incrementSuccesses(int successes, int requiredNumSuccesses) {
	std::stringstream labelString;
	labelString << "Picture " << successes << " of " << requiredNumSuccesses;
	QString str = labelString.str().c_str(); // create a const string, required by setText
	picProgressLabel->setText(str);
}

void TakePicView::showMessage(int messageEnum) {
	/* We currently do not have any errors. What ints should we check? */
	if (messageEnum == Enums::calibrationEnum::CORNERS_FAILURE)
	{
		// display message indicating error
		messages->setText("Error finding corners! Please check you checkerboard.");
	}
	else if (messageEnum == Enums::calibrationEnum::CORNERS_SUCCESS)
	{
		messages->setStyleSheet("color: #000; font-weight: bold;");
		messages->setText("Found corners!");
	}
	else if (messageEnum == Enums::calibrationEnum::CALIBRATION_SUCCESS)
	{
		messages->setStyleSheet("color: green; font-weight: bold;");
		messages->setText("Calibration was successful!");
	}
	
}
