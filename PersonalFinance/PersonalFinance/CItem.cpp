#include "stdafx.h"
#include "CItem.h"


CItem::CItem()
{
	
}


CItem::~CItem()
{
}

int CItem::getType()
{
	if (wcscmp(this->LoaiChiTieu, L"Ăn uống") == 0)
	{
		return 0;
	}
	else if (wcscmp(this->LoaiChiTieu, L"Di chuyển") == 0)
	{
		return 1;
	}
	else if (wcscmp(this->LoaiChiTieu, L"Nhà cửa") == 0)
	{
		return 2;
	}
	else if (wcscmp(this->LoaiChiTieu, L"Xe cộ") == 0)
	{
		return 3;
	}
	else if (wcscmp(this->LoaiChiTieu, L"Nhu yếu phẩm") == 0)
	{
		return 4;
	}
	else if (wcscmp(this->LoaiChiTieu, L"Dịch vụ") == 0)
	{
		return 5;
	}
}