#ifndef ENUMS_H
#define ENUMS_H

class Enums {
public:
	enum controllerEnum {
		INTRINSIC,
		EXTRINSIC
	};

	enum calibrationEnum {
		TAKE_PIC,
		CORNERS_SUCCESS,
		CORNERS_FAILURE,
		CALIBRATION_SUCCESS,
		SAVE_STATUS
	};

	enum extrinsicBoardLocation {
		GROUND_PLANE,
		BACK_PLANE
	};
};

#endif //ENUMS_H