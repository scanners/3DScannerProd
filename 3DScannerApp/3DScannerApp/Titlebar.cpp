#include "Titlebar.h"
#include <qpixmap.h>
#include <qstyle.h>
#include <qlabel.h>
#include <qboxlayout.h>
#include <qevent.h>

TitleBar::TitleBar(QWidget *parent)
{
	// do not use the parent's background color
	//setAutoFillBackground(true);

	setBackgroundRole(QPalette::Window);
	//this->setStyleSheet("background-color: #000;");

	minimize = new QToolButton(this);
	maximize = new QToolButton(this);
	close = new QToolButton(this);

	// use the style to determine the pixmap of the buttons
	QPixmap pix = this->style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
	close->setIcon(pix);

	maxPix = this->style()->standardPixmap(QStyle::SP_TitleBarMaxButton);
	maximize->setIcon(maxPix);

	minPix = this->style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	minimize->setIcon(minPix);

	minimize->setMinimumHeight(20);
	close->setMinimumHeight(20);
	maximize->setMaximumHeight(20);

	QLabel * label = new QLabel(this);
	label->setText("<b>3D Scanner</b>");
	label->setStyleSheet("background-color: transparent; color: #000;");
	parent->setWindowTitle("3D Scanner");

	QHBoxLayout * hbox = new QHBoxLayout(this);

	hbox->addWidget(label);
	hbox->addWidget(minimize);
	hbox->addWidget(maximize);
	hbox->addWidget(close);

	// not entirely sure how this works, we will have to wait and see
	hbox->insertStretch(1, 500);
	hbox->setSpacing(0);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	maxNormal = false;

	connect(close, SIGNAL(clicked()), parent, SLOT(close()));
	connect(minimize, SIGNAL(clicked()), this, SLOT(showSmall()));
	connect(maximize, SIGNAL(clicked()), this, SLOT(showMaxRestore()));
}

void TitleBar::showSmall()
{
	parentWidget()->showMinimized();
}

void TitleBar::showMaxRestore()
{
	// currently not implemented, as we might not use this
}

void TitleBar::mousePressEvent(QMouseEvent * me)
{
	startPos = me->globalPos();
	clickPos = mapToParent(me->pos());
}

void TitleBar::mouseMoveEvent(QMouseEvent * me)
{
	if (maxNormal)
		return;
	parentWidget()->move(me->globalPos() - clickPos);
}