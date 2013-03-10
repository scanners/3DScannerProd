#ifndef OVERLAYVIEW_H
#define OVERLAYVIEW_H

#include <qdialog.h>
#include "stdafx.h"
using cv::Mat;
using cv::VideoCapture;

class ScanController;
class QWidget;
class QLabel;
class QPushButton;
class QBoxLayout;
class QGridLayout;
class ScanController;
class OverlayLabel;

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
	QPushButton * startButton;
	QPushButton * resetButton;
	QPushButton * exitButton;
	QBoxLayout * mainLayout;
	QGridLayout * buttonLayout;
	VideoCapture capture;
	int y; // y-position of click
	int x; // x-position of click (for testing purposes)
	Mat image;
	void constructLayout();
	void updateCoords();

public:
	OverlayView(QWidget * parent = 0);
	void setScanController(ScanController& scanController);
};

#endif //OVERLAYVIEW_H

