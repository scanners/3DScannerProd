#ifndef TAKEPICVIEW_H
#define TAKEPICVIEW_H

//#include "scanController.h"
#include "stdafx.h"
#include <qwidget.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qtimer.h>
using namespace::cv;

class ScanController;

class TakePicView : public QDialog
{
    Q_OBJECT

private:
	QLabel * videoLabel;
	QTimer * timer;
	VideoCapture capture;
	Mat image;
public:
    explicit TakePicView(int calibType, QWidget *parent = 0);

protected:
	void closeEvent(QCloseEvent * event);
private slots:
	void displayVideoFrame();
	void takePicture();
	void stopVideo();
};

#endif