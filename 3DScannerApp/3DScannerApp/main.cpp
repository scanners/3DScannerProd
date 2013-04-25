#include <QtGui/QApplication>
#include "homeView.h"
#include "MainFrame.h"
#include<qboxlayout.h>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    // instantatiate our main window here

	a.setStyleSheet("QPushButton {"
			"background-color: #333;"
			"color : white;"
			"border-style: outset;"
			"border-width: 2px;"
			"border-radius: 10px;"
			"border-color: white;"
			"font: bold 14px;"
			"min-width: 5em;"
			"padding: 4px;}"
			"QPushButton:hover {"
			"background-color: #FFF;"
			"border: 1px solid black;"
			"color: #000;}"
			"QLineEdit { background-color: #EEE;}"
			"QLineEdit:hover { background-color: #FFF;}"
			"QLineEdit:selected { background-color: #FFF;}"
			);

	MainFrame mf;
	QVBoxLayout * layout = new QVBoxLayout(mf.contentWidget());
	layout->setMargin(0);
	HomeView hv;
	//hv.show();
	layout->addWidget(&hv);
	mf.show();
    return a.exec();
}
