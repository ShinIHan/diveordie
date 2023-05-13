// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/Core/SerialPort.h"

SerialPort::SerialPort(FString portName, int baudRate, int dataBits, int parity, int stopBits)
{
    // COM 포트를 열기 위한 설정
    _portName = std::string(TCHAR_TO_UTF8(*portName));
    _baudRate = baudRate;
    _dataBits = dataBits;
    _parity = parity;
    _stopBits = stopBits;


    // 포트 초기화
    _serialPort = CreateFileA(_portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (_serialPort == INVALID_HANDLE_VALUE) {
        throw std::runtime_error(TCHAR_TO_UTF8(*FString("Unable to open port " + portName)));
    }

    // DCB 구조체를 설정하여 시리얼 통신 속성 설정
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

    // 타임아웃 설정
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 5;
    timeouts.ReadTotalTimeoutConstant = 5;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.WriteTotalTimeoutConstant = 5;
    timeouts.WriteTotalTimeoutMultiplier = 1;

    if (!SetCommTimeouts(_serialPort, &timeouts)) {
        CloseHandle(_serialPort);
        throw std::runtime_error(TCHAR_TO_UTF8(*FString("Unable to set serial port timeouts")));
    }

}

SerialPort::~SerialPort()
{
    if (_serialPort != INVALID_HANDLE_VALUE) {
        CloseHandle(_serialPort);
    }
}

void SerialPort::Write(unsigned char* buffer, int offset, int count)
{
    DWORD bytesWritten;
    if (!WriteFile(_serialPort, &buffer[offset], count, &bytesWritten, NULL)) {
        throw TCHAR_TO_UTF8(*FString("Error occurred while writing to port " + FString(_portName.c_str())));
    }
}

FString SerialPort::ReadLine()
{
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



