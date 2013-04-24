#ifndef TITLEBAR_H
#define	TITLEBAR_H
#include <qwidget.h>
#include <qtoolbutton.h>
#include <qpixmap.h>
#include <qpoint.h>

class TitleBar : public QWidget
{
	Q_OBJECT
private:
	QToolButton * minimize;
	QToolButton * close;
	QPixmap restorePix, minPix;
	bool maxNormal;
	QPoint startPos;
	QPoint clickPos;
public:
	TitleBar(QWidget * parent);
protected:
	void mousePressEvent(QMouseEvent * me);
	void mouseMoveEvent(QMouseEvent * me);
public slots:
	void showSmall();
	void showMaxRestore();
};

#endif