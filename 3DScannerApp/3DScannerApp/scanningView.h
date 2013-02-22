#ifndef SCANNINGVIEW_H
#define SCANNINGVIEW_H

//#include "scanController.h"
#include <qdialog.h>
class ScanController;
class QWidget;
class QLabel;
class QPushButton;
class QBoxLayout;
class QGridLayout;

class ScanningView : public QDialog
{
    Q_OBJECT

private:
	QWidget * scanFrame;
	QLabel * titleLabel;
	QPushButton * startButton;
	QPushButton * exitButton;
	QBoxLayout * mainLayout;
	QGridLayout * buttonLayout;
	void constructLayout();
public:
    explicit ScanningView(QWidget *parent = 0);
	void setScanController(ScanController& sc);
	void displayVideo();
	void stopVideo();
	void showMessage(QString message);
	void updateProgressBar(int processed, int done);
};

#endif