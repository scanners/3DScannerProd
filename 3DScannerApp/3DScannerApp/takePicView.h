#ifndef TAKEPICVIEW_H
#define TAKEPICVIEW_H

//#include "scanController.h"
#include <qwidget.h>
#include <qdialog.h>
class ScanController;

class TakePicView : public QDialog
{
    Q_OBJECT

private:
	QWidget * calibFrame;
public:
    explicit TakePicView(int calibType, QWidget *parent = 0);
	void displayVideo();
	void stopVideo();
};

#endif