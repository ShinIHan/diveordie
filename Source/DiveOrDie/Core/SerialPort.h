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

        _serialPort = CreateFileA(_portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (_serialPort == INVALID_HANDLE_VALUE) {
            UE_LOG(LogTemp, Error, TEXT("Unable to open port %s"), *portName);
            return;
        }

        DCB dcbSerialParams = { 0 };
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (!GetCommState(_serialPort, &dcbSerialParams)) {
            CloseHandle(_serialPort);
            UE_LOG(LogTemp, Error, TEXT("Unable to get serial port parameters"));
            return;
        }

        dcbSerialParams.BaudRate = _baudRate;
        dcbSerialParams.ByteSize = _dataBits;
        dcbSerialParams.Parity = _parity;
        dcbSerialParams.StopBits = _stopBits;

        if (!SetCommState(_serialPort, &dcbSerialParams)) {
            DWORD lastError = GetLastError();
            CloseHandle(_serialPort);
            UE_LOG(LogTemp, Error, TEXT("Unable to set serial port parameters, error code: %d"), lastError);
            return;
        }

        COMMTIMEOUTS timeouts = { 0 };
        timeouts.ReadIntervalTimeout = 100;
        timeouts.ReadTotalTimeoutConstant = 100;
        timeouts.ReadTotalTimeoutMultiplier = 10;
        timeouts.WriteTotalTimeoutConstant = 0;
        timeouts.WriteTotalTimeoutMultiplier = 0;

        if (!SetCommTimeouts(_serialPort, &timeouts)) {
            CloseHandle(_serialPort);
            UE_LOG(LogTemp, Error, TEXT("Unable to set serial port timeouts"));
            return;
        }
    }

    ~SerialPort() {
        if (_serialPort != INVALID_HANDLE_VALUE) {
            CloseHandle(_serialPort);
        }
    }

    bool IsOpen() const {
        return _serialPort != INVALID_HANDLE_VALUE;
    }

    void Write(unsigned char* buffer, int offset, int number) {
        DWORD bytesWritten;
        if (!WriteFile(_serialPort, &buffer[offset], number, &bytesWritten, NULL)) {
            DWORD lastError = GetLastError();
            UE_LOG(LogTemp, Error, TEXT("Error occurred while writing to port %s, error code: %d"), *_portName.c_str(), lastError);
            return;
        }
    }

    FString ReadLine() {
        FString result;
        char c;
        DWORD bytesRead;

        do {
            if (!ReadFile(_serialPort, &c, 1, &bytesRead, NULL)) {
                DWORD lastError = GetLastError();
                UE_LOG(LogTemp, Error, TEXT("Error occurred while reading from port %s, error code: %d"), *_portName.c_str(), lastError);
                break;
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