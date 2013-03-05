#include "scanningView.h"
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qboxlayout.h>
#include <qtimer.h>
using cv::Scalar;


ScanningView::ScanningView(QWidget *parent) : QDialog(parent)
{
	constructLayout();
	// connections
	connect(exitButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(exitButton, SIGNAL(clicked()), this, SLOT(stopVideo()));
	capture.open(0);
	if (capture.isOpened())
	{
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(displayVideoFrame()));
		timer->start(20);
	}
}

void ScanningView::setScanController(ScanController& sc)
{

}

void ScanningView::stopVideo()
{
	if (capture.isOpened())
	{
		capture.read(image);
		/* 
		We might be able to do something like this to loop through the Mat
		for(int row = 0; row < redFrame.rows; ++row)
		{
			uchar* p = redFrame.ptr(row); // gets a pointer to the row
			for (int col = 0; col < redFrame.cols; ++col)
			{
				*p++;
			}
		}
		*/

		capture.release();
		timer->stop();
	}
}

void ScanningView::closeEvent(QCloseEvent * event)
{
	this->stopVideo();
}

void ScanningView::showMessage(QString message)
{

}

void ScanningView::updateProgressBar(int processed, int done)
{

}

void ScanningView::constructLayout()
{
	videoLabel = new QLabel(this);
	videoLabel->setMinimumWidth(640);
	videoLabel->setMinimumHeight(480);
	// create widgets:
	titleLabel = new QLabel("<h1>Scanning Process</h1>");
	startButton = new QPushButton("Start Scan");
	startButton->setMaximumWidth(80);
	exitButton = new QPushButton("Cancel");
	exitButton->setMaximumWidth(80);
	// create layouts and add the widgets
	mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	buttonLayout = new QGridLayout();
	mainLayout->addWidget(titleLabel);
	mainLayout->addWidget(videoLabel);
	buttonLayout->addWidget(startButton, 0, 0);
	buttonLayout->addWidget(exitButton, 1, 0);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);
}

void ScanningView::displayVideoFrame()
{
	capture.read(image); // read the image from the videocapture input
	cvtColor(image, image, CV_BGR2RGB); // convert from BGR to RGB
	inRange(image, Scalar(139, 0, 0), Scalar(255, 255, 255), redFrame); // get the red component
	QImage qimg((uchar*)redFrame.data, redFrame.cols, redFrame.rows, redFrame.step, QImage::Format_Indexed8);
	videoLabel->setPixmap(QPixmap::fromImage(qimg));
}