#include<iostream>
#include<string>
#include<sstream>
#include<stdio.h>
#include<cstdio>
#include<conio.h>
#include<stdlib.h>

//Windows����
#include<Windows.h>
//

#include "fn.h"

#define cls system("cls")

bool movePosition(int y/*X����*/, int x/*Y����*/) //�ƶ�����̨���(��Windows)
{
	HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position;    // position ��һ�� COORD �ṹ
	position.X = x; //��������x����
	position.Y = y; //��������y����
	//�����������Ȼ��ʼ��ӡ
	SetConsoleCursorPosition(screen, position);
	return false;
}

void startupExe(const std::wstring& paras)
{
    
}
