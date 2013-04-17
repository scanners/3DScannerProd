#include <QtGui/QApplication>
#include "homeView.h"
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
			"background-color: #666;}"
			"QLineEdit { background-color: #EEE;}"
			"QLineEdit:hover { background-color: #FFF;}"
			"QLineEdit:selected { background-color: #FFF;}"
			);


	HomeView hv;
	hv.show();

    return a.exec();
}
