#ifndef SCANINPUTVIEW_H
#define SCANINPUTVIEW_H

//#include "scanController.h"
#include <qwidget.h>
#include <qapplication.h>
#include "scanningView.h"
class ScanController;

class ScanInputView : public QWidget
{
    Q_OBJECT

private slots:
	void createScanView();
private:
	ScanningView * scanningView;
public:
    explicit ScanInputView(QWidget *parent = 0);

};

#endif