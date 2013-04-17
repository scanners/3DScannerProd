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
	void takePicture();
	void startScan();
protected:
	void closeEvent(QCloseEvent * event);
private:
	ScanController * scanController;
	OverlayLabel * displayImage;
	QLabel * titleLabel;
	QLabel * positionLabel;
	QImage * videoFrame;
	QTimer * timer;
	QPushButton * startButton;
	QPushButton * resetButton;
	QPushButton * exitButton;
	QPushButton * takePicButton; // used for getting the image initially
	QBoxLayout * mainLayout;
	QGridLayout * buttonLayout;
	VideoCapture capture;
	int y; // y-position of click
	int x; // x-position of click
	int numClicks;
	Mat image;
	Mat display;
	void constructLayout();
	void updateCoords();
	void setButtonStyle(QPushButton * button, bool isEnabled);
public:
	OverlayView(QWidget * parent = 0);
	~OverlayView();
	void setScanController(ScanController& scanController);
	void drawOverlayYRegions(vector<Point> coords, int imageWidth);
	void drawOverlayXRegions(vector<Point> coords);
};

#endif //OVERLAYVIEW_H