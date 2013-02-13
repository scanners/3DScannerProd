#ifndef HOMEVIEW_H
#define HOMEVIEW_H

#include <QtGui/QMainWindow>
#include "ui_homeview.h"

class HomeView : public QMainWindow
{
	Q_OBJECT

public:
	HomeView(QWidget *parent = 0, Qt::WFlags flags = 0);
	~HomeView();

private:
	Ui::HomeViewClass ui;
};

#endif // HOMEVIEW_H
