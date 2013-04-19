#ifndef SCANNINGVIEW_H
#define SCANNINGVIEW_H

#define EXPOSURE_VALUE -2
#define GAIN_VALUE 95
#define BRIGHTNESS_VALUE 128
#define CONTRAST_VALUE 32

//#include "scanController.h"
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
class QProgressBar;

class ScanningView : public QDialog
{
    Q_OBJECT

private slots:	
	void displayVideoFrame();
	void scanImage();
	void cancelScan();
private:
	ScanController * scanController;
	QLabel * globalText;
	QWidget * scanFrame;
	QLabel * titleLabel;
	QPushButton * cancelButton;
	QPushButton * doneButton;
	QBoxLayout * mainLayout;
	QGridLayout * buttonLayout;
	QTimer * timer;
	VideoCapture capture;
	Mat image;
	QLabel * videoLabel;
	QProgressBar * progressBar;
	QLabel * progressLabel;
	QLabel * messageLabel;
	void constructLayout();
	void releaseVideo();
	void setButtonStyle(QPushButton * button, bool isEnabled);
protected:
	void closeEvent(QCloseEvent * event);
public:
    explicit ScanningView(QWidget *parent = 0);
	~ScanningView();
	void setScanController(ScanController& scanController);
	void showMessage(QString message);
	void setProgressBarRange(int range);
	void updateProgressBar(int done);
	void stopVideo();
	void enableDoneButton(bool enable);
};

#endif