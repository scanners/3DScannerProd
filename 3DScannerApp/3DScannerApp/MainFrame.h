#include <qframe.h>
#include "Titlebar.h"
#include <qpoint.h>
#include <qwidget.h>

class MainFrame : public QFrame
{
public:
	MainFrame();
	QWidget * contentWidget() const { return mainContent; };
	TitleBar * titleBar() const { return mainTitleBar; };
private:
	TitleBar * mainTitleBar;
	QWidget * mainContent;
	QPoint oldPos;
	bool mouseDown;
	bool left, right, bottom;

};