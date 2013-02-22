#include "scanInputView.h"
#include <qapplication.h>
#include "qlabel.h"
#include "qpushbutton.h"
#include "qgridlayout.h"


ScanInputView::ScanInputView(QWidget *parent) : QWidget(parent)
{
	constructLayout();

	connect(startButton, SIGNAL(clicked()), this, SLOT(createScanView()));
	connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

	
}

void ScanInputView::createScanView()
{
	scanningView = new ScanningView();
	scanningView->setModal(true);
	scanningView->show();
}

void ScanInputView::constructLayout()
{
	infoLabel = new QLabel("Please click \"Start Scan\" below to being scan process.");
	startButton = new QPushButton("Start Scan");
	exitButton = new QPushButton("Exit");
	mainLayout = new QGridLayout();
	mainLayout->addWidget(infoLabel, 0, 0);
	mainLayout->addWidget(startButton, 0, 1);
	mainLayout->addWidget(exitButton, 0, 2);

	setLayout(mainLayout);
}