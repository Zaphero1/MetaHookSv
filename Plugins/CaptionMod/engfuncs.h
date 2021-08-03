#pragma once

#include "plugins.h"
#include <usercmd.h>
#include <com_model.h>
#include <studio.h>
#include <pm_defs.h>
#include <tier0/basetypes.h>
#include <cvardef.h>
#include "enginedef.h"

typedef struct
{	
	void (*S_Init)(void);
	sfx_t *(*S_FindName)(char *name, int *pfInCache);//hooked
	void (*S_StartDynamicSound)(int entnum, int entchannel, sfx_t *sfx, float *origin, float fvol, float attenuation, int flags, int pitch);//hooked
	void (*S_StartStaticSound)(int entnum, int entchannel, sfx_t *sfx, float *origin, float fvol, float attenuation, int flags, int pitch);//hooked

	int(__fastcall *ScClient_FindSoundEx)(void *pthis, int, const char *soundName);
	HMODULE fmodex;
	int(__stdcall *FMOD_Sound_getLength)(int a1, void* a2, int a3);//?getLength@Sound@FMOD@@QAG?AW4FMOD_RESULT@@PAII@Z

	float *(*GetClientColor)(int clientIndex);
	void *GameViewport;
	bool (__fastcall *GameViewport_AllowedToPrintText)(void *pthis, int);
	int *g_iVisibleMouse;
	void(__fastcall *GameViewport_UpdateCursorState)(void *pthis, int);

	client_textmessage_t *(*pfnTextMessageGet)(const char *pName);
	void(*MessageMode_f)(void);
	void(*MessageMode2_f)(void);
	sfxcache_t *(*S_LoadSound)(sfx_t *s, channel_t *ch);
	void *(**pfnClientFactory)(void);
	FARPROC (WINAPI *GetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
	hook_t *hk_GetProcAddress;
	double *pcl_time;
	double *pcl_oldtime;
	char szLanguage[32];
}cap_funcs_t;

#define cl_time (*gCapFuncs.pcl_time)
#define cl_oldtime (*gCapFuncs.pcl_oldtime)

extern cap_funcs_t gCapFuncs;