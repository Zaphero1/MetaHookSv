#pragma once

typedef struct shadow_control_s
{
	bool enabled;
	vec3_t vforward;
	vec3_t vright;
	vec3_t vup;
	vec3_t angles;
	float color[4];
	float distfade[2];
	float lumfade[2];
	float quality[3][2];
}shadow_control_t;

extern shadow_control_t r_shadow_control;

typedef struct light_dynamic_s
{
	int type;
	vec3_t origin;
	float color[3];
	float distance;
	float ambient;
	float diffuse;
	float specular;
	float specularpow;
}light_dynamic_t;

extern std::vector<light_dynamic_t> g_DynamicLights;

extern cvar_t *r_light_dynamic;
extern cvar_t *r_light_debug;

extern bool drawgbuffer;

typedef struct
{
	int program;
	int gbufferTex;
	int stencilTex;
	int depthTex;
	int viewpos;
	int lightdir;
	int lightpos;
	int lightcolor;
	int lightcone;
	int lightradius;
	int lightambient;
	int lightdiffuse;
	int lightspecular;
	int lightspecularpow;
	int modelmatrix;
}dlight_program_t;

typedef struct
{
	int program;
	int gbufferTex;
	//int stencilTex;
	int depthTex;
	int linearDepthTex;
	int viewpos;
	int viewmatrix;
	int projmatrix;
	int invprojmatrix;
}dfinal_program_t;

void R_InitLight(void);
void R_ShutdownLight(void);
bool R_BeginRenderGBuffer(void);
void R_EndRenderGBuffer(void);
void R_SetGBufferMask(int mask);

#define GBUFFER_INDEX_DIFFUSE		0
#define GBUFFER_INDEX_LIGHTMAP		1
#define GBUFFER_INDEX_WORLDNORM		2
#define GBUFFER_INDEX_SPECULAR		3
#define GBUFFER_INDEX_ADDITIVE		4
#define GBUFFER_INDEX_MAX			5

#define GBUFFER_MASK_DIFFUSE		(1<<GBUFFER_INDEX_DIFFUSE)
#define GBUFFER_MASK_LIGHTMAP		(1<<GBUFFER_INDEX_LIGHTMAP)
#define GBUFFER_MASK_WORLDNORM		(1<<GBUFFER_INDEX_WORLDNORM)
#define GBUFFER_MASK_SPECULAR		(1<<GBUFFER_INDEX_SPECULAR)
#define GBUFFER_MASK_ADDITIVE		(1<<GBUFFER_INDEX_ADDITIVE)

#define GBUFFER_MASK_ALL			(GBUFFER_MASK_DIFFUSE | GBUFFER_MASK_LIGHTMAP | GBUFFER_MASK_WORLDNORM | GBUFFER_MASK_SPECULAR | GBUFFER_MASK_ADDITIVE)

#define DLIGHT_SPOT_ENABLED			1
#define DLIGHT_POINT_ENABLED		2
#define DLIGHT_VOLUME_ENABLED		4

#define DFINAL_LINEAR_FOG_ENABLED		1

#define DLIGHT_POINT					0
#define DLIGHT_SPOT						1