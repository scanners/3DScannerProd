#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <Windows.h>
#include "Serial.h"
#include <qmessagebox.h>

class SerialCommunication : public QObject
{
	Q_OBJECT

public slots:
	int receiveStopSignalFromHardware();
signals:
	void sendErrorMessage(LONG, char *);
private:
	CSerial serial;
	LONG lLastError;
	bool isScanComplete;
public:
	SerialCommunication();
	int showError (LONG lError, char * errorMessage);
	void setScanDoneTrue();
	bool getIsScanComplete();
	int initializeSerialPort();
	int startStepperMotor();
};

#endif //SERIALCOMMUNICATION_H