#include <qwidget.h>
#include "homeView.h"
#include "inputView.h"
#include "scanningView.h"
#include "scanInputView.h"
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
	tabWidget->addTab(new TestTab(), "Results/Export");

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(tabWidget);
	setLayout(mainLayout);

	setWindowTitle("This is a test");
	
}

TestTab::TestTab(QWidget *parent) : QWidget(parent)
{
	QLabel *title = new QLabel("Testing Title");
	QLabel *title2 = new QLabel("Testing Title2");
	QLabel *title3 = new QLabel("Testing Title3");
	QLabel *title4 = new QLabel("Testing Title4");
	QLabel *title5 = new QLabel("Testing Title5");
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(title);
	mainLayout->addWidget(title2);
	mainLayout->addWidget(title3);
	mainLayout->addWidget(title4);
	mainLayout->addWidget(title5);
	mainLayout->addStretch(1);
	setLayout(mainLayout);

}