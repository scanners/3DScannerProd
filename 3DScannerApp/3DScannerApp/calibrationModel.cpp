#include "stdafx.h"

#include "calibrationModel.h"

CalibrationModel::CalibrationModel(): successes(0) {

}

void CalibrationModel::saveIntrinsicFiles() {
	FileStorage fs(saveDirectory + "\\intrinsics.xml", FileStorage::WRITE);
	fs << "intrinsicMatrix" << intrinsicMatrix << "distortionCoefficients" << distortionCoefficients;
	fs.release();
}

void CalibrationModel::saveExtrinsicFiles() {
	FileStorage fs(saveDirectory + "\\extrinsics.xml", FileStorage::WRITE);
	fs << "backRotationVector" << backRotationVector << "backTranslationVector" << backTranslationVector;
	fs << "groundRotationVector" << groundRotationVector << "groundTranslationVector" << groundTranslationVector;
	fs.release();
}

bool CalibrationModel::loadXML() {
	FileStorage fs(loadDirectory + "\\intrinsics.xml", FileStorage::READ);
	fs["intrinsicMatrix"] >> intrinsicMatrix;
	fs["distortionCoefficients"] >> distortionCoefficients;
	fs.release();

	return false;
}

int CalibrationModel::getRequiredNumSuccesses(int controllerType) {
	switch (controllerType) {
	case Enums::controllerEnum::INTRINSIC:
		return INTRINSIC_REQUIRED_NUM_SUCCESSES;
	case Enums::controllerEnum::EXTRINSIC:
		return EXTRINSIC_REQUIRED_NUM_SUCCESSES;
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
	vector<vector<Point3f>> objectPoints(1);
	intrinsicMatrix = Mat::eye(3, 3, CV_64F);
	distortionCoefficients = Mat::zeros(8, 1, CV_64F);
	for( int i = 0; i < innerCorners.height; i++ ) {
            for( int j = 0; j < innerCorners.width; j++ ) {
                objectPoints[0].push_back(Point3f(float(j*squareUnits),
                                          float(i*squareUnits), 0));
			}
	}
	objectPoints.resize(imagePoints.size(), objectPoints[0]);
	double error = calibrateCamera(objectPoints, imagePoints, imageSize, intrinsicMatrix,
		distortionCoefficients, rotationVectors, translationVectors);
	saveIntrinsicFiles();
}

void CalibrationModel::calibrateExtrinsics(int boardLocation) {
	vector<vector<Point3f>> objectPoints(1);
	for( int i = 0; i < innerCorners.height; i++ ) {
            for( int j = 0; j < innerCorners.width; j++ ) {
                objectPoints[0].push_back(Point3f(float(j*squareUnits),
                                          float(i*squareUnits), 0));
			}
	}
	objectPoints.resize(imagePoints.size(), objectPoints[0]);

	Mat rotationVector;
	Mat translationVector;

	solvePnP(objectPoints[0], imagePoints[0], intrinsicMatrix, distortionCoefficients, rotationVector, translationVector);
	if (boardLocation == Enums::extrinsicBoardLocation::BACK_PLANE) {
		backRotationVector = rotationVector;
		backTranslationVector = translationVector;
	} else if (boardLocation == Enums::extrinsicBoardLocation::GROUND_PLANE) {
		groundRotationVector = rotationVector;
		groundTranslationVector = translationVector;
	}
	//Reset for next extrinsic calibration
	imagePoints.clear();
}

void CalibrationModel::setNumCorners(int horizontal, int vertical) {
	innerCorners = Size((horizontal - 1), (vertical - 1));
}

void CalibrationModel::setSaveDirectory(string directory) {
	saveDirectory = directory;
}

void CalibrationModel::setLoadDirectory(string directory) {
	loadDirectory = directory;
}