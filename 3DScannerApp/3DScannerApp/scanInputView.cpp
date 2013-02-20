#include "scanInputView.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qgridlayout.h"

ScanInputView::ScanInputView(QWidget *parent) : QWidget(parent)
{
	QLabel * info = new QLabel("Please click \"Start Scan\" below to being scan process.");
	QPushButton * startButton = new QPushButton("Start Scan");
	QPushButton * exitButton = new QPushButton("Exit");
	QGridLayout * mainLayout = new QGridLayout();
	mainLayout->addWidget(info, 0, 0);
	mainLayout->addWidget(startButton, 0, 1);
	mainLayout->addWidget(exitButton, 0, 2);

	connect(startButton, SIGNAL(clicked()), this, SLOT(createScanView()));
	connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

	setLayout(mainLayout);
}

void ScanInputView::createScanView()
{
	scanningView = new ScanningView();
	scanningView->setModal(true);
	scanningView->show();
}