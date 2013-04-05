#include "resultsView.h"
#include <qdialog.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qdialogbuttonbox.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qlineedit.h>
#include <qfiledialog.h>

ResultsView::ResultsView(QWidget *parent) : QWidget(parent)
{
	// NOTE: Probably going to need some logic to check to see
	// if scan is complete
	constructLayout();
	

	connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
	connect(browseButton, SIGNAL(clicked()), this, SLOT(createFileDialog()));

	
}

void ResultsView::createFileDialog()
{
	dir = QFileDialog::getExistingDirectory(this, "Select Output Directory",
		"C:/", QFileDialog::ShowDirsOnly);
	outputDirTextField->setText(dir);
}

void ResultsView::constructLayout()
{
	pointCloudFrame = new QWidget(this);
	pointCloudFrame->setFixedSize(300, 300);
	pointCloudFrame->setStyleSheet("background-color: black;");
	pointCloudLabel = new QLabel("Point Cloud:");
	title = new QLabel("<h1>Scan Results</h1>");
	browseButton = new QPushButton("Browse...");
	exitButton = new QPushButton("Exit");
	outputDirTextField = new QLineEdit();
	outputDirTextField->setPlaceholderText("Type directory, or click \"browse\" to the right");
	outputDirLabel = new QLabel("Output Directory:");
	exitButton->setMaximumWidth(80);
	browseButton->setMaximumWidth(80);

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

ResultsView::~ResultsView()
{
	/*delete title;
	delete directoryLabel;
	delete exitButton;
	delete browseButton;
	delete mainLayout;
	delete dirLayout;
	delete outputDirTextField;
	delete outputDirLabel;
	delete pointCloudLabel;*/
}