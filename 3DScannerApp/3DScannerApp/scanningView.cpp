#include "scanningView.h"
#include "scanController.h"
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qboxlayout.h>
#include <qtimer.h>
#include <qprogressbar.h>
using cv::Scalar;


ScanningView::ScanningView(QWidget *parent) : QDialog(parent)
{
	constructLayout();
	// connections
	connect(doneButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(doneButton, SIGNAL(clicked()), this, SLOT(cancelScan()));
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
	this->messageLabel->setText(message);
}

void ScanningView::setProgressBarRange(int range) {
	//Subtract 1 from range because it starts at 0
	this->progressBar->setRange(0, range - 1);
}

void ScanningView::updateProgressBar(int done)
{
	this->progressBar->setValue(done);
}

void ScanningView::constructLayout()
{
	videoLabel = new QLabel(this);
	videoLabel->setMinimumWidth(640);
	videoLabel->setMinimumHeight(480);
	// create widgets:
	titleLabel = new QLabel("<h1>Scanning Process</h1>");
	doneButton = new QPushButton("Done");
	doneButton->setEnabled(false);
	doneButton->setMaximumWidth(80);
	progressBar = new QProgressBar();
	progressLabel = new QLabel("Scan Progress:");
	messageLabel = new QLabel();
	
	// create layouts and add the widgets
	mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	buttonLayout = new QGridLayout();
	mainLayout->addWidget(titleLabel);
	mainLayout->addWidget(videoLabel);
	buttonLayout->addWidget(doneButton, 0, 0);
	mainLayout->addLayout(buttonLayout);
	mainLayout->addWidget(messageLabel);
	mainLayout->addWidget(progressLabel);
	mainLayout->addWidget(progressBar);
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
	if (!(scanController->isDoneScanning())) {
		scanController->storeRedChannel(image);
	} else {
		this->stopVideo();
		scanController->processScan();		
	}
}

ScanningView::~ScanningView()
{
	delete videoLabel;
	delete titleLabel;
	delete cancelButton;
	delete progressBar;
	delete progressLabel;
	delete mainLayout;
	delete buttonLayout;
	delete timer;
}

void ScanningView::enableDoneButton(bool enable)
{
	this->doneButton->setEnabled(enable);
}