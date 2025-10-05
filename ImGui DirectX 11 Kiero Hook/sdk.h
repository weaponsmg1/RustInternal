#pragma once
#include "il2cpp.h"
#include "vec.h"
#include "vec2.h"
#include <vector>
#include <utility> // for std::pair
bool show_menu = true;

namespace mem
{
	template<typename T> T read(uintptr_t address) {
		try { return *(T*)address; }
		catch (...) { return T(); }
	}

	template<typename T> void write(uintptr_t address, T value) {
		try { *(T*)address = value; }
		catch (...) { return; }
	}
}

namespace sdk
{
	uintptr_t Base;
	uintptr_t GameAssembly;
	uintptr_t UnityPlayer;

	void mousemove(float tarx, float tary, float X, float Y, int smooth)
	{
		float ScreenCenterX = (X / 2);
		float ScreenCenterY = (Y / 2);
		float TargetX = 0;
		float TargetY = 0;

		smooth = smooth + 3;

		if (tarx != 0)
		{
			if (tarx > ScreenCenterX)
			{
				TargetX = -(ScreenCenterX - tarx);
				TargetX /= smooth;
				if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
			}

			if (tarx < ScreenCenterX)
			{
				TargetX = tarx - ScreenCenterX;
				TargetX /= smooth;
				if (TargetX + ScreenCenterX < 0) TargetX = 0;
			}
		}

		if (tary != 0)
		{
			if (tary > ScreenCenterY)
			{
				TargetY = -(ScreenCenterY - tary);
				TargetY /= smooth;
				if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
			}

			if (tary < ScreenCenterY)
			{
				TargetY = tary - ScreenCenterY;
				TargetY /= smooth;
				if (TargetY + ScreenCenterY < 0) TargetY = 0;
			}
		}
		mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);
	}
}

namespace vars
{
	inline static UINT vps = 1;
	Vector2 screen_size = { 0, 0 };
	Vector2 screen_center = { 0, 0 };
	D3D11_VIEWPORT viewport;
	bool initil2cpp = false;
	
	bool ESPLines = false;
	bool ESPBox = false;
	bool crosshair = false;
	int currentplayeridx = 0;
	float crosshairsize = 8.0f;


	ImColor CrosshairColor = ImColor(255.0f / 255, 255.0f / 255, 255.0f / 255);
	ImColor ESPLineColor = ImColor(255.0f / 255, 255.0f / 255, 255.0f / 255);

	Vector3 PlayerPos = { 0, 0, 0 };

}
