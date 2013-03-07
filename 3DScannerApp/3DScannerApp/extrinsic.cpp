#include "stdafx.h"

#include "extrinsic.h"

Extrinsic::Extrinsic(Mat rotation, Mat translation) {
	rotationMatrix = rotation;
	translationMatrix = translation;
}

Mat Extrinsic::getRotationMatrix() {
	return rotationMatrix;
}

Mat Extrinsic::getTranslationMatrix() {
	return translationMatrix;
}