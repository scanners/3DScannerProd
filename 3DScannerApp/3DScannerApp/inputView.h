#ifndef INPUTVIEW_H
#define INPUTVIEW_H

#include "stdafx.h"
#include "takePicView.h"

class QLabel;
class QString;
class QLineEdit;
class QGridLayout;
class QPushButton;


class CalibrationController;

class InputView : public QWidget
{
	Q_OBJECT

private slots:
	void createTakePicView();
	void createFileDialog();
private:
	CalibrationController * calibControl;
	QLabel * message;
	TakePicView * calibPicView;
	int calibrationType;
	QString dir;
	QLineEdit * loadDirText;
	QGridLayout * mainLayout;
	QLabel * horizontalLabel;
	QLabel * verticalLabel;
	QLabel * messagesLabel;
	QPushButton * startButton;
	QPushButton * exitButton;
	QLineEdit * horizontalText;
	QLineEdit * verticalText;
	QLabel * loadLabel; // for extrinsic calib
	QPushButton * browseButton;
	void constructLayout(int calibType);
public:
	explicit InputView(int calibType, QWidget *parent = 0);
	void showMessage(QString msg);
	void setCalibrationController(CalibrationController& calibControl);
};

#endif