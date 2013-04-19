#include "mainframe.h"
#include <qboxlayout.h>
#include "Titlebar.h"

MainFrame::MainFrame()
{
	mouseDown = false;
	setFrameShape(Panel);
	// make this a borderless window, which cannot be resized by the window system
	setWindowFlags(Qt::FramelessWindowHint);
	setMouseTracking(true);

	mainTitleBar = new TitleBar(this);
	mainContent = new QWidget(this);

	QVBoxLayout * vbox = new QVBoxLayout(this);
	vbox->addWidget(mainTitleBar);
	vbox->setMargin(0);
	vbox->setSpacing(0);

	QVBoxLayout * layout = new QVBoxLayout(this);
	layout->addWidget(mainContent);
	layout->setMargin(5);
	layout->setSpacing(0);
	vbox->addLayout(layout);
	 
}