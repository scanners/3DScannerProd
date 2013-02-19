#include "scanningView.h"
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qboxlayout.h>

ScanningView::ScanningView(QWidget *parent) : QDialog(parent)
{
	// create widgets:
	
	scanFrame = new QWidget(this);
	scanFrame->setFixedSize(300, 300);
	scanFrame->setStyleSheet("background-color: black;");
	QLabel * titleLabel = new QLabel("<h1>Scanning Process</h1>");
	QPushButton * startButton = new QPushButton("Start Scan");
	startButton->setMaximumWidth(80);
	
	QPushButton * exitButton = new QPushButton("Exit");
	exitButton->setMaximumWidth(80);

	QBoxLayout * mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	mainLayout->addWidget(titleLabel);
	mainLayout->addWidget(scanFrame);
	QGridLayout * buttonLayout = new QGridLayout();
	buttonLayout->addWidget(startButton, 0, 0);
	buttonLayout->addWidget(exitButton, 1, 0);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);


}

void ScanningView::setScanController(ScanController& sc)
{

}

void ScanningView::displayVideo()
{

}

void ScanningView::stopVideo()
{

}

void ScanningView::showMessage(QString message)
{

}

void ScanningView::updateProgressBar(int processed, int done)
{

}