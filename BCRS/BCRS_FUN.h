// BCRS_FUN.h: ͷ�ļ�
// ˫Ŀ�����λ�������������

#ifndef _BCRS_FUN_H_
#define _BCRS_FUN_H_
#include <vector>
typedef std::vector<CString> VCTSTR;


/// <summary>
/// д�������ļ�
/// </summary>
/// <param name="lpAppName">�ֶ�</param>
/// <param name="vsKeyName">����</param>
/// <param name="vsKeyValue">��ֵ</param>
/// <param name="lpFileName">�ļ�·��</param>
/// <returns>�ɹ�д�뷵��TRUE</returns>
BOOL WriteIni(LPCWSTR lpAppName, VCTSTR vsKeyName, VCTSTR vsKeyValue, LPCWSTR lpFileName);
BOOL WriteIni(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpKeyValue, LPCWSTR lpFileName);

/// <summary>
/// ��ȡ�����ļ�
/// </summary>
/// <param name="lpAppName">�ֶ�</param>
/// <param name="vsKeyName">����</param>
/// <param name="vsRetValue">���صļ�ֵ</param>
/// <param name="lpFileName">�ļ�·��</param>
/// <returns>�ɹ���ȡ����TRUE</returns>
BOOL ReadIni(LPCWSTR lpAppName, VCTSTR vsKeyName, VCTSTR& vsRetValue, LPCWSTR lpFileName);
BOOL ReadIni(LPCWSTR lpAppName, LPCWSTR lpKeyName, CString& lpRetValue, LPCWSTR lpFileName);

/// <summary>
/// ��������
/// </summary>
/// <param name="fliename">�ļ�������: "test.txt"</param>
/// <param name="dat">��������</param>
void LoadData(const char* fliename, std::vector<double> dat);












#endif // 

