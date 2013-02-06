#ifndef EXTRINSIC_H
#define EXTRINSIC_H

using cv::Mat;

class Extrinsic {
private:
	Mat backRot;
	Mat backTrans;
	Mat groundRot;
	Mat groundTrans;
public:
	Extrinsic(Mat, Mat, Mat, Mat);
	Mat getBackRotationMatrix();
	Mat getBackTranslationMatrix();
	Mat getGroundRotationMatrix();
	Mat getGroundTranslationMatrix();
};

#endif //EXTRINSIC_H