#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include<Windows.h>
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<ctime>

using std::string;
using std::ios;
//����UserData����,�洢�û���������
class userud {
protected:
	string name;
	string password;
public:
	string getUserName();
	bool setUserName(string);
	bool verifyPasswd(string);
	bool changePasswd(string);
public:
	userud();
	userud(string, string);
	~userud();
};
//��չ��UserData����,�洢�����û�������Ϣ
class userudEx :public userud{
public:
	//���캯��
	userudEx();
	userudEx(string,string);
	//��������
	~userudEx();
protected:
	string sex;
public:
	string getSex();
	bool setSex(string sex);
protected:
	string birthday;
public:
	string getBirthday();
	bool setBirthday(string bth);
	int getAge();
protected:
	string occupation;
public:
	string getOccupation();
	bool setOccupation(string occ);
protected:
	string QQNumber;
	string WeiXinHao;
	string GithubID;
public:
	string getQQ();
	bool setQQ(string qq);
	string getWeiXin();
	bool setWeiXin(string wx);
	string getGithub();
	bool setGithub(string ghname);

};
//�ѵ�¼���û�,�洢������Ϣ
class LoginedUser :public userud {
public:
	void UploadLoginStatus();
protected:
	int logintime = 0;
	bool noForgotPwd = false;
public:
	LoginedUser();
	LoginedUser(string name);
	LoginedUser(string name,bool JiZhuPwd);
	~LoginedUser();
	bool setUsername(string);
	bool changePasswd(string);
};
//std::wstringת����std::string
std::string ws2s(std::wstring);
//std::stringת����std::wstring
std::wstring s2ws(std::string);
