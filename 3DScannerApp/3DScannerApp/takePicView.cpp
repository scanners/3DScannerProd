#include "takePicView.h"

#include "enums.h"
#include "qpushbutton.h"
#include "qboxlayout.h"
#include "qgridlayout.h"

TakePicView::TakePicView(int calibType, QWidget *parent) : QDialog(parent)
{
	if (calibType == Enums::controllerEnum::INTRINSIC)
	{
		// create intrinsic calib stuff
		videoLabel = new QLabel(this);
		videoLabel->setFixedSize(300,300);
		QLabel * titleLabel = new QLabel("Intrinsic Calibration");
		QLabel * picProgressLabel = new QLabel("Picture 1 of 20");
		QLabel * messageLabel = new QLabel("Messages: ");
		QPushButton * takePicButton = new QPushButton("Take Picture");
		QPushButton * cancelButton = new QPushButton("Cancel");

		connect(takePicButton, SIGNAL(clicked()), this, SLOT(takePicture));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(stopVideo()));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

		// layout code
		QBoxLayout * mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
		mainLayout->addWidget(titleLabel);
		mainLayout->addWidget(videoLabel);
		QGridLayout * buttonLayout = new QGridLayout();
		buttonLayout->addWidget(takePicButton, 0, 0);
		buttonLayout->addWidget(cancelButton, 1, 0);
		mainLayout->addLayout(buttonLayout);
		setLayout(mainLayout);
	}
	else if (calibType == Enums::controllerEnum::EXTRINSIC)
	{
		// create extrin calib stuff
		videoLabel = new QLabel(this);
		videoLabel->setFixedSize(300,300);
		QLabel * titleLabel = new QLabel("Extrinsic Calibration");
		QLabel * picProgressLabel = new QLabel("Picture 1 of 4");
		QLabel * messageLabel = new QLabel("Messages: ");
		QPushButton * takePicButton = new QPushButton("Take Picture");
		QPushButton * cancelButton = new QPushButton("Cancel");

		connect(takePicButton, SIGNAL(clicked()), this, SLOT(takePicture));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(stopVideo()));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

		// layout code
		QBoxLayout * mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
		mainLayout->addWidget(titleLabel);
		mainLayout->addWidget(videoLabel);
		QGridLayout * buttonLayout = new QGridLayout();
		buttonLayout->addWidget(takePicButton, 0, 0);
		buttonLayout->addWidget(cancelButton, 1, 0);
		mainLayout->addLayout(buttonLayout);
		setLayout(mainLayout);
	}
	capture.open(0);
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(displayVideoFrame()));
	timer->start(20);
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
	capture.release();
	timer->stop();
}

// this is for the "X" Button event
void TakePicView::closeEvent(QCloseEvent * event)
{
	this->stopVideo();
}

void TakePicView::takePicture()
{

}