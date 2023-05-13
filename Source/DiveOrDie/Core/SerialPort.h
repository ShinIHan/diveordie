// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <iostream>
#include <string>
#include <stdbool.h>
#include <sstream>
#include <chrono>
#include <thread>
#include <vector>
#include <array>
#include <cmath>
#include <iomanip>
#include <stdexcept>
#include <Windows.h>

class GAME_API SerialPort {
public:
	SerialPort(FString portName, int baudRate, int dataBits, int parity, int stopBits) 
	{
	    _portName = std::string(TCHAR_TO_UTF8(*portName));
	    _baudRate = baudRate;
	    _dataBits = dataBits;
	    _parity = parity;
	    _stopBits = stopBits;
	
	    try
	    {
	        _serialPort = CreateFileA(_portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	        if (_serialPort == INVALID_HANDLE_VALUE) {
	            throw std::runtime_error(TCHAR_TO_UTF8(*FString("Unable to open port " + portName)));
	        }
	
	        DCB dcbSerialParams = { 0 };
	        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	        if (!GetCommState(_serialPort, &dcbSerialParams)) {
	            CloseHandle(_serialPort);
	            throw std::runtime_error(TCHAR_TO_UTF8(*FString("Unable to get serial port parameters")));
	        }
	
	        dcbSerialParams.BaudRate = _baudRate;
	        dcbSerialParams.ByteSize = _dataBits;
	        dcbSerialParams.Parity = _parity;
	        dcbSerialParams.StopBits = _stopBits;
	
	        if (!SetCommState(_serialPort, &dcbSerialParams)) {
	            DWORD lastError = GetLastError();
	            CloseHandle(_serialPort);
	            throw std::system_error(lastError, std::system_category(), TCHAR_TO_UTF8(*FString("Unable to set serial port parameters")));
	        }

	        COMMTIMEOUTS timeouts = { 0 };
	        timeouts.ReadIntervalTimeout = 100;
	        timeouts.ReadTotalTimeoutConstant = 100;
	        timeouts.ReadTotalTimeoutMultiplier = 10;
	        timeouts.WriteTotalTimeoutConstant = 0;
	        timeouts.WriteTotalTimeoutMultiplier = 0;
	
	        if (!SetCommTimeouts(_serialPort, &timeouts)) {
	            CloseHandle(_serialPort);
	            throw std::runtime_error(TCHAR_TO_UTF8(*FString("Unable to set serial port timeouts")));
	        }
	    }
		catch (const std::exception& e)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed, %s"), e.what());
		}
	}
	
	~SerialPort() {
	    if (_serialPort != INVALID_HANDLE_VALUE) {
	        CloseHandle(_serialPort);
	    }
	}
	
	void Write(unsigned char* buffer, int offset, int count) {
	    DWORD bytesWritten;
	    if (!WriteFile(_serialPort, &buffer[offset], count, &bytesWritten, NULL)) {
	        throw TCHAR_TO_UTF8(*FString("Error occurred while writing to port " + FString(_portName.c_str())));
	    }
	}
	
	FString ReadLine() {
	    FString result;
	    char c;
	    DWORD bytesRead;
	        
	    do {
	        if (!ReadFile(_serialPort, &c, 1, &bytesRead, NULL)) {
	            throw std::runtime_error(TCHAR_TO_UTF8(*FString("Error occurred while reading from port " + FString(_portName.c_str()))));
	        }
	        if (bytesRead == 0) {
	            break;
	        }
	        result += c;
	        UE_LOG(LogTemp, Log, TEXT("Received data: %c"), c);
	    } while (c != '\n');
	
	    return result;
	}
	
private:
	std::string _portName;
	int _baudRate;
	int _dataBits;
	int _parity;
	int _stopBits;
	HANDLE _serialPort;
};