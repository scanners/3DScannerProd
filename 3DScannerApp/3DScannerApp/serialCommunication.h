#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <Windows.h>
#include "Serial.h"
#include <qmessagebox.h>
#include <QMutex>

class SerialCommunication : public QObject
{
	Q_OBJECT

public slots:
	int showError (LONG lError, char * errorMessage);
	void receiveStopSignalFromHardware();
signals:
	void sendErrorMessage(LONG, char *);
private:
	CSerial serial;
	LONG lLastError;
	QMutex mutex;
	bool isScanComplete;
public:
	SerialCommunication();
	void setScanDoneTrue();
	bool getIsScanComplete();
	int initializeSerialPort();
	int startStepperMotor();
};

#endif //SERIALCOMMUNICATION_H