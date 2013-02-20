#ifndef RESULTSVIEW_H
#define RESULTSVIEW_H

// NOTE: THIS CLASS AND OTHER CLASSES THAT HAVE POINTERS NEED DESTRUCTORS, ETC!
// OTHERWISE WE WILL RUN INTO MEMORY LEAK PROBLEMS

#include <qdialog.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qdialogbuttonbox.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qlineedit.h>
#include <qfiledialog.h>

class ResultsView : public QWidget
{
	Q_OBJECT

public:
	explicit ResultsView(QWidget *parent = 0);
private:
	QLabel * title;
	QLabel * directoryLabel;
	QWidget * pointCloudFrame;
	QPushButton * exitButton;
	QPushButton * browseButton;
	QBoxLayout * mainLayout;
	QBoxLayout * dirLayout;
	QLineEdit * outputDirTextField;
	QLabel * outputDirLabel;
	QLabel * pointCloudLabel;
	QString dir;
private slots:
	void createFileDialog();

};

#endif RESULTSVIEW_H