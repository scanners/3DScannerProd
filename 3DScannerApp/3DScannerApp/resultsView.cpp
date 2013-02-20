#include "resultsView.h"

ResultsView::ResultsView(QWidget *parent) : QWidget(parent)
{
	// NOTE: Probably going to need some logic to check to see
	// if scan is complete

	pointCloudFrame = new QWidget(this);
	pointCloudFrame->setFixedSize(300, 300);
	pointCloudFrame->setStyleSheet("background-color: black;");
	pointCloudLabel = new QLabel("Point Cloud:");
	title = new QLabel("<h1>Scan Results</h1>");
	browseButton = new QPushButton("Browse...");
	exitButton = new QPushButton("Exit");
	outputDirTextField = new QLineEdit("[Input Here or click browse]");
	outputDirLabel = new QLabel("Output Directory:");
	exitButton->setMaximumWidth(80);
	browseButton->setMaximumWidth(80);

	connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

	mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	mainLayout->addWidget(title);
	dirLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	dirLayout->addWidget(outputDirLabel);
	dirLayout->addWidget(outputDirTextField);
	dirLayout->addWidget(browseButton);
	mainLayout->addLayout(dirLayout);
	mainLayout->addWidget(pointCloudLabel);
	mainLayout->addWidget(pointCloudFrame);
	mainLayout->addWidget(exitButton);
	setLayout(mainLayout);
}