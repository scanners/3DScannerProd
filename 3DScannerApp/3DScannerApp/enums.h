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
		NUM_SUCCESSES,
		FAILURE,
		CALIBRATION_SUCCESS,
		SAVE_STATUS
	};
};

#endif //ENUMS_H