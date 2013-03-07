#ifndef OVERLAYVIEW_H
#define OVERLAYVIEW_H

class ScanController;

class OverlayView {
private:
	ScanController * scanController;
public:
	OverlayView();
	void setScanController(ScanController& scanController);
};

#endif //OVERLAYVIEW_H