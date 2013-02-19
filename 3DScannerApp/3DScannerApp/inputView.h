#ifndef INPUTVIEW_H
#define INPUTVIEW_H

#include "stdafx.h"
#include <qlabel.h>
#include <string>
class CalibrationController;

class InputView : public QWidget
{
	Q_OBJECT

private:
	CalibrationController * calibControl;
	QLabel * message;
public:
	explicit InputView(int calibType, QWidget *parent = 0);
	void showMessage(QString msg);
	void setCalibrationController(CalibrationController& calibControl);
};

#endif