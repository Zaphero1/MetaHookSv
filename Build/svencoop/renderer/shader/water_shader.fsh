#version 410

#include "common.h"

uniform vec4 u_watercolor;
uniform vec3 u_depthfactor;
uniform vec4 u_fresnelfactor;
uniform float u_normfactor;
uniform float u_scale;
uniform float u_speed;

//Don't conflict with WSurfShader
#ifndef BINDLESS_ENABLED
uniform sampler2D baseTex;
uniform sampler2D normalTex;
uniform sampler2D reflectTex;
uniform sampler2D refractTex;
uniform sampler2D depthTex;
#endif

in vec4 v_projpos;
in vec3 v_worldpos;
in vec3 v_normal;
in vec2 v_diffusetexcoord;

layout(location = 0) out vec4 out_Diffuse;
layout(location = 1) out vec4 out_Lightmap;
layout(location = 2) out vec4 out_WorldNorm;
layout(location = 3) out vec4 out_Specular;
layout(location = 4) out vec4 out_Additive;

vec3 GenerateWorldPositionFromDepth(vec2 texCoord)
{
	#ifdef BINDLESS_ENABLED
		sampler2D depthTex = sampler2D(TextureSSBO[TEXTURE_SSBO_WATER_DEPTH]);
	#endif

	vec4 clipSpaceLocation;	
	clipSpaceLocation.xy = texCoord * 2.0-1.0;
	clipSpaceLocation.z  = texture2D(depthTex, texCoord).x * 2.0-1.0;
	clipSpaceLocation.w  = 1.0;
	vec4 homogenousLocation = SceneUBO.invViewMatrix * SceneUBO.invProjMatrix * clipSpaceLocation;
	return homogenousLocation.xyz / homogenousLocation.w;
}

void main()
{
	vec3 vNormal = normalize(v_normal);

	ClipPlaneTest(v_worldpos.xyz, vNormal.xyz);

	vec4 vFinalColor = vec4(0.0);
	
	float flWaterColorAlpha = clamp(u_watercolor.a, 0.0, 1.0);
	vec4 vWaterColor = vec4(u_watercolor.xyz, 1.0);

	vWaterColor = GammaToLinear(vWaterColor);

#ifdef LEGACY_ENABLED

	#ifdef BINDLESS_ENABLED
		sampler2D baseTex = sampler2D(TextureSSBO[TEXTURE_SSBO_WATER_BASE]);
	#endif

	vFinalColor.xyz = texture2D(baseTex, v_diffusetexcoord.xy).xyz;
	vFinalColor.a = flWaterColorAlpha;

	//The basetexture of water is in TexGamme Space and will need to convert to Linear Space
	vFinalColor = TexGammaToLinear(vFinalColor);

#else

	#ifdef BINDLESS_ENABLED
		sampler2D normalTex = sampler2D(TextureSSBO[TEXTURE_SSBO_WATER_NORMAL]);
	#endif

	//calculate the normal texcoord and sample the normal vector from texture
	vec2 vNormTexCoord1 = vec2(0.2, 0.15) * SceneUBO.time + v_diffusetexcoord.xy; 
	vec2 vNormTexCoord2 = vec2(-0.13, 0.11) * SceneUBO.time + v_diffusetexcoord.xy;
	vec2 vNormTexCoord3 = vec2(-0.14, -0.16) * SceneUBO.time + v_diffusetexcoord.xy;
	vec2 vNormTexCoord4 = vec2(0.17, 0.15) * SceneUBO.time + v_diffusetexcoord.xy;
	vec4 vNorm1 = texture2D(normalTex, vNormTexCoord1);
	vec4 vNorm2 = texture2D(normalTex, vNormTexCoord2);
	vec4 vNorm3 = texture2D(normalTex, vNormTexCoord3);
	vec4 vNorm4 = texture2D(normalTex, vNormTexCoord4);
	vNormal = vNorm1.xyz + vNorm2.xyz + vNorm3.xyz + vNorm4.xyz;
	vNormal = (vNormal * 0.25) * 2.0 - 1.0;

	//calculate texcoord
	vec2 vBaseTexCoord = v_projpos.xy / v_projpos.w * 0.5 + 0.5;

	vec3 vEyeVect = SceneUBO.viewpos.xyz - v_worldpos.xyz;
	float flHeight = abs(vEyeVect.z);
	float dist = length(vEyeVect);
	float sinX = flHeight / (dist + 0.001);
	float flFresnel = asin(sinX) / (0.5 * 3.14159);
	
	float flOffsetFactor = clamp(flFresnel, 0.0, 1.0) * u_normfactor;

	flFresnel = 1.0 - flFresnel;

	flFresnel = clamp(flFresnel + smoothstep(u_fresnelfactor.x, u_fresnelfactor.y, flHeight), 0.0, 1.0);

	vec2 vOffsetTexCoord = normalize(vNormal).xy * flOffsetFactor;

	#ifdef REFRACT_ENABLED

		#ifdef BINDLESS_ENABLED
			sampler2D refractTex = sampler2D(TextureSSBO[TEXTURE_SSBO_WATER_REFRACT]);
		#endif

		vec2 vRefractTexCoord = vBaseTexCoord + vOffsetTexCoord;
		vec4 vRefractColor = texture2D(refractTex, vRefractTexCoord);
		vRefractColor.a = 1.0;

		vRefractColor = mix(vRefractColor, vWaterColor, flWaterColorAlpha);

	#else

		vec4 vRefractColor = vWaterColor;

	#endif

	#ifdef UNDERWATER_ENABLED

		vFinalColor = vRefractColor;

	#else

		vec3 worldScene = vec3(0.0);

		#ifdef DEPTH_ENABLED

			worldScene = GenerateWorldPositionFromDepth(vBaseTexCoord);

			float flDiffDistance = distance(worldScene.xyz, SceneUBO.viewpos.xyz) - distance(v_worldpos.xyz, SceneUBO.viewpos.xyz);
			float flEdgeFeathering = clamp(flDiffDistance / u_depthfactor.z, 0.0, 1.0);
			vOffsetTexCoord *= (flEdgeFeathering * flEdgeFeathering);

		#endif

		float flWaterBlendAlpha = 1.0;

		#if defined(DEPTH_ENABLED) && defined(REFRACT_ENABLED)

			float flDiffZ = v_worldpos.z - worldScene.z;

			flWaterBlendAlpha = clamp( clamp( u_depthfactor.x * flDiffZ, 0.0, 1.0 ) + u_depthfactor.y, 0.0, 1.0 );

		#endif

		//Sample the reflect color (texcoord inverted)
		vec2 vBaseTexCoord2 = vec2(v_projpos.x, -v_projpos.y) / v_projpos.w * 0.5 + 0.5;

		#ifdef BINDLESS_ENABLED
			sampler2D reflectTex = sampler2D(TextureSSBO[TEXTURE_SSBO_WATER_REFLECT]);
		#endif

		vec2 vReflectTexCoord = vBaseTexCoord2 + vOffsetTexCoord;
		vec4 vReflectColor = texture2D(reflectTex, vReflectTexCoord);
		vReflectColor.a = 1.0;

		float flReflectFactor = clamp(pow(flFresnel, u_fresnelfactor.z), 0.0, u_fresnelfactor.w);

		vFinalColor = vRefractColor + vReflectColor * flReflectFactor;

		vFinalColor.a = flWaterBlendAlpha;

	#endif

#endif

#ifdef GBUFFER_ENABLED

	vec2 vOctNormal = UnitVectorToOctahedron(vNormal);

	float flDistanceToFragment = distance(v_worldpos.xyz, SceneUBO.viewpos.xyz);

	out_Diffuse = vFinalColor;
	out_Lightmap = vec4(1.0, 1.0, 1.0, 1.0);
	out_WorldNorm = vec4(vOctNormal.x, vOctNormal.y, flDistanceToFragment, 0.0);
	out_Specular = vec4(0.0);
	out_Additive = vec4(0.0);

#else

	vec4 color = CalcFog(vFinalColor);

	#if defined(OIT_ALPHA_BLEND_ENABLED) || defined(OIT_ADDITIVE_BLEND_ENABLED) 

		GatherFragment(color);

	#endif

	out_Diffuse = color;

#endif
}