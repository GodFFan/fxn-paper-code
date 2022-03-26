// BCRS_FUN.h: 头文件
// 双目相机定位软件公共函数库

#ifndef _BCRS_FUN_H_
#define _BCRS_FUN_H_
#include <vector>
typedef std::vector<CString> VCTSTR;


/// <summary>
/// 写入配置文件
/// </summary>
/// <param name="lpAppName">字段</param>
/// <param name="vsKeyName">键名</param>
/// <param name="vsKeyValue">键值</param>
/// <param name="lpFileName">文件路径</param>
/// <returns>成功写入返回TRUE</returns>
BOOL WriteIni(LPCWSTR lpAppName, VCTSTR vsKeyName, VCTSTR vsKeyValue, LPCWSTR lpFileName);
BOOL WriteIni(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpKeyValue, LPCWSTR lpFileName);

/// <summary>
/// 读取配置文件
/// </summary>
/// <param name="lpAppName">字段</param>
/// <param name="vsKeyName">键名</param>
/// <param name="vsRetValue">返回的键值</param>
/// <param name="lpFileName">文件路径</param>
/// <returns>成功读取返回TRUE</returns>
BOOL ReadIni(LPCWSTR lpAppName, VCTSTR vsKeyName, VCTSTR& vsRetValue, LPCWSTR lpFileName);
BOOL ReadIni(LPCWSTR lpAppName, LPCWSTR lpKeyName, CString& lpRetValue, LPCWSTR lpFileName);

/// <summary>
/// 下载数据
/// </summary>
/// <param name="fliename">文件名，如: "test.txt"</param>
/// <param name="dat">数据向量</param>
void LoadData(const char* fliename, std::vector<double> dat);












#endif // 

