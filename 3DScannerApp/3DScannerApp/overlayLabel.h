#ifndef OVERLAYLABEL_H
#define OVERLAYLABEL_H

#include "stdafx.h"
#include <qlabel.h>

class OverlayLabel : public QLabel
{
    Q_OBJECT
signals:
	void mousePressed();
private:
	// event for mouse clicks, will get the coords of the click
	void mousePressEvent(QMouseEvent * event);
	int x;
	int y;

	
public:
    OverlayLabel(QWidget *parent = 0);
	int getX();
	int getY();
};

#endif