#include "takePicView.h"
#include "enums.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qboxlayout.h"
#include "qgridlayout.h"

TakePicView::TakePicView(int calibType, QWidget *parent) : QDialog(parent)
{
	if (calibType == Enums::controllerEnum::INTRINSIC)
	{
		// create intrinsic calib stuff
		calibFrame = new QWidget(this);
		calibFrame->setFixedSize(300, 300);
		calibFrame->setStyleSheet("background-color: black;");
		QLabel * titleLabel = new QLabel("Intrinsic Calibration");
		QLabel * picProgressLabel = new QLabel("Picture 1 of 20");
		QLabel * messageLabel = new QLabel("Messages: ");
		QPushButton * takePicButton = new QPushButton("Take Picture");
		QPushButton * cancelButton = new QPushButton("Cancel");

		connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

		// layout code
		QBoxLayout * mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
		mainLayout->addWidget(titleLabel);
		mainLayout->addWidget(calibFrame);
		QGridLayout * buttonLayout = new QGridLayout();
		buttonLayout->addWidget(takePicButton, 0, 0);
		buttonLayout->addWidget(cancelButton, 1, 0);
		mainLayout->addLayout(buttonLayout);
		setLayout(mainLayout);
	}
	else if (calibType == Enums::controllerEnum::EXTRINSIC)
	{
		// create extrin calib stuff
		calibFrame = new QWidget(this);
		calibFrame->setFixedSize(300, 300);
		calibFrame->setStyleSheet("background-color: black;");
		QLabel * titleLabel = new QLabel("Extrinsic Calibration");
		QLabel * picProgressLabel = new QLabel("Picture 1 of 4");
		QLabel * messageLabel = new QLabel("Messages: ");
		QPushButton * takePicButton = new QPushButton("Take Picture");
		QPushButton * cancelButton = new QPushButton("Cancel");

		connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

		// layout code
		QBoxLayout * mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
		mainLayout->addWidget(titleLabel);
		mainLayout->addWidget(calibFrame);
		QGridLayout * buttonLayout = new QGridLayout();
		buttonLayout->addWidget(takePicButton, 0, 0);
		buttonLayout->addWidget(cancelButton, 1, 0);
		mainLayout->addLayout(buttonLayout);
		setLayout(mainLayout);
	}
}

void TakePicView::displayVideo()
{

}

void TakePicView::stopVideo()
{

}