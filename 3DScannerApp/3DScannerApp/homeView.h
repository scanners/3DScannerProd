#ifndef HOMEVIEW_H
#define HOMEVIEW_H

// NOTE: THIS CLASS AND OTHER CLASSES THAT HAVE POINTERS NEED DESTRUCTORS, ETC!
// OTHERWISE WE WILL RUN INTO MEMORY LEAK PROBLEMS

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
	~HomeView();
	void setScanController(const ScanController& scanControl);

private:
	QTabWidget *tabWidget;
	QDialogButtonBox *buttonBox;
	ScanController * scanControl;
	CalibrationController * intrinsicController;
	CalibrationController * extrinsicController;
	CalibrationModel * intrinsicCalibrationModel;
	CalibrationModel * extrinsicCalibrationModel;
	ScanController * scanController;
	ScanModel * scanModel;
};

class HomeTab : public QWidget
{
	Q_OBJECT

public:
	explicit HomeTab(QWidget *parent = 0);
};

#endif