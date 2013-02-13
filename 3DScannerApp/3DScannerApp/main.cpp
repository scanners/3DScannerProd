#include "homeview.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	HomeView w;
	w.show();
	return a.exec();
}
