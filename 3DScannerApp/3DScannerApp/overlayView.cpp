#include "overlayView.h"
#include "overlayLabel.h"
#include "scanController.h"
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qboxlayout.h>
#include <qtimer.h>
#include <qpainter.h>
using cv::Scalar;

OverlayView::OverlayView(QWidget * parent) : QDialog(parent) {
	numClicks = 0; // no clicks have been made yet
	// construct layout
	constructLayout();
	// add the signals/slots:
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetClicks()));
	connect(exitButton, SIGNAL(clicked()), this, SLOT(stopVideo()));
	connect(exitButton, SIGNAL(clicked()), this, SLOT(reject()));
	// open video capture stuff
	capture.open(0);
	if (capture.isOpened())
	{
		// display the initial frame
		//this->displayCameraFrame();
		// only want to connect this here, because otherwise users can click before frame is rendered
		connect(displayImage, SIGNAL(mousePressed()), this, SLOT(obtainCoordinates()));
		// when the start button is clicked, display the single frame for overlay
		connect(resetButton, SIGNAL(clicked()), this, SLOT(displayCameraFrame()));
	}
	// othewise, just display an error message in the frame
	else
	{
		displayImage->setPixmap(QPixmap("noCamera.png"));
	}

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
	//refreshButton = new QPushButton("Refresh Image");
	//refreshButton->setMaximumWidth(80);
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
	//buttonLayout->addWidget(refreshButton, 0, 1);
	buttonLayout->addWidget(resetButton, 0, 1);
	buttonLayout->addWidget(exitButton, 0, 2);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);
}

// this method will simply display a single frame to the overlayLabel
void OverlayView::displayCameraFrame()
{
	capture.read(image);
	scanController->setImageWidth(image);
	cvtColor(image, display, CV_BGR2RGB);
	//QImage qimg((uchar*)image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
	videoFrame = new QImage((uchar*)display.data, display.cols, display.rows, display.step, QImage::Format_RGB888);
	//displayImage->setPixmap(QPixmap::fromImage(qimg));
	displayImage->setPixmap(QPixmap::fromImage(*videoFrame));
}

void OverlayView::resetClicks()
{
	numClicks = 0;
	scanController->resetRegions();
	this->displayCameraFrame();
	// todo
	this->y = 0;
	this->x = 0;
	updateCoords();
}

void OverlayView::obtainCoordinates()
{
	this->x = displayImage->getX();
	this->y = displayImage->getY();
	numClicks++; // another click has been made
	if (numClicks <= 4)
	{
		scanController->setRegion(this->y); // send the y-click
	}
	updateCoords();
}

void OverlayView::updateCoords()
{
	std::stringstream stream;
	stream << "X = " << this->x << " Y = " << this->y;
	positionLabel->setText(stream.str().c_str());
}

void OverlayView::drawOverlayRegions(vector<Point> coords, int rectWidth)
{
	// create painter for displaying overlays:
	QPainter painter;
	painter.begin(videoFrame); // tell the painter where it is drawing
	QLinearGradient linearGrad(QPointF(0, 0), QPointF(rectWidth, 0));
	linearGrad.setColorAt(0, Qt::darkGray);
	linearGrad.setColorAt(1, Qt::black);
	painter.fillRect(coords.at(0).x, coords.at(0).y, rectWidth, coords.at(1).y-coords.at(0).y, linearGrad);
	painter.drawText(QPointF(10, (coords.at(0).y) + 10), "Back Plane");
	painter.fillRect(coords.at(2).x, coords.at(2).y, rectWidth, coords.at(3).y-coords.at(2).y, linearGrad);
	painter.drawText(QPointF(10, (coords.at(2).y) + 10), "Ground Plane");
	painter.end(); // free up resources
	// refresh the image
	displayImage->setPixmap(QPixmap::fromImage(*videoFrame));
}