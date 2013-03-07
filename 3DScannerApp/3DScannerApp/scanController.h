#ifndef SCANCONTROLLER_H
#define SCANCONTROLLER_H

#include <string>
using std::string;

class ScanModel;
class ScanningView;
class Image;

class ScanController {
private:
	ScanningView * scanningView;
	ScanModel * scanModel;
public:
	ScanController();
	void startScan();
	void setSaveDirectory(string saveDir);
	void setLoadDirectory(string loadDir);
	void createScanningView();
	void saveScan();
	void sendRegion(float);
	bool savePicture(Image * image);
	bool loadXML();
	void exit();
	void setScanModel(ScanModel& scanMod);
};

#endif //SCANCONTROLLER_H