#ifndef CALIBRATIONMODEL_H
#define CALIBRATIONMODEL_H

//////////////////  CHANGE THESE VALUES /////////////////////////////////////
#define INTRINSIC_REQUIRED_NUM_SUCCESSES 10
#define EXTRINSIC_REQUIRED_NUM_SUCCESSES 2

using std::string;
using namespace::cv;

const float squareUnits = 1.0f;

class CalibrationModel {
private:
	Size innerCorners;
	Size imageSize;
	int corners;
    vector<vector<Point2f>> imagePoints;
	int successes;
	Mat intrinsicMatrix;
	Mat distortionCoefficients;
	Mat backRotationMatrix;
	Mat groundRotationMatrix;
	Mat backTranslationMatrix;
	Mat groundTranslationMatrix;
	string saveDirectory;
	string loadDirectory;

	//FOR DEMO ONLY
	Mat demoImage;
public:
	CalibrationModel();
	bool saveIntrinsicFiles();
	bool saveExtrinsicFiles();
	bool loadXML();
	int getRequiredNumSuccesses(int controllerType);
	int findCorners(Mat image);
	void calibrateIntrinsics();
	void calibrateExtrinsics(int boardLocation);
	void setNumCorners(int horizontal, int vertical);
	void resetSuccesses();
	void setSaveDirectory(string directory);
	void setLoadDirectory(string directory);
	
	//FOR DEMO ONLY
	void setImageForCornerDisplay(Mat image);
};

#endif // CALIBRATIONMODEL_H