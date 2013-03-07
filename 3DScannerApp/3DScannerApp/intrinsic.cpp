#include "stdafx.h"

#include "intrinsic.h"

Intrinsic::Intrinsic(Mat intrinsicMat, Mat distortionCoeff) {
	intrinsicMatrix = intrinsicMat;
	distortionCoefficients = distortionCoeff;
}

Mat Intrinsic::getIntrinsicMatrix() {
	return intrinsicMatrix;
}

Mat Intrinsic::getDistortionCoefficients() {
	return distortionCoefficients;
}