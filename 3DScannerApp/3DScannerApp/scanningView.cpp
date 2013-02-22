#include "scanningView.h"
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qboxlayout.h>


ScanningView::ScanningView(QWidget *parent) : QDialog(parent)
{
	constructLayout();
	// connections
	connect(exitButton, SIGNAL(clicked()), this, SLOT(reject()));
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

void ScanningView::constructLayout()
{
	// create widgets:
	scanFrame = new QWidget();
	scanFrame->setFixedSize(300, 300);
	scanFrame->setStyleSheet("background-color: black;");
	titleLabel = new QLabel("<h1>Scanning Process</h1>");
	startButton = new QPushButton("Start Scan");
	startButton->setMaximumWidth(80);
	exitButton = new QPushButton("Cancel");
	exitButton->setMaximumWidth(80);
	// create layouts and add the widgets
	mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	buttonLayout = new QGridLayout();
	mainLayout->addWidget(titleLabel);
	mainLayout->addWidget(scanFrame);
	buttonLayout->addWidget(startButton, 0, 0);
	buttonLayout->addWidget(exitButton, 1, 0);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);
}