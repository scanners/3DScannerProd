#ifndef HOMEVIEW_H
#define HOMEVIEW_H

#include <qdialog.h>
#include <qdialogbuttonbox.h>
#include <qtabwidget.h>
class ScanController;
class CalibrationController;
class CalibrationModel;
class ScanModel;

class HomeView : public QDialog
{
	Q_OBJECT

public:
	explicit HomeView(QWidget *parent = 0);
	void setScanController(const ScanController& scanControl);

private:
	QTabWidget *tabWidget;
	QDialogButtonBox *buttonBox;
	ScanController * scanControl;
	CalibrationController * intrinsicControl;
	CalibrationController * extrinsicControl;
	CalibrationModel * calibModel;
	ScanModel * scanModel;
};

class TestTab : public QWidget
{
	Q_OBJECT

public:
	explicit TestTab(QWidget *parent = 0);
};

#endif