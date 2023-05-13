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

class SerialPort {
public:
    SerialPort(FString portName, int baudRate, int dataBits, int parity, int stopBits);

    ~SerialPort();

    void Write(unsigned char* buffer, int offset, int count);
    FString ReadLine();

private:
    std::string _portName;
    int _baudRate;
    int _dataBits;
    int _parity;
    int _stopBits;
    HANDLE _serialPort;
};
