#version 410

#include "common.h"

uniform ivec2 width_height;
uniform vec4 up_down_left_right;
uniform vec4 in_color;
uniform vec3 in_origin;
uniform vec3 in_angles;
uniform float in_scale;

uniform sampler2D baseTex;

in vec3 v_worldpos;
in vec3 v_normal;
in vec4 v_color;
in vec2 v_texcoord;

#ifdef GBUFFER_ENABLED

layout(location = 0) out vec4 out_Diffuse;
layout(location = 1) out vec4 out_Lightmap;
layout(location = 2) out vec4 out_WorldNorm;
layout(location = 3) out vec4 out_Specular;
layout(location = 4) out vec4 out_Additive;

#else

layout(location = 0) out vec4 out_Diffuse;

#endif

void main(void)
{
	ClipPlaneTest(v_worldpos.xyz, v_normal.xyz);

	vec4 baseColor = texture2D(baseTex, v_texcoord);

#if !defined(ADDITIVE_BLEND_ENABLED) && !defined(OIT_ADDITIVE_BLEND_ENABLED)
	//Alpha blend
	baseColor = TexGammaToLinear(baseColor);
	baseColor.a = pow(baseColor.a, SceneUBO.r_alpha_shift);
#else
	//Additive blend
	baseColor = TexGammaToLinear(baseColor);
	baseColor.a = pow(baseColor.a, SceneUBO.r_additive_shift);
#endif

	vec4 lightmapColor = v_color;
	lightmapColor.r = clamp(lightmapColor.r, 0.0, 1.0);
	lightmapColor.g = clamp(lightmapColor.g, 0.0, 1.0);
	lightmapColor.b = clamp(lightmapColor.b, 0.0, 1.0);
	lightmapColor.a = clamp(lightmapColor.a, 0.0, 1.0);

#if !defined(ADDITIVE_BLEND_ENABLED) && !defined(OIT_ADDITIVE_BLEND_ENABLED)
	//Alpha blend
	lightmapColor = GammaToLinear(lightmapColor);
#else
	//Additive blend
	lightmapColor = GammaToLinear(lightmapColor);
#endif

	vec3 vNormal = normalize(v_normal.xyz);

#ifdef GBUFFER_ENABLED

	vec2 vOctNormal = UnitVectorToOctahedron(vNormal);

	float flDistanceToFragment = distance(v_worldpos.xyz, SceneUBO.viewpos.xyz);

	out_Diffuse = baseColor;
	out_Lightmap = lightmapColor;
	out_WorldNorm = vec4(vOctNormal.x, vOctNormal.y, flDistanceToFragment, 0.0);
	out_Specular = vec4(0.0);
	out_Additive = vec4(0.0);

#else

#if defined(OIT_ALPHA_BLEND_ENABLED) || defined(ALPHA_BLEND_ENABLED)
	vec4 finalColor = CalcFog(baseColor) * lightmapColor;
#else
	vec4 finalColor = baseColor * lightmapColor;
#endif

	#if defined(OIT_ALPHA_BLEND_ENABLED) || defined(OIT_ADDITIVE_BLEND_ENABLED) 
		
		GatherFragment(finalColor);

	#endif

	out_Diffuse = finalColor;	

#endif
}