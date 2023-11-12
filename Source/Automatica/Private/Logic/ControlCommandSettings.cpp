// 2023 Lukas A. Schopf // thelaumix productions


#include "Logic/ControlCommandSettings.h"

bool UControlCommandSettings::GetCommandSettingDrawer(const ELogicControlType Type, FCommandSettingDrawer& Drawer)
{
	if (ControlMap.Contains(Type))
	{
		Drawer = *ControlMap.Find(Type);
		return true;
	}
	return false;
}
