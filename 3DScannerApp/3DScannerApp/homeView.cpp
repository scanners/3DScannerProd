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
	InputView * intrinsicInputView = new InputView(Enums::controllerEnum::INTRINSIC);
	InputView * extrinsicInputView = new InputView(Enums::controllerEnum::EXTRINSIC);
	ScanInputView * scanInputView = new ScanInputView();
	ResultsView * resultsView = new ResultsView();

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
	tabWidget->addTab(resultsView, "Results/Export");

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(tabWidget);
	setLayout(mainLayout);

	setWindowTitle("3D Scanner");
	//this->setFixedSize(550, 350);
	this->setFixedSize(515, 515);
	//this->setBaseSize(550, 600);
	
}

HomeView::~HomeView()
{
	delete intrinsicController;
	delete extrinsicController;
	delete intrinsicCalibrationModel;
	delete extrinsicCalibrationModel;
	//if (scanController) delete scanController;
	delete scanModel; // this is causing a memory exception.
	delete tabWidget;
}

HomeTab::HomeTab(QWidget *parent) : QWidget(parent)
{
	QLabel *title = new QLabel("<h1>Welcome to 3D Scanner</h1>");
	QLabel *title2 = new QLabel("To get started, please click on the tabs above.");
	QLabel *title3 = new QLabel("Note: We could also place instructions, or a link to a pdf <b>here</b>");
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(title);
	mainLayout->addWidget(title2);
	mainLayout->addWidget(title3);
	mainLayout->addStretch(1);
	setLayout(mainLayout);

}