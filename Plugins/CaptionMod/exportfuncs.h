#pragma once
#include <gl/gl.h>
#include <const.h>
#include <triangleapi.h>
#include <cl_entity.h>
#include <event_api.h>
#include <ref_params.h>
#include <com_model.h>
#include <cvardef.h>
#include <r_efx.h>
#include <r_studioint.h>
#include <pm_movevars.h>
#include <studio.h>
#include <entity_types.h>
#include <usercmd.h>
#include "enginedef.h"

extern cl_enginefunc_t gEngfuncs;

char * NewV_strncpy(char *a1, const char *a2, size_t a3);

void HUD_Init(void);
int HUD_VidInit(void);
void HUD_Frame(double time);
int HUD_Redraw(float time, int intermission);
void HUD_Shutdown(void);
void IN_MouseEvent(int mstate);
void IN_Accumulate(void);
void CL_CreateMove(float frametime, struct usercmd_s *cmd, int active);

client_textmessage_t *pfnTextMessageGet(const char *pName);
void Client_FillAddress(void);
void Client_InstallHooks(void);
void Client_UninstallHooks(void);
void Engine_FillAddress(void);
void Engine_InstallHooks(void);
void Engine_UninstallHooks(void);
void BaseUI_InstallHook(void);
void GameUI_InstallHooks(void);
void GameUI_UninstallHooks(void);
void ClientVGUI_InstallHook(void);
void ClientVGUI_Shutdown(void);
void VGUI1_InstallHook(void);
void VGUI1_Shutdown(void);
void Surface_InstallHooks(void);
void Surface_UninstallHooks(void);
void Scheme_InstallHooks(void);
void KeyValuesSystem_InstallHook(void);

void *NewClientFactory(void);

const char *GetBaseDirectory();

int FileSystem_SetGameDirectory(const char *pDefaultDir, const char *pGameDir);

IBaseInterface *NewCreateInterface(const char *pName, int *pReturnCode);

void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, float *origin, float fvol, float attenuation, int flags, int pitch);
void S_StartStaticSound(int entnum, int entchannel, sfx_t *sfx, float *origin, float fvol, float attenuation, int flags, int pitch);
sfx_t *S_FindName(char *name, int *pfInCache);

void __fastcall CWin32Font_GetCharRGBA(void *pthis, int, int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba);

void MessageMode_f(void);
void MessageMode2_f(void);
void Cap_Version_f(void);

LRESULT WINAPI VID_MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int __fastcall ScClient_FindSoundEx(void* pthis, int, const char *sound);
void __fastcall WeaponsResource_SelectSlot(void *pthis, int, int iSlot, int fAdvance, int iDirection);

extern cvar_t* cap_debug;
extern cvar_t* cap_enabled;
extern cvar_t* cap_max_distance;
extern cvar_t* cap_min_avol;
extern cvar_t *cap_netmessage;
extern cvar_t *cap_hudmessage;