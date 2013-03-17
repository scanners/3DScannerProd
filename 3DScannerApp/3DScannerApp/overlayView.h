#ifndef OVERLAYVIEW_H
#define OVERLAYVIEW_H

#include <qdialog.h>
#include "stdafx.h"
using cv::Mat;
using cv::Point;
using cv::VideoCapture;
using std::vector;

class ScanController;
class QWidget;
class QLabel;
class QPushButton;
class QBoxLayout;
class QGridLayout;
class ScanController;
class OverlayLabel;
class QPainter;

class OverlayView : public QDialog {

Q_OBJECT

private slots:	
	void displayCameraFrame();
	void resetClicks();
	void stopVideo();
	void obtainCoordinates();
protected:
	void closeEvent(QCloseEvent * event);
private:
	ScanController * scanController;
	OverlayLabel * displayImage;
	QLabel * titleLabel;
	QLabel * positionLabel;
	QImage * videoFrame;
	QPushButton * startButton;
	QPushButton * resetButton;
	QPushButton * exitButton;
	//QPushButton * refreshButton; // used for getting the image initially and refreshing it
	QBoxLayout * mainLayout;
	QGridLayout * buttonLayout;
	VideoCapture capture;
	int y; // y-position of click
	int x; // x-position of click (for testing purposes)
	int numClicks;
	Mat image;
	Mat display;
	void constructLayout();
	void updateCoords();
public:
	OverlayView(QWidget * parent = 0);
	void setScanController(ScanController& scanController);
	void drawOverlayRegions(vector<Point> coords, int imageWidth);
};

#endif //OVERLAYVIEW_H

