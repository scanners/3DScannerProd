#include <QtGui/QApplication>
#include "homeView.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    // instantatiate our main window here
	HomeView hv;
	hv.show();

    return a.exec();
}
