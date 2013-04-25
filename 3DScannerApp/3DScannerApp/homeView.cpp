#include <qwidget.h>
#include "homeView.h"
#include "inputView.h"
#include "scanInputView.h"
#include "resultsView.h"
#include "intrinsicController.h"
#include "extrinsicController.h"
#include "calibrationModel.h"
#include "scanController.h"
#include "scanModel.h"
#include <qlabel.h>
#include <qboxlayout.h>

HomeView::HomeView(QWidget *parent) : QDialog (parent) 
{
	this->setAutoFillBackground(true);
	this->setStyleSheet("background-color: #333;");
	InputView * intrinsicInputView = new InputView(Enums::controllerEnum::INTRINSIC);
	InputView * extrinsicInputView = new InputView(Enums::controllerEnum::EXTRINSIC);
	ScanInputView * scanInputView = new ScanInputView();
	//ResultsView * resultsView = new ResultsView();

	intrinsicController = new IntrinsicController();
	extrinsicController = new ExtrinsicController();
	intrinsicCalibrationModel = new CalibrationModel();
	extrinsicCalibrationModel = new CalibrationModel();
	intrinsicController->setCalibrationModel(*intrinsicCalibrationModel);
	extrinsicController->setCalibrationModel(*extrinsicCalibrationModel);
	intrinsicInputView->setCalibrationController(*intrinsicController);
	extrinsicInputView->setCalibrationController(*extrinsicController);
	intrinsicController->setInputView(*intrinsicInputView);
	extrinsicController->setInputView(*extrinsicInputView);

	scanController = new ScanController();
	scanModel = new ScanModel();
	scanController->setScanModel(*scanModel);
	scanInputView->setScanController(*scanController);
	scanController->setScanInputView(*scanInputView);

	tabWidget = new QTabWidget;
	tabWidget->addTab(new HomeTab(), "Home");
	tabWidget->addTab(intrinsicInputView, "Intrinsic Calibration");
	tabWidget->addTab(extrinsicInputView, "Extrinsic Calibration");
	tabWidget->addTab(scanInputView, "Scan Object");
	//tabWidget->addTab(resultsView, "Results/Export"); --> no time! taking it off the tabs!

	messageTitle = new QLabel("Messages: ");
	messageTitle->setStyleSheet(
			"QLabel {"
			"font-weight: bold; }"
			);

	messageText = new QLabel("");
	messageText->setStyleSheet( "QLabel { font-weight: bold; color: red; }");

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(tabWidget);
	mainLayout->addWidget(messageTitle);
	mainLayout->addWidget(messageText);
	setLayout(mainLayout);

	setWindowTitle("3D Scanner");
	this->setFixedSize(515, 300);

	this->setStyleSheet("QTabWidget::pane { background-color: white; border: 1px solid black;}"
		);
	tabWidget->setStyleSheet("QTabBar::tab {"
		"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
		"stop: 0 #FFF, stop: 1 #CCC);"
		"border: 1px solid #000;"
		"border-bottom-color: #000;" /* same as the pane color */
		"border-top-left-radius: 5px;"
		"border-top-right-radius: 5px;"
		"min-width: 12ex;"
		"padding: 6px; }"
		"QTabBar::tab:hover, QTabBar::tab:selected { background-color: white; border-bottom: 1px solid red }"
		);
	
	intrinsicInputView->connectGlobalMessage(messageText);
	extrinsicInputView->connectGlobalMessage(messageText);
	scanInputView->connectGlobalMessage(messageText);

}

HomeView::~HomeView()
{
	delete intrinsicController;
	delete extrinsicController;
	delete intrinsicCalibrationModel;
	delete extrinsicCalibrationModel;
	delete scanModel; // this is causing a memory exception.
	
	delete tabWidget;
	delete scanController;
}

HomeTab::HomeTab(QWidget *parent) : QWidget(parent)
{
	QLabel *title = new QLabel("<h1>Welcome to 3D Scanner</h1>");
	title->setStyleSheet("color: red;");
	QLabel *title2 = new QLabel("To get started, please click on the tabs above.");
	QLabel *title3 = new QLabel("Begin with <b>Intrinsic Calibration</b>, following the prompts.<br />"
		"Once complete, click on <b>Extrinsic Calibration</b> to begin extrinsic camera calibration.<br />"
		"Finally, scan the object in the <b>Scan</b> tab, which will be output as a VRML file.");
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(title);
	mainLayout->addWidget(title2);
	mainLayout->addWidget(title3);
	mainLayout->addStretch(1);
	setLayout(mainLayout);

}