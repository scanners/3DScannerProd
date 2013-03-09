#include "overlayLabel.h"
#include "qevent.h"


OverlayLabel::OverlayLabel(QWidget *parent) : QLabel(parent)
{
	
}


void OverlayLabel::mousePressEvent(QMouseEvent * event)
{
	this->x = event->x();
	this->y = event->y();
	emit this->mousePressed();
}

int OverlayLabel::getX()
{
	return this->x;
}

int OverlayLabel::getY()
{
	return this->y;
}