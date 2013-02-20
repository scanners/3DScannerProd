#include <qwidget.h>
#include "homeView.h"
#include "inputView.h"
#include "scanningView.h"
#include "scanInputView.h"
#include "resultsView.h"
#include <qlabel.h>
#include <qboxlayout.h>
#include "enums.h"

HomeView::HomeView(QWidget *parent) : QDialog (parent)
{
	tabWidget = new QTabWidget;
	tabWidget->addTab(new TestTab(), "Home");
	tabWidget->addTab(new InputView(Enums::INTRINSIC), "Intrinsic Calibration");
	tabWidget->addTab(new InputView(Enums::EXTRINSIC), "Extrinsic Calibration");
	tabWidget->addTab(new ScanInputView(), "Scan Object");
	tabWidget->addTab(new ResultsView(), "Results/Export");

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(tabWidget);
	setLayout(mainLayout);

	setWindowTitle("3D Scanner");
	//this->setFixedSize(550, 350);
	this->setFixedSize(515, 515);
	//this->setBaseSize(550, 600);
	
}

TestTab::TestTab(QWidget *parent) : QWidget(parent)
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