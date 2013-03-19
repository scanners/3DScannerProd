#include "scanningView.h"
#include "scanController.h"
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
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelScan()));
	capture.open(0);
	if (capture.isOpened())
	{
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(displayVideoFrame()));
		connect(timer, SIGNAL(timeout()), this, SLOT(scanImage()));
		timer->start(20);
	}
	else
	{
		videoLabel->setPixmap(QPixmap("noCamera.png"));
	}
}

void ScanningView::setScanController(ScanController& scanControl)
{
	scanController = &scanControl;
}

void ScanningView::cancelScan() {
	this->releaseVideo();
	scanController->resetScan();
}

void ScanningView::stopVideo()
{
	if (capture.isOpened())
	{
		timer->stop();
	}
}

void ScanningView::releaseVideo() {
	if (capture.isOpened()) {
		capture.release();
		timer->stop();
	}
}

void ScanningView::closeEvent(QCloseEvent * event)
{
	this->cancelScan();
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
	cancelButton = new QPushButton("Cancel");
	cancelButton->setMaximumWidth(80);
	// create layouts and add the widgets
	mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	buttonLayout = new QGridLayout();
	mainLayout->addWidget(titleLabel);
	mainLayout->addWidget(videoLabel);
	buttonLayout->addWidget(cancelButton, 0, 0);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);
}

void ScanningView::displayVideoFrame()
{
	Mat displayImage;
	capture.read(image);
	cvtColor(image, displayImage, CV_BGR2RGB);
	QImage qimg((uchar*)displayImage.data, displayImage.cols, displayImage.rows, displayImage.step, QImage::Format_RGB888);
	videoLabel->setPixmap(QPixmap::fromImage(qimg));
}

void ScanningView::scanImage() {
	//For testing
	static int numImages = 0;
	if (numImages >= 10) {
		this->stopVideo();
		scanController->startScan();
		numImages = 0;
	} else {
		numImages++;
	} //End test section
	
	scanController->sendImage(image);
	
}