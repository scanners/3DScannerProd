#include "stdafx.h"

#include "calibrationModel.h"

CalibrationModel::CalibrationModel(): successes(0) {
	
}

void CalibrationModel::saveFiles(string directory) {
	//CHECK FOR CONTROLLER TYPE AND ONLY SAVE RELEVANT FILE, i.e. intrinsics or extrinsics
	FileStorage fs(directory + "intrinsics.xml", FileStorage::WRITE);
	fs << "intrinsicMatrix" << intrinsicMatrix << "distortionCoefficients" << distortionCoefficients;
	fs.release();
}

bool CalibrationModel::loadXML(string directory) {
	FileStorage fs(directory + "intrinsics.xml", FileStorage::READ);
	fs["intrinsicMatrix"] >> intrinsicMatrix;
	fs["distortionCoefficients"] >> distortionCoefficients;
	fs.release();

	return false;
}

int CalibrationModel::getMaxNumSuccesses(int controllerType) {
	switch (controllerType) {
	case Enums::controllerEnum::INTRINSIC:
		return INTRINSIC_MAX_NUM_SUCCESSES;
	case Enums::controllerEnum::EXTRINSIC:
		return EXTRINSIC_MAX_NUM_SUCCESSES;
	default:
		return -1;
	}
}

int CalibrationModel::findCorners(Mat image) {
	Mat grayImage;
	bool foundCorrectNumCorners;
	vector<Point2f> pointBuffer;
	imageSize = image.size();
	foundCorrectNumCorners = findChessboardCorners(image, innerCorners, pointBuffer, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
	if (foundCorrectNumCorners) {
		successes++;
		cvtColor(image, grayImage, CV_BGR2GRAY);
		cornerSubPix(grayImage, pointBuffer, Size(11,11), Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
		imagePoints.push_back(pointBuffer);
		return successes;
	} else {
		return -1;
	}

}

void CalibrationModel::calibrateIntrinsics() {
	intrinsicMatrix = Mat::eye(3, 3, CV_64F);
	distortionCoefficients = Mat::zeros(8, 1, CV_64F);

	objectPoints.resize(imagePoints.size());
	calibrateCamera(objectPoints, imagePoints, imageSize, intrinsicMatrix,
		distortionCoefficients, rotationVectors, translationVectors);
}

void CalibrationModel::calibrateExtrinsics(int boardLocation) {
	CvMat objectPointsC = Mat(objectPoints, true);
	CvMat imagePointsC = Mat(imagePoints, true);
	CvMat intrinsicMatrixC = intrinsicMatrix;
	CvMat distortionCoefficientsC = distortionCoefficients;
	//--------------MAY NEED TO PUT SOME DATA IN THESE (e.g. aspect ratio)
	CvMat * rotationVectorC = cvCreateMat(3, 3, CV_32FC1);
	CvMat * translationVectorC = cvCreateMat(4, 1, CV_32FC1);
	cvFindExtrinsicCameraParams2(&objectPointsC, &imagePointsC, &intrinsicMatrixC, 
		&distortionCoefficientsC, rotationVectorC, translationVectorC);
	if (boardLocation == Enums::extrinsicBoardLocation::BACK_PLANE) {
		backRotationVector = Mat(rotationVectorC, true);
		backTranslationVector = Mat(translationVectorC, true);
	} else if (boardLocation == Enums::extrinsicBoardLocation::GROUND_PLANE) {
		groundRotationVector = Mat(rotationVectorC, true);
		groundTranslationVector = Mat(translationVectorC, true);
	}
	
}

void CalibrationModel::setNumCorners(int horizontal, int vertical) {
	innerCorners = Size((horizontal - 1), (vertical - 1));
}