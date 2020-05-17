#pragma once
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996)
//#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include "Ws2tcpip.h"
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <math.h>

typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef long long int64;
typedef int int32;
typedef short int16;
typedef char int8;
typedef int bool32;
typedef float float32;
typedef double float64;

constexpr uint16 	c_port = 8888;
constexpr uint32 	c_socket_buffer_size = 1024;
constexpr uint16	c_max_clients = 4;
constexpr uint32	c_ticks_per_second = 60;
constexpr float32	c_seconds_per_tick = 1.0f / (float32)c_ticks_per_second;
//constexpr float32   c_client_timeout = 100.0f;
