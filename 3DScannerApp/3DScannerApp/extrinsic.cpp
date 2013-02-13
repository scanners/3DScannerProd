#include "stdafx.h"

#include "extrinsic.h"

Extrinsic::Extrinsic(Mat backRot, Mat backTrans, Mat groundRot, Mat groundTrans) {

}

Mat Extrinsic::getBackRotationMatrix() {
	return backRot;
}

Mat Extrinsic::getBackTranslationMatrix() {
	return backTrans;
}

Mat Extrinsic::getGroundRotationMatrix() {
	return groundRot;
}

Mat Extrinsic::getGroundTranslationMatrix() {
	return groundTrans;
}