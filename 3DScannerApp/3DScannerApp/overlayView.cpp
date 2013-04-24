#include "overlayView.h"
#include "overlayLabel.h"
#include "scanController.h"
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qboxlayout.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qmessagebox.h>
using cv::Scalar;

OverlayView::OverlayView(QWidget * parent) : QDialog(parent) {
	numClicks = 0; // no clicks have been made yet
	// construct layout
	constructLayout();
	// add the signals/slots:
	connect(takePicButton, SIGNAL(clicked()), this, SLOT(takePicture()));
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetClicks()));
	connect(exitButton, SIGNAL(clicked()), this, SLOT(stopVideo()));
	connect(exitButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(startButton, SIGNAL(clicked()), this, SLOT(startScan()));
	// open video capture stuff
	capture.open(0);
	if (capture.isOpened())
	{
		// if the Video Capture Stream is open, set button and create timer
		takePicButton->setEnabled(true);
		setButtonStyle(takePicButton, true);
		timer = new QTimer(this);
		// slot for displaying video every 20ms
		connect(timer, SIGNAL(timeout()), this, SLOT(displayCameraFrame()));
		timer->start(20);
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
	// reset clicks and state when exit button is clicked
	this->resetClicks();
	if (capture.isOpened())
	{
		capture.release();
		timer->stop();
	}
}

void OverlayView::closeEvent(QCloseEvent * event)
{
	this->resetClicks();
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
	setButtonStyle(startButton, false);
	startButton->setMaximumWidth(80);
	takePicButton = new QPushButton("Being Region Clicking");
	//takePicButton->setMaximumWidth(120);
	takePicButton->setEnabled(false);
	setButtonStyle(takePicButton, false);

	resetButton = new QPushButton("Reset");
	resetButton->setMaximumWidth(80);
	resetButton->setEnabled(false);
	setButtonStyle(resetButton, false);

	exitButton = new QPushButton("Exit");
	setButtonStyle(exitButton, true);
	exitButton->setMaximumWidth(80);
	// create layouts and add the widgets
	mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	buttonLayout = new QGridLayout();
	mainLayout->addWidget(titleLabel);
	mainLayout->addWidget(displayImage);
	mainLayout->addWidget(positionLabel);
	buttonLayout->addWidget(startButton, 0, 0);
	buttonLayout->addWidget(takePicButton, 0, 1, 1, 2);
	buttonLayout->addWidget(resetButton, 0, 3);
	buttonLayout->addWidget(exitButton, 0, 4);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);
}

void OverlayView::takePicture() {
	resetButton->setEnabled(true);
	setButtonStyle(resetButton, true);
	timer->stop();
	scanController->setImageWidth(image);
	takePicButton->setEnabled(false);
	setButtonStyle(takePicButton, false);
	resetButton->setToolTip("Retakes picture and resets clicks/regions");
	// only want to connect this here, because otherwise users can click before frame is rendered
	connect(displayImage, SIGNAL(mousePressed()), this, SLOT(obtainCoordinates()));
	// when the start button is clicked, display the single frame for overlay
	connect(resetButton, SIGNAL(clicked()), this, SLOT(displayCameraFrame()));
}

void OverlayView::setButtonStyle(QPushButton * button, bool isEnabled)
{
	if (isEnabled == true)
	{
		button->setStyleSheet("QPushButton {"
			"background-color: #333;"
			"color : white;"
			"border-style: outset;"
			"border-width: 2px;"
			"border-radius: 10px;"
			"border-color: white;"
			"font: bold 14px;"
			"min-width: 5em;"
			"padding: 4px;}"
			"QPushButton:hover {"
			"background-color: #FFF;"
			"border: 1px solid black;"
			"color: #000;}"
			);
	}
	else
	{
		button->setStyleSheet("QPushButton {"
			"background-color: #CCC;"
			"color : #AAA;"
			"border-style: outset;"
			"border-width: 1px;"
			"border-radius: 10px;"
			"border-color: white;"
			"font: bold 14px;"
			"min-width: 5em;"
			"padding: 4px;}"
			);
	}
}

// this method will simply display a single frame to the overlayLabel
void OverlayView::displayCameraFrame()
{
	if (capture.isOpened())
	{
		capture.read(image);
		cvtColor(image, display, CV_BGR2RGB);
		videoFrame = new QImage((uchar*)display.data, display.cols, display.rows, display.step, QImage::Format_RGB888);
		displayImage->setPixmap(QPixmap::fromImage(*videoFrame));
	}
}

void OverlayView::resetClicks()
{
	numClicks = 0;
	startButton->setEnabled(false);
	setButtonStyle(startButton, false);
	scanController->resetRegions();
	this->displayCameraFrame();
	//To be used to display ? instead of int
	this->y = -1;
	this->x = -1;
	updateCoords();
}

void OverlayView::obtainCoordinates()
{
	this->x = displayImage->getX();
	this->y = displayImage->getY();
	numClicks++; // another click has been made
	
	if (numClicks <= scanController->getRequiredNumStoredYCoords()) //Region Y Clicking
	{
		scanController->setYRegion(this->y); // send the y-click
	} else if (numClicks <= 
			(scanController->getRequiredNumStoredXCoords() + scanController->getRequiredNumStoredYCoords())) { // Region X Clicking
		scanController->setXRegion(this->x);
	}
	updateCoords();
}

void OverlayView::updateCoords()
{
	std::stringstream stream;
	//Reset was clicked
	if ((this->x == -1) && (this->y == -1)) {
		stream << "X = ? Y = ?";
	} else {
		stream << "X = " << this->x << " Y = " << this->y;
	}
	positionLabel->setText(stream.str().c_str());
}

void OverlayView::drawOverlayYRegions(vector<Point> coords, int rectWidth)
{
	// create painter for displaying overlays:
	QPainter painter;
	painter.begin(videoFrame); // tell the painter where it is drawing
	QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, 0));
	linearGrad.setColorAt(0, QColor(255,255,255, 180));
	linearGrad.setColorAt(1, QColor(255,255,255, 180));
	painter.fillRect(coords.at(0).x, coords.at(0).y, rectWidth, coords.at(1).y-coords.at(0).y, linearGrad);
	painter.setFont(QFont("Verdana", 24, 10, false));
	painter.drawText(QPointF((rectWidth/2)-100, coords.at(0).y + 30), "Back Plane");
	painter.fillRect(coords.at(2).x, coords.at(2).y, rectWidth, coords.at(3).y-coords.at(2).y, linearGrad);
	painter.drawText(QPointF((rectWidth/2)-100, (coords.at(2).y) + 30), "Ground Plane");
	painter.end(); // free up resources

	// refresh the image
	displayImage->setPixmap(QPixmap::fromImage(*videoFrame));
}

void OverlayView::drawOverlayXRegions(vector<Point> coords) {
	// create painter for displaying overlays:
	QPainter painter;
	painter.begin(videoFrame); // tell the painter where it is drawing
	QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, 0));
	linearGrad.setColorAt(0, QColor(150,150,150, 180));
	linearGrad.setColorAt(1, QColor(150,150,150, 180));
	painter.fillRect(coords.at(0).x, coords.at(0).y, coords.at(1).x-coords.at(0).x, coords.at(1).y-coords.at(0).y, linearGrad);
	painter.setFont(QFont("Verdana", 24, 10, false));
	painter.fillRect(coords.at(2).x, coords.at(2).y, coords.at(3).x-coords.at(2).x, coords.at(3).y-coords.at(2).y, linearGrad);
	painter.end(); // free up resources

	// refresh the image
	displayImage->setPixmap(QPixmap::fromImage(*videoFrame));
	//set accept button so the scan can begin
	startButton->setEnabled(true);
	setButtonStyle(startButton, true);
}

void OverlayView::startScan() {
	this->stopVideo();
	QMessageBox message;
	message.setIcon(QMessageBox::Information);
	message.setWindowTitle("Turn Off Lights");
	message.setText("Please turn off the lights.  Then hit 'OK' to begin the scan.\n"
		"Make sure your camera settings are properly adjusted for the scan.");
	message.exec();
	this->reject();
	scanController->createScanningView();
}

OverlayView::~OverlayView()
{
	if (displayImage)
		delete displayImage;
	delete titleLabel;
	delete positionLabel;
	if (videoFrame)
		delete videoFrame;
	delete timer;
	delete startButton;
	delete resetButton;
	delete exitButton;
	delete takePicButton;
	delete mainLayout;
	delete buttonLayout;
}