#include "includes.h"
#include "Rendering.hpp"
#include "Font.h"
#include "sdk.h"
#include "Menu.h"
#include "Functions.h"
#include "kiero/minhook/include/MinHook.h"
#include "il2cpp_resolver.hpp"
#include "Lists.hpp"
#include "Callback.hpp"
#include <Utils/VFunc.hpp>
#include <iostream>
#include <PaternScan.hpp>
#include <intrin.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

void initvars() {
	if (IL2CPP::Initialize(true))
	{
		printf("[ DEBUG ] Il2Cpp initialized\n");
	}
	else 
	{
		printf("[ DEBUG ] Il2Cpp initialize failed, quitting...");
		Sleep(300);
		exit(0);
	}
	sdk::Base = (uintptr_t)GetModuleHandleA(NULL);
	sdk::GameAssembly = (uintptr_t)GetModuleHandleA("GameAssembly.dll");
	sdk::UnityPlayer = (uintptr_t)GetModuleHandleA("UnityPlayer.dll");
}

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProcA(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;

void Player_Cache()
{
	while (true)
	{
		if (!vars::initil2cpp)
			continue;
		void* m_pThisThread = IL2CPP::Thread::Attach(IL2CPP::Domain::Get());
		LocalPlayer = NULL;
		PlayerList.clear();
		auto list = Unity::Object::FindObjectsOfType<Unity::CComponent>("BasePlayer");
		for (int i = 0; i < list->m_uMaxLength; i++)
		{
			if (!list->operator[](i)) {
				continue;
			}

			PlayerList.push_back(list->operator[](i)->GetGameObject());
		}

		IL2CPP::Thread::Detach(m_pThisThread);
		Sleep(1000);
	}
}

static DWORD lastShotTime = 0;
static DWORD lasttick = 0;

auto RectFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags) -> VOID
{
	auto vList = ImGui::GetBackgroundDrawList();
	vList->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color, rounding, rounding_corners_flags);
}
auto HealthBar(float x, float y, float w, float h, int phealth, ImColor col) -> VOID
{
	auto vList = ImGui::GetBackgroundDrawList();

	int healthValue = max(0, min(phealth, 100));

	int barColor = ImColor
	(min(510 * (100 - healthValue) / 100, 255), min(510 * healthValue / 100, 255), 25, 255);
	vList->AddRect(ImVec2(x - 1, y - 1), ImVec2(x + w + 1, y + h + 1), col);
	RectFilled(x, y, x + w, y + (((float)h / 100.0f) * (float)phealth), barColor, 0.0f, 0);
}

void renderloop()
{
	DWORD currentTime = GetTickCount64(); // Get current time in milliseconds

	if (!vars::initil2cpp)
		return;

	if (vars::crosshair)
	{
		ImColor color = vars::CrosshairColor;
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(vars::screen_center.x - vars::crosshairsize, vars::screen_center.y), ImVec2((vars::screen_center.x - vars::crosshairsize) + (vars::crosshairsize * 2), vars::screen_center.y), color, 1.2f);
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(vars::screen_center.x, vars::screen_center.y - vars::crosshairsize), ImVec2(vars::screen_center.x, (vars::screen_center.y - vars::crosshairsize) + (vars::crosshairsize * 2)), color, 1.2f);
	}
	
	if (PlayerList.size() > 0)
	{
		for (int i = 0; i < PlayerList.size(); i++)
		{
			if (!PlayerList[i]) // Verify that the player is valid
				continue;

			vars::currentplayeridx = i; // Set the current player index in our sdk

			//auto localplayerPosition = LocalPlayer->GetTransform()->GetPosition();

			auto playerPosition = PlayerList[vars::currentplayeridx]->GetTransform()->GetPosition();

			if (vars::ESPLines)
			{
				Unity::Vector3 root_pos = playerPosition;
				root_pos.y -= 0.2f;
				Vector2 pos;
				if (Functions::worldtoscreen(root_pos, pos))
				{
					ImColor Colortouse = vars::ESPLineColor;
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vars::screen_center.x, vars::screen_size.y), ImVec2(pos.x, pos.y), Colortouse, 1.5f);
				}
				if (vars::ESPBox)
				{
					ImColor boxColor = vars::ESPLineColor;
					float boxHeight = 55.0f;
					float boxWidth = 25.0f;

					ImVec2 top_left(pos.x - boxWidth / 2.0f, pos.y - boxHeight);
					ImVec2 bottom_right(pos.x + boxWidth / 2.0f, pos.y);

					ImGui::GetBackgroundDrawList()->AddRect(
						top_left, bottom_right,
						boxColor, 0.0f, 0, 1.5f
					);
				}
			}
		}
	}

	if (currentTime - lasttick > 5) //  5 ms Timer For Whatever you want to do
	{	
		
		lasttick = currentTime;
	}
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	void* m_pThisThread = IL2CPP::Thread::Attach(IL2CPP::Domain::Get());

	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			ImGui::GetIO().Fonts->AddFontDefault();
			ImFontConfig font_cfg;
			font_cfg.GlyphExtraSpacing.x = 1.2;
			gameFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(TTSquaresCondensedBold, 14, 14, &font_cfg);
			ImGui::GetIO().Fonts->AddFontDefault();
			// Grab A shader Here If You want
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	pContext->RSGetViewports(&vars::vps, &vars::viewport);
	vars::screen_size = { vars::viewport.Width, vars::viewport.Height };
	vars::screen_center = { vars::viewport.Width / 2.0f, vars::viewport.Height / 2.0f };

	auto begin_scene = [&]() {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	};

	auto end_scene = [&]() {
		ImGui::Render();
	};

	begin_scene();

	auto isFrames = ImGui::GetFrameCount();
	static float isRed = 0.0f, isGreen = 0.01f, isBlue = 0.0f;
	if (isFrames % 1 == 0)
	{
		if (isGreen == 0.01f && isBlue == 0.0f)
		{
			isRed += 0.01f;

		}
		if (isRed > 0.99f && isBlue == 0.0f)
		{
			isRed = 1.0f;

			isGreen += 0.01f;

		}
		if (isGreen > 0.99f && isBlue == 0.0f)
		{
			isGreen = 1.0f;

			isRed -= 0.01f;

		}
		if (isRed < 0.01f && isGreen == 1.0f)
		{
			isRed = 0.0f;

			isBlue += 0.01f;

		}
		if (isBlue > 0.99f && isRed == 0.0f)
		{
			isBlue = 1.0f;

			isGreen -= 0.01f;

		}
		if (isGreen < 0.01f && isBlue == 1.0f)
		{
			isGreen = 0.0f;

			isRed += 0.01f;

		}
		if (isRed > 0.99f && isGreen == 0.0f)
		{
			isRed = 1.0f;

			isBlue -= 0.01f;

		}
		if (isBlue < 0.01f && isGreen == 0.0f)
		{
			isBlue = 0.0f;

			isRed -= 0.01f;

			if (isRed < 0.01f)
				isGreen = 0.01f;

		}
	}

	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(window, &mousePos);

	if (show_menu)
	{
		DrawMenu();
	}
	// Render
	try
	{
		renderloop();
	}
	catch (...) {}

	end_scene();

	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		show_menu = !show_menu;
		ImGui::GetIO().MouseDrawCursor = show_menu;
	}

	if (GetKeyState(VK_END) & 1)
	{
		MH_DisableHook(MH_ALL_HOOKS);
		MH_Uninitialize();
		show_menu = false;
	}

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	IL2CPP::Thread::Detach(m_pThisThread);

	return oPresent(pSwapChain, SyncInterval, Flags);
}

void initchair()
{
	initvars();
	IL2CPP::Callback::Initialize();
	kiero::bind(8, (void**)&oPresent, hkPresent);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Player_Cache, NULL, NULL, NULL);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			initchair();
			init_hook = true;
			vars::initil2cpp = true;
		}
	} while (!init_hook);
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE mod, DWORD reason, LPVOID lpReserved)
{
	if (reason == 1)
	{
		DisableThreadLibraryCalls(mod);
		CreateThread(nullptr, 0, MainThread, mod, 0, nullptr);
	}
	return TRUE;
}