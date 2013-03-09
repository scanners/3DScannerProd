#include "overlayView.h"
#include "overlayLabel.h"
#include "scanController.h"
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qboxlayout.h>
#include <qtimer.h>
using cv::Scalar;

OverlayView::OverlayView(QWidget * parent) : QDialog(parent) {
	// construct layout
	constructLayout();
	// add the signals/slots:
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetClicks()));
	connect(exitButton, SIGNAL(clicked()), this, SLOT(stopVideo()));
	connect(exitButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(displayImage, SIGNAL(mousePressed()), this, SLOT(obtainCoordinates()));
	// open video capture stuff: (we may actually wait until the click "start")
	capture.open(0);
	if (capture.isOpened())
	{
		// when the start button is clicked, display the single frame for overlay
		connect(startButton, SIGNAL(clicked()), this, SLOT(displayCameraFrame()));
	}
	displayImage->setPixmap(QPixmap("noCamera.png"));

}

void OverlayView::setScanController(ScanController& scanControl) {
	scanController = &scanControl;
}

void OverlayView::stopVideo()
{
	if (capture.isOpened())
	{
		capture.release();
	}
}

void OverlayView::closeEvent(QCloseEvent * event)
{
	this->stopVideo();
}

void OverlayView::constructLayout()
{
	displayImage = new OverlayLabel(this);
	displayImage->setMinimumWidth(640);
	displayImage->setMinimumHeight(480);
	// create widgets:
	titleLabel = new QLabel("<h1>Scanning Overlay Selection</h1>");
	positionLabel = new QLabel("X = ? Y = ?");
	startButton = new QPushButton("Start Scan");
	startButton->setEnabled(false);
	startButton->setMaximumWidth(80);
	resetButton = new QPushButton("Reset");
	resetButton->setMaximumWidth(80);
	exitButton = new QPushButton("Exit");
	exitButton->setMaximumWidth(80);
	// create layouts and add the widgets
	mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	buttonLayout = new QGridLayout();
	mainLayout->addWidget(titleLabel);
	mainLayout->addWidget(displayImage);
	mainLayout->addWidget(positionLabel);
	buttonLayout->addWidget(startButton, 0, 0);
	buttonLayout->addWidget(resetButton, 0, 1);
	buttonLayout->addWidget(exitButton, 0, 2);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);
}

// this method will simply display a single frame to the overlayLabel
void OverlayView::displayCameraFrame()
{
	capture.read(image);
	scanController->sendImage(image);
	QImage qimg((uchar*)image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
	displayImage->setPixmap(QPixmap::fromImage(qimg));
}

void OverlayView::resetClicks()
{
	// todo
	this->y = 0;
	this->x = 0;
	updateCoords();
}

void OverlayView::obtainCoordinates()
{
	this->x = displayImage->getX();
	this->y = displayImage->getY();
	std::stringstream stream;
	stream << "X = " << x << " Y = " << y;
	positionLabel->setText(stream.str().c_str());

}

void OverlayView::updateCoords()
{
	std::stringstream stream;
	stream << "X = " << this->x << " Y = " << this->y;
	positionLabel->setText(stream.str().c_str());
}