#pragma once
#include <string>
using namespace std;
class CItem
{
public:
	WCHAR LoaiChiTieu[20];
	unsigned long long TienChi;
	wstring ChiTiet;
	int getType();
	CItem();
	~CItem();
};

