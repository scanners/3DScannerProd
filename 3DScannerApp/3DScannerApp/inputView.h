#ifndef INPUTVIEW_H
#define INPUTVIEW_H

#include "stdafx.h"
#include <qlabel.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <string>

class TakePicView;
class InputView : public QWidget
{
	Q_OBJECT

private slots:
	void createTakePicView();
	void createFileDialog();
private:
	QLabel * message;
	TakePicView * calibPicView;
	int calibrationType;
	QString dir;
	QLineEdit * loadDirText;
public:
	explicit InputView(int calibType, QWidget *parent = 0);
	void showMessage(QString msg);
};

#endif