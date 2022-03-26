#include "pch.h"
#include "BCRS_FUN.h"
#include <fstream>
#include <iostream>
using namespace std;


BOOL WriteIni(LPCWSTR lpAppName, VCTSTR vsKeyName, VCTSTR vsKeyValue, LPCWSTR lpFileName)
{
	// ��ֵ������Ŀ��ƥ���׳��쳣
	if (vsKeyName.size() != vsKeyValue.size())
	{
		throw(TEXT("The number of KeyName and KeyValue is not equal !"));
	}

	CFileFind finder;
	BOOL ifFind = finder.FindFile(lpFileName);// ����ָ��·�����ļ�
	std::vector<CString>::iterator itKeyName = vsKeyName.begin();
	std::vector<CString>::iterator itKeyValue = vsKeyValue.begin();

	// �����Ƿ����ini�ļ����������ڣ�������һ���µ�Ĭ�����õ�ini�ļ�
	for (;itKeyName != vsKeyName.end(); itKeyName++, itKeyValue++)
	{
		WritePrivateProfileStringW(lpAppName, *itKeyName, *itKeyValue, lpFileName);
	}
	finder.Close();
	return TRUE;
}

BOOL WriteIni(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpKeyValue, LPCWSTR lpFileName)
{
	CFileFind finder;
	BOOL ifFind = finder.FindFile(lpFileName);// ����ָ��·�����ļ�
	WritePrivateProfileStringW(lpAppName, lpKeyName, lpKeyValue, lpFileName);
	finder.Close();
	return TRUE;
}

BOOL ReadIni(LPCWSTR lpAppName, VCTSTR vsKeyName, VCTSTR& vsRetValue, LPCWSTR lpFileName)
{
	CFileFind finder;
	BOOL ifFind = finder.FindFile(lpFileName);// ����ָ��·�����ļ�
	std::vector<CString>::iterator itKeyName = vsKeyName.begin();
	CString strBuffer;
	// �����Ƿ����ini�ļ����������ڣ�������һ���µ�Ĭ�����õ�ini�ļ�
	if (!ifFind)
	{
		throw(TEXT("Load ini ERROR!"));
	}
	else
	{
		for (;itKeyName != vsKeyName.end();itKeyName++)
		{
			GetPrivateProfileStringW(lpAppName, *itKeyName, TEXT(""), strBuffer.GetBuffer(MAX_PATH), MAX_PATH, lpFileName);
			vsRetValue.push_back(strBuffer);
		}
	}
	finder.Close();
	return TRUE;
}

BOOL ReadIni(LPCWSTR lpAppName, LPCWSTR lpKeyName, CString& lpRetValue, LPCWSTR lpFileName)
{
	CFileFind finder;
	BOOL ifFind = finder.FindFile(lpFileName);// ����ָ��·�����ļ�
	CString strBuffer;
	GetPrivateProfileStringW(lpAppName, lpKeyName, TEXT(""), lpRetValue.GetBuffer(MAX_PATH), MAX_PATH, lpFileName);

	finder.Close();
	return TRUE;
}

void LoadData(const char* fliename, std::vector<double> dat)
{
	std::ofstream output_u(fliename);
	std::ostream_iterator<double> output_iterator_u(output_u, "\n");
	std::copy(dat.begin(), dat.end(), output_iterator_u);
}


