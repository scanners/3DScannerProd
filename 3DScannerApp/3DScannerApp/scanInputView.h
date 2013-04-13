#ifndef SCANINPUTVIEW_H
#define SCANINPUTVIEW_H

#include <qwidget.h>
#include <string>

class ScanController;
class QLabel;
class QLineEdit;
class QGridLayout;
class QPushButton;

class ScanInputView : public QWidget
{
    Q_OBJECT

private slots:
	void startScan();
	void createSaveFileDialog();
	void createLoadFileDialog();
private:
	ScanController * scanController;
	QLineEdit * saveDirText;
	QLineEdit * loadDirText;
	QLabel * saveLabel;
	QLabel * loadLabel;
	QLabel * saveFileNameLabel;
	QLineEdit * saveFileNameText;
	QPushButton * startButton;
	QPushButton * exitButton;
	QPushButton * saveBrowseButton;
	QPushButton * loadBrowseButton;
	QGridLayout * mainLayout;
	void constructLayout();
	bool checkFileName(std::string filename);
public:
    explicit ScanInputView(QWidget *parent = 0);
	~ScanInputView();
	void setScanController(ScanController& scanControl);
};

#endif