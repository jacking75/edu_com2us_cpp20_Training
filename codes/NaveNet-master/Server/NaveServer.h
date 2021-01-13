/** 
 *  @file  		NaveServer.h
 *  @brief 		���� �Լ� (�α׸޽��� �� ���� �����Լ�)
 *  @remarks 
 *  @author  	������(edith2580@gmail.com)
 *  @date  		2009-05-09
 */
#pragma once

#define WIN32_LEAN_AND_MEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

// Windows ��� �����Դϴ�.
#include <windows.h>
#include <time.h>												// timer 
#include <assert.h>												// assert
#include <process.h>											// Thread 
#include <stdio.h>												// standard I/O
#include <stdlib.h>
//#include <wchar_t.h>

// sock
#include<winsock2.h>											// win32 socket 
#pragma comment(lib,"ws2_32.lib")								

#include<Mswsock.h>												// extension socket library 
#pragma comment(lib,"mswsock.lib")

#include <Ws2tcpip.h>

//TODO: ��������  //////////
// ��Ƽ�̵�� Ÿ�̸Ӹ� ����
#include "mmsystem.h"											
#pragma comment(lib,"winmm.lib")
///////////////////////////

#include <string>

#include "NFPacket.h"
#include "NFDefine.h"

/**
 * @brief	���� �����Ǹ� �����ɴϴ�.
 * @param LocalIP	���þ����� ���庯��.
 * @param bClosedIP ���ξ����Ǹ� ���ö�
 * @return ��������
 */
bool GetLocalIP(std::string& localIP_Out, bool bClosedIP = false);
