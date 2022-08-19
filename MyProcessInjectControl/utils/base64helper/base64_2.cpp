#include "base64_2.h"


static uint8 alphabet_map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static uint8 reverse_map[] =
{
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62, 255, 255, 255, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255, 255, 255, 255, 255,
	255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255,
	255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 255, 255, 255, 255, 255
};

//GB2312��UTF-8��ת��
static char* G2U(const char* gb2312) {
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[(size_t)len + 1];
	memset(wstr, 0, (size_t)len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[(size_t)len + 1];
	memset(str, 0, (size_t)len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}
//UTF-8��GB2312��ת��
static char* U2G(const char* utf8) {
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[(size_t)len + 1];
	memset(wstr, 0, (size_t)len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[(size_t)len + 1];
	memset(str, 0, (size_t)len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

size_t __stdcall base64_encode(char* input, uint8* encode) {
	//1���������ĵ��ַ��� �ַ����루windowsĬ����gbk��ת����unicode
	
	//2���ַ����뷽ʽ��utf-8�Ķ�����
	uint8* text = (uint8*)G2U(input);
	uint32 text_len = (uint32)strlen((char*)text);

	uint32 i, j;
	for (i = 0, j = 0; i + 3 <= text_len; i += 3)
	{
		encode[j++] = alphabet_map[text[i] >> 2];                             //ȡ����һ���ַ���ǰ6λ���ҳ���Ӧ�Ľ���ַ�
		encode[j++] = alphabet_map[((text[i] << 4) & 0x30) | (text[i + 1] >> 4)];     //����һ���ַ��ĺ�2λ��ڶ����ַ���ǰ4λ������ϲ��ҵ���Ӧ�Ľ���ַ�
		encode[j++] = alphabet_map[((text[i + 1] << 2) & 0x3c) | (text[i + 2] >> 6)];   //���ڶ����ַ��ĺ�4λ��������ַ���ǰ2λ��ϲ��ҳ���Ӧ�Ľ���ַ�
		encode[j++] = alphabet_map[text[i + 2] & 0x3f];                         //ȡ���������ַ��ĺ�6λ���ҳ�����ַ�
	}

	if (i < text_len)
	{
		uint32 tail = text_len - i;
		if (tail == 1)
		{
			encode[j++] = alphabet_map[text[i] >> 2];
			encode[j++] = alphabet_map[(text[i] << 4) & 0x30];
			encode[j++] = '=';
			encode[j++] = '=';
		}
		else //tail==2
		{
			encode[j++] = alphabet_map[text[i] >> 2];
			encode[j++] = alphabet_map[((text[i] << 4) & 0x30) | (text[i + 1] >> 4)];
			encode[j++] = alphabet_map[(text[i + 1] << 2) & 0x3c];
			encode[j++] = '=';
		}
	}
	encode[j] = 0;
	return j;
}

size_t __stdcall base64_decode(const uint8* code, size_t code_len, char* str)
{
	uint8 plain[1024]{ 0 };
	//assert((code_len & 0x03) == 0);  //��������������ش�������ֹ����ִ�С�4�ı�����
	if (!((code_len & 0x03) == 0)) return 0;

	size_t i, j = 0;
	uint8 quad[4]{ 0 };
	for (i = 0; i < code_len; i += 4)
	{
		for (uint32 k = 0; k < 4; k++)
		{
			quad[k] = reverse_map[code[i + k]];//���飬ÿ���ĸ��ֱ�����ת��Ϊbase64���ڵ�ʮ������
		}

		assert(quad[0] < 64 && quad[1] < 64);

		plain[j++] = (quad[0] << 2) | (quad[1] >> 4); //ȡ����һ���ַ���Ӧbase64���ʮ��������ǰ6λ��ڶ����ַ���Ӧbase64���ʮ��������ǰ2λ�������

		if (quad[2] >= 64)
			break;
		else if (quad[3] >= 64)
		{
			plain[j++] = (quad[1] << 4) | (quad[2] >> 2); //ȡ���ڶ����ַ���Ӧbase64���ʮ�������ĺ�4λ��������ַ���Ӧbase64���ʮ��������ǰ4λ�������
			break;
		}
		else
		{
			plain[j++] = (quad[1] << 4) | (quad[2] >> 2);
			plain[j++] = (quad[2] << 6) | quad[3];//ȡ���������ַ���Ӧbase64���ʮ�������ĺ�2λ���4���ַ��������
		}
	}
	plain[j] = 0;
	//char str[1024] = "";
	strcpy_s(str, 1024, (char*)plain);
	strcpy_s(str, sizeof(plain), U2G(str));
	return j;
}
