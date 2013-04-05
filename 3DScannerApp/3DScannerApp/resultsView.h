#ifndef RESULTSVIEW_H
#define RESULTSVIEW_H

// NOTE: THIS CLASS AND OTHER CLASSES THAT HAVE POINTERS NEED DESTRUCTORS, ETC!
// OTHERWISE WE WILL RUN INTO MEMORY LEAK PROBLEMS

#include <qwidget.h>
class QLabel;
class QPushButton;
class QBoxLayout;
class QLineEdit;
class QString;

class ResultsView : public QWidget
{
	Q_OBJECT

public:
	explicit ResultsView(QWidget *parent = 0);
	~ResultsView();
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
	void constructLayout();
private slots:
	void createFileDialog();

};

#endif RESULTSVIEW_H