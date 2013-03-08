#include "stdafx.h"
#include <exception>

#include "calibrationModel.h"

CalibrationModel::CalibrationModel() {

}

bool CalibrationModel::saveIntrinsicFiles() {
	FileStorage * fs = new FileStorage();
	bool directoryOk = fs->open(saveDirectory + "\\intrinsics.xml", FileStorage::WRITE);
	if (!directoryOk) {
		return false;
	}
	*fs << "intrinsicMatrix" << intrinsicMatrix << "distortionCoefficients" << distortionCoefficients;
	fs->release();
	return true;
}

bool CalibrationModel::saveExtrinsicFiles() {
	FileStorage * fs = new FileStorage();
	bool directoryOk = fs->open(saveDirectory + "\\extrinsics.xml", FileStorage::WRITE);
	if (!directoryOk) {
		//Message to run as admin or choose different directory
		return false;
	}
	*fs << "backRotationMatrix" << backRotationMatrix << "backTranslationMatrix" << backTranslationMatrix;
	*fs << "groundRotationMatrix" << groundRotationMatrix << "groundTranslationMatrix" << groundTranslationMatrix;
	fs->release();
	return true;
}

bool CalibrationModel::loadXML() {
	try {
		FileStorage * fs = new FileStorage();
		bool loadOk = fs->open(loadDirectory + "\\intrinsics.xml", FileStorage::READ);
		if(!loadOk) {
			return false;
		}
		(*fs)["intrinsicMatrix"] >> intrinsicMatrix;
		(*fs)["distortionCoefficients"] >> distortionCoefficients;
		fs->release();
		return true;
	} catch (cv::Exception& e) {
		return false;
	} catch (std::exception& e) {
		return false;
	}
}

int CalibrationModel::getRequiredNumSuccesses(int controllerType) {
	switch (controllerType) {
	case Enums::controllerEnum::INTRINSIC:
		return INTRINSIC_REQUIRED_NUM_SUCCESSES;
	case Enums::controllerEnum::EXTRINSIC:
		return EXTRINSIC_REQUIRED_NUM_SUCCESSES;
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

	Mat rotationMatrix3x1;
	Mat rotationMatrix;
	Mat translationMatrix;

	solvePnP(objectPoints[0], imagePoints[0], intrinsicMatrix, distortionCoefficients, rotationMatrix3x1, translationMatrix);
	Rodrigues(rotationMatrix3x1, rotationMatrix);
	if (boardLocation == Enums::extrinsicBoardLocation::BACK_PLANE) {
		backRotationMatrix = rotationMatrix;
		backTranslationMatrix = translationMatrix;
	} else if (boardLocation == Enums::extrinsicBoardLocation::GROUND_PLANE) {
		groundRotationMatrix = rotationMatrix;
		groundTranslationMatrix = translationMatrix;
	}
	//Reset for next extrinsic calibration
	imagePoints.clear();
}

void CalibrationModel::setNumCorners(int horizontal, int vertical) {
	innerCorners = Size((horizontal - 1), (vertical - 1));
}

void CalibrationModel::resetSuccesses() {
	successes = 0;
}

void CalibrationModel::setSaveDirectory(string directory) {
	saveDirectory = directory;
}

void CalibrationModel::setLoadDirectory(string directory) {
	loadDirectory = directory;
}