#ifndef INPUTVIEW_H
#define INPUTVIEW_H

#include "stdafx.h"
#include <string>
#include <qdialog.h>

class QLabel;
class QLineEdit;
class QGridLayout;
class QPushButton;
class QDir;
class QTextEdit;

class TakePicView;
class CalibrationController;

class InputView : public QWidget
{
	Q_OBJECT

private slots:
	void startCalibration();
	void createLoadFileDialog();
	void createSaveFileDialog();
private:
	TakePicView * takePicView;
	CalibrationController * calibrationController;
	//QLabel * message;
	QTextEdit * message;
	int calibrationType;
	QDir * loadDir;
	QDir * saveDir;
	bool errors;
	QLineEdit * saveDirText;
	QLineEdit * loadDirText;
	QGridLayout * mainLayout;
	QLabel * horizontalLabel;
	QLabel * verticalLabel;
	QLabel * messagesLabel;
	QPushButton * startButton;
	QPushButton * exitButton;
	QLineEdit * horizontalText;
	QLineEdit * verticalText;
	QLabel * saveLabel;
	QLabel * loadLabel; // for extrinsic calib
	QPushButton * saveBrowseButton;
	QPushButton * loadBrowseButton;
	void constructLayout();
public:
	explicit InputView(int calibType, QWidget *parent = 0);
	~InputView(); // dtor!
	void showMessage(QString msg);
	void setCalibrationController(CalibrationController& calibControl);
};

#endif