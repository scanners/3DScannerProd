#ifndef INPUTVIEW_H
#define INPUTVIEW_H

#include "stdafx.h"
#include <qlabel.h>
#include <string>
#include "takePicView.h"
class CalibrationController;

class InputView : public QWidget
{
	Q_OBJECT

private slots:
	void createTakePicView();
private:
	CalibrationController * calibControl;
	QLabel * message;
	TakePicView * calibPicView;
	int calibrationType;
public:
	explicit InputView(int calibType, QWidget *parent = 0);
	void showMessage(QString msg);
	void setCalibrationController(CalibrationController& calibControl);
};

#endif