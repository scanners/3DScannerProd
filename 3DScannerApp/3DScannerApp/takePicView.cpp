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
		takePicButton = new QPushButton("Take Picture");
		cancelButton = new QPushButton("Cancel");

		connect(takePicButton, SIGNAL(clicked()), this, SLOT(takePicture()));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(stopVideo()));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

		// layout code
		mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
		mainLayout->addWidget(titleLabel);
		mainLayout->addWidget(videoLabel);
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
		takePicButton = new QPushButton("Take Picture");
		cancelButton = new QPushButton("Cancel");

		connect(takePicButton, SIGNAL(clicked()), this, SLOT(takePicture()));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(stopVideo()));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

		// layout code
		mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
		mainLayout->addWidget(titleLabel);
		mainLayout->addWidget(videoLabel);
		buttonLayout = new QGridLayout();
		buttonLayout->addWidget(takePicButton, 0, 0);
		buttonLayout->addWidget(cancelButton, 1, 0);
		mainLayout->addLayout(buttonLayout);
		setLayout(mainLayout);
	}
	capture.open(0);
	if (capture.isOpened())
	{
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(displayVideoFrame()));
		timer->start(20);
	}
	else
	{
		videoLabel->setPixmap(QPixmap("noCamera.png"));
	}
}

void TakePicView::displayVideoFrame()
{
	capture.read(image);
	cvtColor(image, image, CV_BGR2RGB);
	QImage qimg((uchar*)image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
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
}

void TakePicView::takePicture()
{
	calibrationController->findCorners(image);
	// **** will need to check if the picture was taken successfully **** \\ 
	
}

void TakePicView::setCalibrationController(CalibrationController& calibControl){
	calibrationController = &calibControl;
}

void TakePicView::incrementSuccesses(int successes, int requiredNumSuccesses) {
	// todo
}

void TakePicView::showMessage(int messageEnum) {

}
