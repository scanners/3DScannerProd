#ifndef SCANNINGVIEW_H
#define SCANNINGVIEW_H

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
	QWidget * scanFrame;
	QLabel * titleLabel;
	QPushButton * cancelButton;
	QBoxLayout * mainLayout;
	QGridLayout * buttonLayout;
	QTimer * timer;
	VideoCapture capture;
	Mat image;
	QLabel * videoLabel;
	QProgressBar * progressBar;
	QLabel * progressLabel;
	void constructLayout();
	void releaseVideo();
protected:
	void closeEvent(QCloseEvent * event);
public:
    explicit ScanningView(QWidget *parent = 0);
	void setScanController(ScanController& scanController);
	void showMessage(QString message);
	void updateProgressBar(int processed, int done);
	void stopVideo();
};

#endif