#include<iostream>
#include<string>
#include<sstream>
#include<stdio.h>
#include<cstdio>
#include<conio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fstream>

//Windows����
#include<Windows.h>
//Vector�䳤����
#include<vector>

#include<future>

#include "fn.h"

using namespace std;

#define N 16383
#define getche _getche
#define getch _getch

#define wout oss_last

void hideWindow();
bool file_exists2(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}
bool SetConsoleColor(WORD color)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    return SetConsoleTextAttribute(handle, color); // BACKGROUND_BLUE
}
unsigned int noactcnt = (short)(unsigned)0;
void NoActiveFn() {
        SetConsoleColor((WORD)04);
            MessageBox(FindWindow("ConsoleWindowClass", NULL), "�뾡�켤��!", "��ܰ��ʾ", MB_ICONINFORMATION);
}

#include"timer.h"
int main(int argc,char* argv[]) {
    if (argc>1&&argv[1][0]=='-') {
        if (argv[1][1] == '-') {
            if (argv[1][2] == 's'&&argv[1][3] == 'h'&&argv[1][4] == 'o'&&argv[1][5] == 'w') {
                system("echo true > maf_ma_t_sw_show_test");
                return 0;
            }
        }
    }
    HWND hwnd;
    hwnd = FindWindow("ConsoleWindowClass", NULL);	//���������ڵ������ʹ�������ƥ��ָ�����ַ���,�������Ӵ��ڡ�
    if (hwnd) ShowWindow(hwnd, SW_SHOW);
    char c;
    int MALLOCSLENGTH = 64;//(argv[1] == "/length" || argv[1] == "--length") ? atoi(argv[2]) : 64;
    vector<int*> mallocs(N);
    ostringstream oss,oss_last;
    int icid;
    mallocs[0] = NULL;
    bool again = true;
    int i = 0,mcon;
    string aa,mid;

    string actkey;
    if (file_exists2("act.ok.key")) {
        if (system("act")==3) {
            MessageBox(FindWindow("ConsoleWindowClass", NULL), "���򱻷Ƿ��ƽ�!", "����", MB_ICONERROR);
            exit(3);
        }
        fstream actfp;
        actfp.open("act.ok.key", ios::in);
        char vcode[64];
        actfp.getline(vcode, 64);
        actkey = vcode;
        actfp.close();
    }
    else {
    }

    system("title int�ڴ���乤��");
    while (again) {
        again = false;
        i = 0;
        for (;i < N;) {
            cls;cin.clear();cin.sync();
            if(actkey.length()||(!(noactcnt % 3))) SetConsoleColor((WORD)07);
            if (!actkey.length()) {
                noactcnt++;
                if (noactcnt > 2147483648) noactcnt = 0;
                if (!(noactcnt % 25)) NoActiveFn();
            }
            cout << "========\n"
                "����ѡ��: \n"
                "����: [��Ҫ��Ϣ!!!!]����ʹ�ô˳�������Ҫ����!�˳������ڴ��д洢����,������!!!\n";
            if (actkey.length()) {
                SetConsoleColor((WORD)02);
                cout << "[������Կ:" << actkey << "](�Ѽ���)\n";
            } else  {
                SetConsoleColor((WORD)04);
                cout << "[������Կ:δ����](�뾡�켤��)\n";
            }
            if(actkey.length()||(!(noactcnt % 3))) SetConsoleColor((WORD)07);
            cout << "��ʾ (��ǰ" << sizeof(int*) * 8 << "λ����):\n"
                <<(actkey.length()?"":"��~�������,\n")<<
                "��+�ͷ�������������ڴ�,\n"
                "����������������,\n"
                "������ " << i << " ��,�������� "/*<< N - i << */ << "������" << " ��,\n"
                "��&��ָ����ID����(������),\n"
                "��*�ͷ�ָ����ID,\n"
                "��!��\\��/�˳�,\n"
                "��$���ش���(����--show�ָ�),\n"
                "��%��ʾ���صĴ���,\n"
                "��#��ID��ָ����������ڴ�д������,\n"
                "��@��ID��ָ����������ڴ������: ";
            cout << endl << "========" << endl;
            if(wout.str()!="") cout << endl << "�ϴ����: " << oss_last.str();
            oss_last.str("");
            movePosition(0,0);
            movePosition(1,12);
            c = getche();
            if (c == '+') {
                for (int j = 0; j < i; j++) {
                    if (mallocs[j] == NULL) continue;
                    ostringstream oss;
                    oss << mallocs[j];
                    free(mallocs[j]);
                    mallocs[j] = NULL;
                    wout << "�ɹ��ͷ��ڴ�: " << oss.str() << endl;
                }
                again = true;
                break;
            }
            else if (c == '!' || c == '��' || c == '\\' || c == '/') {
                char a = 'Y';
                cls;
                if (mallocs[0]!=NULL){
                    cout << "����!����δ�ͷŵ��ڴ�,ֱ���˳��������ڴ�й©!\nҪǿ���˳�,����Y;\n" <<
                        "Ҫ�ͷ��ڴ�,������N,������+,�������!��\\�˳�." << endl << "��������ѡ��:";
                    a = getche();
                    cout << endl;
                }
                if (a == 'Y' || a == 'y') exit(0);
            }
            else if (c == '#') {
                cls;
                icid = NULL; mcon=NULL;
                cout << "======" << endl;
                cout << "����ID: ";
                cin >> icid;
                if(cin.good()!=1){
                    wout << "����Ƿ�:��ȡʧ��"; continue;
                }
                if(icid>mallocs.capacity()){
                    wout << "����Ƿ�:Խ��"; continue;
                }
                if (mallocs[icid] == NULL) {
                    wout << "����Ƿ�:����ʹ��δ������ڴ�"; continue;
                }
                cout << "����Ҫд�������(int, -2147483648<=������<2147483648): ";
                cin >> mcon;
                if(mcon<-2147483648||mcon>2147483647){
                    wout << "����Ƿ�:Խ��"; continue;
                }
                if(cin.good()!=1){
                    wout << "����Ƿ�:��ȡʧ��"; continue;
                }
                wout << "����д��... ";
                *mallocs[icid] = mcon;
                wout << "д��ɹ�" << endl;
                cout << "======" << endl;
            }
            else if (c == '@') {
                icid = NULL; mcon=NULL;
                cls;
                cout << "======" << endl;
                cout << "����ID: ";
                cin >> icid;
                if (cin.good() != 1) {
                    wout << "����Ƿ�:��ȡʧ��"; continue;
                }
                if (icid > mallocs.capacity()) {
                    wout << "����Ƿ�:Խ��"; continue;
                }
                if (mallocs[icid] == NULL) {
                    wout << "����Ƿ�:����ʹ��δ������ڴ�"; continue;
                }
                wout << "IDΪ " << icid << " (�ڴ��ַΪ" << mallocs[icid] << ") ���ڴ�ֵ��: "<<endl << *mallocs[icid] << endl;
                cout << "======" << endl;
            }
            else if (c == '&') {
                icid = NULL; mcon=NULL;
                cls;
                cout << "======" << endl;
                cout << "����ID: ";
                cin >> icid;
                if (cin.good() != 1) {
                    wout << "����Ƿ�:��ȡʧ��"; continue;
                }
                mallocs.reserve(icid);
                wout << "��������... "; 
                if (mallocs[icid] = (int*)malloc(MALLOCSLENGTH))
                    wout << "�ɹ������ڴ�: " << mallocs[icid] << " ID: " << icid << endl;
                else wout << "�޷������ڴ�" << endl;
                cout << "======" << endl;
            }
            else if (c == '*') {
                icid = NULL; mcon=NULL;
                cls;
                cout << "======" << endl;
                cout << "����ID: ";
                cin >> icid;
                if (cin.good() != 1) {
                    wout << "����Ƿ�:��ȡʧ��"; continue;
                }
                if (icid > mallocs.capacity()) {
                    wout << "����Ƿ�:Խ��"; continue;
                }
                if (mallocs[icid] == NULL) continue;
                ostringstream oss;
                oss << mallocs[icid];
                free(mallocs[icid]);
                mallocs[icid] = NULL;
                wout << "�ɹ��ͷ��ڴ�: " << oss.str() << endl;
            }
            else if (c == '$') {
                hideWindow();
                again = true;
                break;
            }
            else if (c == '%') {
                oss.str("");
                oss << argv[0] << "  --show";
                system(oss.str().c_str());
                exit(0);
            }
            else if (c == '~') {
                if (system("act")) {
                    MessageBox(FindWindow("ConsoleWindowClass", NULL), "����ʧ��!", "����", MB_ICONERROR);
                    continue;
                };
                WinExec((((string)"powershell start-process -filepath \"")+GetCommandLineA()+"\"").c_str(), SW_SHOW);
                exit(0);
            }
            else {
                cout << "======" << endl;
                while (mallocs[i] && mallocs[i] != NULL) i++;
                if (mallocs[i] = (int*)malloc(MALLOCSLENGTH))
                    wout << "�ɹ������ڴ�: " << mallocs[i] << " ID: " << i << endl;
                else wout << "�޷������ڴ�" << endl;
                i++;cout << "======" << endl;
            }
        }
    }
    cls;
    cout << "�޷������ڴ�,�����ͷ�..." << endl; 
    for (int j = 0; j < i; j++) {
        ostringstream oss;
        oss << mallocs[j];
        free(mallocs[j]);
        mallocs[j] = NULL;
        cout << "�ɹ��ͷ��ڴ�: " << oss.str() << endl;
    }
    exit(0);
}

bool file_exists(const std::string& name) {
    fstream _file;
    _file.open(name, ios::in|ios::binary);
    if (!_file) {
        _file.close();
        return false;
    } else {
        _file.close();
        return true;
    }
}
void hideWindow() {
    if (MessageBox(NULL,"ȷ�����ش���?\nͨ������ͬĿ¼ִ�� ���� --show �ָ�","ȷ����?",MB_ICONQUESTION|MB_OKCANCEL) == IDCANCEL) return;
    HWND hwnd;
    hwnd = FindWindow("ConsoleWindowClass", NULL);	//���������ڵ������ʹ�������ƥ��ָ�����ַ���,�������Ӵ��ڡ�
    if (hwnd)
    {
        ShowWindow(hwnd, SW_HIDE);				//����ָ�����ڵ���ʾ״̬
        while (1) {
            bool fe = file_exists("maf_ma_t_sw_show_test");
            //MessageBox(NULL,(""+file_exists("maf_ma_t_sw_show_test")),"",MB_OK);
            if (fe) {
                ShowWindow(hwnd, SW_SHOW);
                system("del /f /s /q maf_ma_t_sw_show_test");
                return;
            } 
            Sleep(3000);
        }
    }
    else return;
}