#include "serialCommunication.h"

SerialCommunication::SerialCommunication(){
	this->initializeSerialPort();
	isScanComplete = false;
}

bool SerialCommunication::getIsScanComplete(){
	return isScanComplete;
}

int SerialCommunication::showError (LONG lError, char * errorMessage)
{
	QMessageBox messageBox;
	QString message;
	message.append(errorMessage);
	message.append("\n(error code ");
	message.append(lError+")");
	
	messageBox.setIcon(QMessageBox::Information);
	messageBox.setWindowTitle("Serial Communications Error");
	messageBox.setText(message);

	return 1;
}

int SerialCommunication::initializeSerialPort() {

	lLastError = ERROR_SUCCESS;

	// Attempt to open the serial port (COM1)
	lLastError = serial.Open("COM1",0,0,true);
	if (lLastError != ERROR_SUCCESS)
		return showError(serial.GetLastError(), "Unable to open COM-port");
	
	//Setup the Serial Port
	lLastError = serial.Setup(CSerial::EBaud9600,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS)
		return showError(serial.GetLastError(), "Unable to set COM-port setting");

	// Setup handshaking
    lLastError = serial.SetupHandshaking(CSerial::EHandshakeSoftware);
	if (lLastError != ERROR_SUCCESS)
		return showError(serial.GetLastError(), "Unable to set COM-port handshaking");

	// Register only for the receive event
    lLastError = serial.SetMask(CSerial::EEventError |
								CSerial::EEventRecv);
	if (lLastError != ERROR_SUCCESS)
		return showError(serial.GetLastError(), "Unable to set COM-port event mask");

	// Use 'non-blocking' reads, because we don't know how many bytes
	// will be received. This is normally the most convenient mode
	// (and also the default mode for reading data).
    lLastError = serial.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
	if (lLastError != ERROR_SUCCESS)
		return showError(serial.GetLastError(), "Unable to set COM-port read timeout.");
	
	return 0;
}

int SerialCommunication::startStepperMotor(){
	lLastError = serial.Write("stop");
	if (lLastError != ERROR_SUCCESS)
		return showError(serial.GetLastError(), "Unable to send data");

	return 0;
}

void SerialCommunication::receiveStopSignalFromHardware() {

	char * complete = "stop";

	bool fContinue = true;
	do
	{
		// Wait for an event
		lLastError = serial.WaitEvent();
		if (lLastError != ERROR_SUCCESS)
			emit sendErrorMessage(serial.GetLastError(), "Unable to wait for a COM-port event.");

		// Save event
		const CSerial::EEvent eEvent = serial.GetEventType();

		// Handle break event
		if (eEvent & CSerial::EEventBreak)
		{
			printf("\n### BREAK received ###\n");
		}

		// Handle error event
		if (eEvent & CSerial::EEventError)
		{
			QString message;
			message.append("Error occured while scanning.\nError Type: ");

			switch (serial.GetError())
			{
				case CSerial::EErrorBreak:		message.append("Break condition");			break;
				case CSerial::EErrorFrame:		message.append("Framing error");			break;
				case CSerial::EErrorIOE:		message.append("IO device error");			break;
				case CSerial::EErrorMode:		message.append("Unsupported mode");			break;
				case CSerial::EErrorOverrun:	message.append("Buffer overrun");			break;
				case CSerial::EErrorRxOver:		message.append("Input buffer overflow");	break;
				case CSerial::EErrorParity:		message.append("Input parity error");		break;
				case CSerial::EErrorTxFull:		message.append("Output buffer full");		break;
				default:						message.append("Unknown Error");			break;
			}

			
			emit sendErrorMessage(serial.GetLastError(), message.toLocal8Bit().data());
		}

		// Handle data receive event
		if (eEvent & CSerial::EEventRecv)
		{
			// Read data, until there is nothing left
			DWORD dwBytesRead = 0;
			char szBuffer[10];
			do
			{
				// Read data from the COM-port
				lLastError = serial.Read(szBuffer,sizeof(szBuffer)-1,&dwBytesRead);
				if (lLastError != ERROR_SUCCESS)
					emit sendErrorMessage(serial.GetLastError(), "Unable to read from COM-port.");

				if (dwBytesRead > 0)
				{
					// Finalize the data, so it is a valid string
					szBuffer[dwBytesRead] = '\0';

					// Check for Stop Text
					//if(strstr(szBuffer, complete) != NULL)
					fContinue = false;
					
					mutex.lock();
					isScanComplete = true;
					mutex.unlock();
				}
			}
		    while (dwBytesRead == sizeof(szBuffer)-1);
		}
	}
	while (fContinue);

    // Close the port again
    serial.Close();
}