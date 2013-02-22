#ifndef SCANINPUTVIEW_H
#define SCANINPUTVIEW_H

#include <qwidget.h>
#include "scanningView.h"

class ScanController;
class QLabel;
class QPushButton;
class QGridLayout;

class ScanInputView : public QWidget
{
    Q_OBJECT

private slots:
	void createScanView();
private:
	ScanningView * scanningView;
	QLabel * infoLabel;
	QPushButton * startButton;
	QPushButton * exitButton;
	QGridLayout * mainLayout;
	void constructLayout();
public:
    explicit ScanInputView(QWidget *parent = 0);

};

#endif