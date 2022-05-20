#include "gl_local.h"
#include "gl_water.h"

colorVec *gWaterColor = NULL;
cshift_t *cshift_water = NULL;
int *gSkyTexNumber = NULL;
int *r_loading_skybox = NULL;

void R_DrawWaterVBO(water_vbo_t *WaterVBOCache, cl_entity_t *ent)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, WaterVBOCache->hEBO);

	if (r_draw_opaque)
	{
		glEnable(GL_STENCIL_TEST);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	}

	R_SetRenderMode(ent);
	R_SetGBufferMask(GBUFFER_MASK_ALL);

	bool bIsAboveWater = (WaterVBOCache->normal[2] > 0) && R_IsAboveWater(WaterVBOCache) ? true : false;

	float color[4];
	color[0] = WaterVBOCache->color.r / 255.0f;
	color[1] = WaterVBOCache->color.g / 255.0f;
	color[2] = WaterVBOCache->color.b / 255.0f;
	color[3] = 1;

	if((*currententity)->curstate.rendermode == kRenderTransTexture)
		color[3] = (*r_blend);

	if (WaterVBOCache->level >= WATER_LEVEL_REFLECT_SKYBOX && WaterVBOCache->level <= WATER_LEVEL_REFLECT_ENTITY && r_water->value)
	{
		if (!refractmap_ready)
		{
			if (drawgbuffer)
			{
				R_BlitGBufferToFrameBuffer(&s_WaterFBO);

				//Must restore VBO and EBO
				glBindBuffer(GL_ARRAY_BUFFER, r_wsurf.hSceneVBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, WaterVBOCache->hEBO);

				glBindFramebuffer(GL_FRAMEBUFFER, s_GBufferFBO.s_hBackBufferFBO);
			}
			else
			{
				GL_BlitFrameBufferToFrameBufferColorDepth(&s_BackBufferFBO, &s_WaterFBO);

				glBindFramebuffer(GL_FRAMEBUFFER, s_BackBufferFBO.s_hBackBufferFBO);
			}

			refractmap_ready = true;
		}

		int programState = 0;

		if (bIsAboveWater)
			programState |= WATER_DEPTH_ENABLED;

		if (r_water_forcetrans->value)
		{
			programState |= WATER_REFRACT_ENABLED;

			if (color[3] > WaterVBOCache->maxtrans)
				color[3] = WaterVBOCache->maxtrans;
		}
		else
		{
			if ((*currententity)->curstate.rendermode == kRenderTransTexture || (*currententity)->curstate.rendermode == kRenderTransAdd)
			{
				programState |= WATER_REFRACT_ENABLED;

				if (color[3] > WaterVBOCache->maxtrans)
					color[3] = WaterVBOCache->maxtrans;
			}
		}

		if (!bIsAboveWater)
		{
			programState |= WATER_UNDERWATER_ENABLED;
		}
		else
		{
			if (!drawgbuffer && r_fog_mode == GL_LINEAR)
			{
				programState |= WATER_LINEAR_FOG_ENABLED;
			}
			else if (!drawgbuffer && r_fog_mode == GL_EXP)
			{
				programState |= WATER_EXP_FOG_ENABLED;
			}
			else if (!drawgbuffer && r_fog_mode == GL_EXP2)
			{
				programState |= WATER_EXP2_FOG_ENABLED;
			}
		}

		if (drawgbuffer)
		{
			programState |= WATER_GBUFFER_ENABLED;
		}

		if (r_draw_oitblend)
		{
			programState |= WATER_OIT_ALPHA_BLEND_ENABLED;
		}

		water_program_t prog = { 0 };
		R_UseWaterProgram(programState, &prog);

		if (prog.u_watercolor != -1)
			glUniform4f(prog.u_watercolor, color[0], color[1], color[2], color[3]);

		if (prog.u_depthfactor != -1)
			glUniform3f(prog.u_depthfactor, WaterVBOCache->depthfactor[0], WaterVBOCache->depthfactor[1], WaterVBOCache->depthfactor[2]);

		if (prog.u_fresnelfactor != -1)
			glUniform4f(prog.u_fresnelfactor, WaterVBOCache->fresnelfactor[0], WaterVBOCache->fresnelfactor[1], WaterVBOCache->fresnelfactor[2], WaterVBOCache->fresnelfactor[3]);

		if (prog.u_normfactor != -1)
			glUniform1f(prog.u_normfactor, WaterVBOCache->normfactor);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		R_SetGBufferBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, WaterVBOCache->normalmap);

		glActiveTexture(GL_TEXTURE3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, WaterVBOCache->reflectmap);

		glActiveTexture(GL_TEXTURE4);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, s_WaterFBO.s_hBackBufferTex);

		glActiveTexture(GL_TEXTURE5);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, s_WaterFBO.s_hBackBufferDepthTex);

		glDrawElements(GL_POLYGON,  WaterVBOCache->vIndicesBuffer.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

		r_wsurf_drawcall++;
		r_wsurf_polys += WaterVBOCache->iPolyCount;

		glActiveTexture(GL_TEXTURE5);
		glDisable(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE4);
		glDisable(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE3);
		glDisable(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_2D);

		glActiveTexture(*oldtarget);

		glDisable(GL_BLEND);
	}
	else if(WaterVBOCache->level == WATER_LEVEL_LEGACY_RIPPLE && r_water->value)
	{
		int programState = WATER_LEGACY_ENABLED;

		if (!bIsAboveWater)
		{
		
		}
		else
		{
			if (!drawgbuffer && r_fog_mode == GL_LINEAR)
			{
				programState |= WATER_LINEAR_FOG_ENABLED;
			}
			else if (!drawgbuffer && r_fog_mode == GL_EXP)
			{
				programState |= WATER_EXP_FOG_ENABLED;
			}
			else if (!drawgbuffer && r_fog_mode == GL_EXP2)
			{
				programState |= WATER_EXP2_FOG_ENABLED;
			}
		}

		if (drawgbuffer)
		{
			programState |= WATER_GBUFFER_ENABLED;
		}

		if (r_draw_oitblend)
		{
			if ((*currententity)->curstate.rendermode == kRenderTransAdd)
				programState |= WATER_OIT_ADDITIVE_BLEND_ENABLED;
			else
				programState |= WATER_OIT_ALPHA_BLEND_ENABLED;
		}

		water_program_t prog = { 0 };
		R_UseWaterProgram(programState, &prog);

		if (prog.u_watercolor != -1)
			glUniform4f(prog.u_watercolor, color[0], color[1], color[2], color[3]);

		if (prog.u_scale != -1)
			glUniform1f(prog.u_scale, 0);

		if (prog.u_speed != -1)
			glUniform1f(prog.u_speed, 0);

		GL_Bind(WaterVBOCache->ripplemap);
		
		glDrawElements(GL_POLYGON, WaterVBOCache->vIndicesBuffer.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

		r_wsurf_drawcall++;
		r_wsurf_polys += WaterVBOCache->iPolyCount;
	}
	else
	{
		float scale;

		if (bIsAboveWater)
			scale = (*currententity)->curstate.scale;
		else
			scale = -(*currententity)->curstate.scale;

		int programState = WATER_LEGACY_ENABLED;

		if (!bIsAboveWater)
		{

		}
		else
		{
			if (!drawgbuffer && r_fog_mode == GL_LINEAR)
			{
				programState |= WATER_LINEAR_FOG_ENABLED;
			}
			else if (!drawgbuffer && r_fog_mode == GL_EXP)
			{
				programState |= WATER_EXP_FOG_ENABLED;
			}
			else if (!drawgbuffer && r_fog_mode == GL_EXP2)
			{
				programState |= WATER_EXP2_FOG_ENABLED;
			}
		}

		if (drawgbuffer)
		{
			programState |= WATER_GBUFFER_ENABLED;
		}

		if (r_draw_oitblend)
		{
			if ((*currententity)->curstate.rendermode == kRenderTransAdd)
				programState |= WATER_OIT_ADDITIVE_BLEND_ENABLED;
			else
				programState |= WATER_OIT_ALPHA_BLEND_ENABLED;
		}

		water_program_t prog = { 0 };
		R_UseWaterProgram(programState, &prog);

		if (prog.u_watercolor != -1)
			glUniform4f(prog.u_watercolor, color[0], color[1], color[2], color[3]);

		if (prog.u_scale != -1)
			glUniform1f(prog.u_scale, scale);

		if (prog.u_speed != -1)
			glUniform1f(prog.u_speed, WaterVBOCache->speedrate);

		GL_Bind(WaterVBOCache->texture->gl_texturenum);

		glDrawElements(GL_POLYGON, WaterVBOCache->vIndicesBuffer.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

		r_wsurf_drawcall++;
		r_wsurf_polys += WaterVBOCache->iPolyCount;
	}

	//GL_UseProgram(0);

	if (r_draw_opaque)
	{
		glDisable(GL_STENCIL_TEST);
	}
	//glDisable(GL_BLEND);
}

void R_DrawWaters(cl_entity_t *ent)
{
	if (r_draw_reflectview)
		return;

	if (g_iNumRenderWaterVBOCache)
	{
		static glprofile_t profile_DrawWaters;
		GL_BeginProfile(&profile_DrawWaters, "R_DrawWaters");

		glBindBuffer(GL_ARRAY_BUFFER, r_wsurf.hSceneVBO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(brushvertex_t), OFFSET(brushvertex_t, pos));
		glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(brushvertex_t), OFFSET(brushvertex_t, normal));
		glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(brushvertex_t), OFFSET(brushvertex_t, s_tangent));
		glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(brushvertex_t), OFFSET(brushvertex_t, t_tangent));
		glVertexAttribPointer(4, 3, GL_FLOAT, false, sizeof(brushvertex_t), OFFSET(brushvertex_t, texcoord));
		glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
		//glDisable(GL_CULL_FACE);

		for (int i = 0; i < g_iNumRenderWaterVBOCache; ++i)
		{
			R_DrawWaterVBO(g_RenderWaterVBOCache[i], ent);
		}

		//glEnable(GL_CULL_FACE);
		glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		g_iNumRenderWaterVBOCache = 0;

		GL_EndProfile(&profile_DrawWaters);
	}
}

void EmitWaterPolys(msurface_t *fa, int direction)
{
	if (r_draw_reflectview)
		return;

	auto pSourcePalette = fa->texinfo->texture->pPal;
	gWaterColor->r = pSourcePalette[9];
	gWaterColor->g = pSourcePalette[10];
	gWaterColor->b = pSourcePalette[11];
	cshift_water->destcolor[0] = pSourcePalette[9];
	cshift_water->destcolor[1] = pSourcePalette[10];
	cshift_water->destcolor[2] = pSourcePalette[11];
	cshift_water->percent = pSourcePalette[12];

	if (gWaterColor->r == 0 && gWaterColor->g == 0 && gWaterColor->b == 0)
	{
		gWaterColor->r = pSourcePalette[0];
		gWaterColor->g = pSourcePalette[1];
		gWaterColor->b = pSourcePalette[2];
	}
	
	auto VBOCache = R_PrepareWaterVBO((*currententity), fa, direction);

	if (g_iNumRenderWaterVBOCache == 512)
	{
		g_pMetaHookAPI->SysError("EmitWaterPolys: Too many waters!");
	}

	if (VBOCache->framecount != (*r_framecount))
	{
		VBOCache->framecount = (*r_framecount);
		g_RenderWaterVBOCache[g_iNumRenderWaterVBOCache] = VBOCache;
		g_iNumRenderWaterVBOCache++;
	}
}

void R_DrawSkyBox(void)
{
	if (CL_IsDevOverviewMode())
		return;

	if (!gSkyTexNumber[0])
		return;

	static glprofile_t profile_DrawSkyBox;
	GL_BeginProfile(&profile_DrawSkyBox, "R_DrawSkyBox");

	glDisable(GL_BLEND);
	glDepthMask(0);

	int WSurfProgramState = WSURF_DIFFUSE_ENABLED | WSURF_SKYBOX_ENABLED;

	if (r_draw_reflectview)
	{
		WSurfProgramState |= WSURF_CLIP_WATER_ENABLED;
	}
	else if (g_bPortalClipPlaneEnabled[0])
	{
		WSurfProgramState |= WSURF_CLIP_ENABLED;
	}

	if (r_wsurf_sky_fog->value)
	{
		if (!drawgbuffer && r_fog_mode == GL_LINEAR)
		{
			WSurfProgramState |= WSURF_LINEAR_FOG_ENABLED;
		}
		else if (!drawgbuffer && r_fog_mode == GL_EXP)
		{
			WSurfProgramState |= WSURF_EXP_FOG_ENABLED;
		}
		else if (!drawgbuffer && r_fog_mode == GL_EXP2)
		{
			WSurfProgramState |= WSURF_EXP2_FOG_ENABLED;
		}
	}

	if (drawgbuffer)
	{
		WSurfProgramState |= WSURF_GBUFFER_ENABLED;
	}

	wsurf_program_t prog = { 0 };
	R_UseWSurfProgram(WSurfProgramState, &prog);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (bUseBindless)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BINDING_POINT_SKYBOX_SSBO, r_wsurf.hSkyboxSSBO);
		glDrawArrays(GL_QUADS, 0, 4 * 6);
	}
	else
	{
		if (g_iEngineType == ENGINE_SVENGINE)
		{
			for (int i = 0; i < 6; ++i)
			{
				GL_Bind(gSkyTexNumber[i]);
				glDrawArrays(GL_QUADS, 4 * i, 4);
			}
		}
		else
		{
			const int skytexorder[6] = { 0, 2, 1, 3, 4, 5 };
			for (int i = 0; i < 6; ++i)
			{
				GL_Bind(gSkyTexNumber[skytexorder[i]]);
				glDrawArrays(GL_QUADS, 4 * i, 4);
			}
		}
	}

	GL_UseProgram(0);

	glDepthMask(1);

	GL_EndProfile(&profile_DrawSkyBox);
}