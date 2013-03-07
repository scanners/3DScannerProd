#include "overlayView.h"
#include "scanController.h"

OverlayView::OverlayView() {

}

void OverlayView::setScanController(ScanController& scanControl) {
	scanController = &scanControl;
}