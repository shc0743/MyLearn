#include "UnExp.h"

#pragma warning(push)
#pragma warning(disable: 4218)
PspTerminateThreadByPointer = 0;
PspExitThread_ PspExitThread = 0;
#pragma warning(pop)

//������ַ
ULONG_PTR KxSearchFeatureCodeForAddress(ULONG_PTR StartAddress, PUCHAR FeatureCode, int FeatureCodeSize, int Search_MaxLength)
{
	PUCHAR Start_scan = (PUCHAR)StartAddress;
	BOOLEAN IsTrue = FALSE;
	ULONG_PTR Real_address = 0;
	int i, fi;

	for (i = 0; i < Search_MaxLength; i++)
	{
		if (Start_scan[i] == FeatureCode[0])
		{
			for (fi = 0; fi < FeatureCodeSize; fi++)
			{
				if (Start_scan[i + fi] != FeatureCode[fi])
				{
					//�κ�һ���ֽڲ�ͬ,�����ʧ��,����
					IsTrue = FALSE;
					break;
				}
				else
				{
					if (fi == FeatureCodeSize - 1)
					{
						IsTrue = TRUE;
						//ָ�����ȵ��ֽڶ���ͬ.��Ϊ�ҵ���
					}
				}
			}

			if (IsTrue == TRUE)
			{
				Real_address = (ULONG_PTR)&Start_scan[i];
				break;
			}
		}
	}
	return Real_address;
}
//��ȡ PspTerminateThreadByPointer 32/64 ����Win
//0 : 32 0x50 64 0xA0
//0x50,0xe8 (push eax call XXXX) / 0xA0,0xe8 (mov r8b call XXXX)
ULONG_PTR KxGetPspTerminateThreadByPointerAddressX_7Or8Or10(UCHAR FeatureCode0)
{
	//ժ���Ĵ���
	UNICODE_STRING  FunName;
	UCHAR FeatureCode[2]; //������
	FeatureCode[0] = FeatureCode0;
	FeatureCode[1] = 0xe8;

	ULONG_PTR   FeatureAddress = 0; //�ҵ�����������׵�ַ
	ULONG_PTR   FunAddress = 0;
	ULONG_PTR   RetAddress = 0;//���շ���
	ULONG_PTR   FunAdd = 0;

	RtlInitUnicodeString(&FunName, L"PsTerminateSystemThread");
	FunAdd = (ULONG_PTR)MmGetSystemRoutineAddress(&FunName);

	//���Ȼ�ȡPsTerminateSystemThread���������ַ.��ʵ�������ֱ�ӵ��õ�PspTerminateThreadByPointer
	//�����������ǳ���С,����ͨ����������.��λcall Ȼ��call�ĵ�ַ�ó������������㼴��
	FeatureAddress = KxSearchFeatureCodeForAddress(FunAdd, FeatureCode, 2, 0x27);
	//PsTerminateSystemThread������Ҫ�����ĳ���0x27
	if (FeatureAddress != 0) {
		RtlCopyMemory(&FunAddress, (ULONG_PTR*)(FeatureAddress + 2), 4);

		//Ŀ���ַ=����ָ��ĵ�ַ+������E8����������32λ��
		//ע�⣬������E8����������32λ�� �� little-endian ��ʽ�ģ���8λ��ǰ����8λ�ں�
		RetAddress = FunAddress + (FeatureAddress + 2 + 4);
	}
	return RetAddress;
}

//��ȡ PspExitThread 32/64 ����Win
ULONG_PTR KxGetPspExitThread_32_64() {
	ULONG_PTR i = 0;
	ULONG_PTR callcode = 0;
	ULONG_PTR  curcodeptr = 0;
	ULONG_PTR RetAddress = 0;
	ULONG_PTR AddressPTTBP = (ULONG_PTR)PspTerminateThreadByPointer;//PspTerminateThreadByPointer��ַ
	PUCHAR Scan = (PUCHAR)AddressPTTBP;

	if (AddressPTTBP != 0)
	{
		//�������0x60��
		for (i = 1; i < 0x60; i++)
		{
			curcodeptr = (ULONG_PTR)&Scan[i];
			if (MmIsAddressValid((PVOID)curcodeptr))
			{
				if (Scan[i] == 0xe8/*call immed16 ��λλ�� (һ�� 5 �ֽ�) */
					&& Scan[i + 5] == 0xcc/*int 3*/)//��������࣬ÿ��ϵͳcall PspExitThread���涼��int 3��
					//��ǰ���ָ�һ����������ʹ�ô˷������
				{
					//Ŀ���ַ=������+5+ԭʼ��ַ
					RtlMoveMemory(&callcode, (PVOID)(curcodeptr + 1), 4);//��ת��ַ
					RetAddress = (ULONG_PTR)((curcodeptr/*������*/ + 5 + callcode) & 0x0000ffffffff);
					break;
				}
			}
		}
	}
	else KdPrint(("AddressPTTBP == 0!"));
	return RetAddress;
}