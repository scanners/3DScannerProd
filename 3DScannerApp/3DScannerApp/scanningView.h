#ifndef SCANNINGVIEW_H
#define SCANNINGVIEW_H

//#include "scanController.h"
#include <qdialog.h>
#include "stdafx.h"
class ScanController;
class QWidget;
class QLabel;
class QPushButton;
class QBoxLayout;
class QGridLayout;

using cv::Mat;
using cv::VideoCapture;


class ScanningView : public QDialog
{
    Q_OBJECT

private slots:	
	void displayVideoFrame();
	void stopVideo();
private:
	QWidget * scanFrame;
	QLabel * titleLabel;
	QPushButton * startButton;
	QPushButton * exitButton;
	QBoxLayout * mainLayout;
	QGridLayout * buttonLayout;
	QTimer * timer;
	VideoCapture capture;
	Mat image;
	Mat redFrame;
	QLabel * videoLabel;
	void constructLayout();
protected:
	void closeEvent(QCloseEvent * event);
public:
    explicit ScanningView(QWidget *parent = 0);
	void setScanController(ScanController& sc);
	void showMessage(QString message);
	void updateProgressBar(int processed, int done);
};

#endif