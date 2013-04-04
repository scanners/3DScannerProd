#ifndef TAKEPICVIEW_H
#define TAKEPICVIEW_H

//#include "scanController.h"
#include "calibrationController.h"
#include "stdafx.h"
#include <qdialog.h>

using namespace::cv;
class QLabel;
class QTimer;
class QPushButton;
class QGridLayout;
class QBoxLayout;

class ScanController;

class TakePicView : public QDialog
{
    Q_OBJECT

private:
	QLabel * titleLabel;
	QLabel * videoLabel;
	QLabel * picProgressLabel;
	QLabel * messageLabel;
	QLabel * messages;
	QTimer * timer;
	QPushButton * takePicButton;
	QPushButton * cancelButton;
	QBoxLayout * mainLayout;
	QGridLayout * buttonLayout;
	VideoCapture capture;
	Mat image;
	CalibrationController * calibrationController;
public:
    explicit TakePicView(int calibType, QWidget *parent = 0);
	~TakePicView();
	void setCalibrationController(CalibrationController& calibControl);
	void incrementSuccesses(int successes, int requiredNumSuccesses);
	void showMessage(int messageEnum);
protected:
	void closeEvent(QCloseEvent * event);
private slots:
	void displayVideoFrame();
	void takePicture();
	void stopVideo();
};

#endif