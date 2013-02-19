#ifndef SCANNINGVIEW_H
#define SCANNINGVIEW_H

//#include "scanController.h"
#include <qwidget.h>
#include <qdialog.h>
class ScanController;

class ScanningView : public QDialog
{
    Q_OBJECT

private:
	QWidget * scanFrame;
public:
    explicit ScanningView(QWidget *parent = 0);
	void setScanController(ScanController& sc);
	void displayVideo();
	void stopVideo();
	void showMessage(QString message);
	void updateProgressBar(int processed, int done);
};

#endif