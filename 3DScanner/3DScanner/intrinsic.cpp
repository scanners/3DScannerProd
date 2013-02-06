#include "stdafx.h"

#include "intrinsic.h"

Intrinsic::Intrinsic(Mat intrinsicMat, Mat distortionMat) {
	intrinsic = intrinsicMat;
	distortion = distortionMat;
}

Mat Intrinsic::getIntrinsic() {
	return intrinsic;
}

Mat Intrinsic::getDistortion() {
	return distortion;
}