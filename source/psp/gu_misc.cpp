/*
Copyright (C) 1996-1997 Id Software, Inc.
Copyright (C) 2007 Peter Mackay and Chris Swindle.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// r_misc.c

#include <pspgu.h>
#include <pspgum.h>

extern "C"
{
#include "../quakedef.h"
void VLight_ResetAnormTable();
}

void GL_InitTextureUsage ();

/*
==================
R_InitTextures
==================
*/
void	R_InitTextures (void)
{
	int		x,y, m;
	byte	*dest;

	GL_InitTextureUsage ();

// create a simple checkerboard texture for the default
	r_notexture_mip = static_cast<texture_t*>(Hunk_AllocName (sizeof(texture_t) + 16*16+8*8+4*4+2*2, "notexture"));

	r_notexture_mip->width = r_notexture_mip->height = 16;
	r_notexture_mip->offsets[0] = sizeof(texture_t);
	r_notexture_mip->offsets[1] = r_notexture_mip->offsets[0] + 16*16;
	r_notexture_mip->offsets[2] = r_notexture_mip->offsets[1] + 8*8;
	r_notexture_mip->offsets[3] = r_notexture_mip->offsets[2] + 4*4;

	for (m=0 ; m<4 ; m++)
	{
		dest = (byte *)r_notexture_mip + r_notexture_mip->offsets[m];
		for (y=0 ; y< (16>>m) ; y++)
			for (x=0 ; x< (16>>m) ; x++)
			{
				if (  (y< (8>>m) ) ^ (x< (8>>m) ) )
					*dest++ = 0;
				else
					*dest++ = 0xff;
			}
	}
}
/*
byte	dottexture[8][8] =
{
	{0,1,1,0,0,0,0,0},
	{1,1,1,1,0,0,0,0},
	{1,1,1,1,0,0,0,0},
	{0,1,1,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
};

void R_InitParticleTexture (void)
{
	int		x,y;
	byte	data[8][8][2];

	//
	// particle texture
	//

	for (x=0 ; x<8 ; x++)
	{
		for (y=0 ; y<8 ; y++)
		{
			data[y][x][0] = (dottexture[x][y])*0xF0 | (dottexture[x][y])*0x0F;
			data[y][x][1] = (dottexture[x][y])*0xF0 | (dottexture[x][y])*0x0F;
		}
	}
	particletexture = GL_LoadLightmapTexture ("_particle_", 8, 8, &data[0][0][0], 2, GU_LINEAR, qtrue);
}
*/
byte	dottexture2[16][16] =
{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
	{0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
	{0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
	{0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
	{0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
	{0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
	{0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

void R_InitParticleTexture (void)
{
	int		x,y;
	byte	data[16][16][2];

	//
	// particle texture
	//

	for (x=0 ; x<16 ; x++)
	{
		for (y=0 ; y<16 ; y++)
		{
			data[y][x][0] = (dottexture2[x][y])*0xF0 | (dottexture2[x][y])*0x0F;
			data[y][x][1] = (dottexture2[x][y])*0xF0 | (dottexture2[x][y])*0x0F;
		}
	}
	particletexture = GL_LoadLightmapTexture ("_particle_", 16, 16, &data[0][0][0], 2, GU_LINEAR, qtrue);
}

/*
===============
R_Envmap_f

Grab six views for environment mapping tests
===============
*/
void R_Envmap_f (void)
{
	byte	buffer[256*256*4];
//	char	name[1024];

	/*glDrawBuffer  (GL_FRONT);
	glReadBuffer  (GL_FRONT);*/
	envmap = qtrue;

	r_refdef.vrect.x = 0;
	r_refdef.vrect.y = 0;
	r_refdef.vrect.width = 256;
	r_refdef.vrect.height = 256;

	r_refdef.viewangles[0] = 0;
	r_refdef.viewangles[1] = 0;
	r_refdef.viewangles[2] = 0;
	GL_BeginRendering (&glx, &gly, &glwidth, &glheight);
	R_RenderView ();
	/*glReadPixels (0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);*/
	COM_WriteFile ("env0.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[1] = 90;
	GL_BeginRendering (&glx, &gly, &glwidth, &glheight);
	R_RenderView ();
	/*glReadPixels (0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);*/
	COM_WriteFile ("env1.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[1] = 180;
	GL_BeginRendering (&glx, &gly, &glwidth, &glheight);
	R_RenderView ();
	/*glReadPixels (0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);*/
	COM_WriteFile ("env2.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[1] = 270;
	GL_BeginRendering (&glx, &gly, &glwidth, &glheight);
	R_RenderView ();
	/*glReadPixels (0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);*/
	COM_WriteFile ("env3.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[0] = -90;
	r_refdef.viewangles[1] = 0;
	GL_BeginRendering (&glx, &gly, &glwidth, &glheight);
	R_RenderView ();
	/*glReadPixels (0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);*/
	COM_WriteFile ("env4.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[0] = 90;
	r_refdef.viewangles[1] = 0;
	GL_BeginRendering (&glx, &gly, &glwidth, &glheight);
	R_RenderView ();
	/*glReadPixels (0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);*/
	COM_WriteFile ("env5.rgb", buffer, sizeof(buffer));

	envmap = qfalse;
	/*glDrawBuffer  (GL_BACK);
	glReadBuffer  (GL_BACK);*/
	GL_EndRendering ();
}

/*
===============
R_Init
===============
*/
void R_Init (void)
{
//	extern byte *hunk_base;
	/*extern cvar_t gl_finish;*/

	Cmd_AddCommand ("timerefresh", R_TimeRefresh_f);
	Cmd_AddCommand ("envmap", R_Envmap_f);
	Cmd_AddCommand ("pointfile", R_ReadPointFile_f);

//	Cmd_AddCommand ("fog", R_Fog_f);

    Cvar_RegisterVariable (&r_showtris, NULL);
    Cvar_RegisterVariable (&r_skyfog, NULL);
	Cvar_RegisterVariable (&r_skyclip, NULL);
	Cvar_RegisterVariable (&r_menufade, NULL);
	Cvar_RegisterVariable (&r_norefresh, NULL);
	Cvar_RegisterVariable (&r_lightmap, NULL);
	Cvar_RegisterVariable (&r_fullbright, NULL);
	Cvar_RegisterVariable (&r_drawentities, NULL);
	Cvar_RegisterVariable (&r_drawviewmodel, NULL);
	Cvar_RegisterVariable (&r_shadows, NULL);
	Cvar_RegisterVariable (&r_mirroralpha, NULL);
	Cvar_RegisterVariable (&r_glassalpha, NULL);
	Cvar_RegisterVariable (&r_wateralpha, NULL);
	Cvar_RegisterVariable (&r_dynamic, NULL);
	Cvar_RegisterVariable (&r_novis, NULL);
	Cvar_RegisterVariable (&r_speeds, NULL);
	Cvar_RegisterVariable (&r_tex_scale_down, NULL);
	Cvar_RegisterVariable (&r_particles_simple, NULL);
    Cvar_RegisterVariable (&r_vsync, NULL);
    Cvar_RegisterVariable (&r_dithering, NULL);
    Cvar_RegisterVariable (&r_test, NULL);
	Cvar_RegisterVariable (&r_mipmaps, NULL);
	Cvar_RegisterVariable (&r_mipmaps_func, NULL);
	Cvar_RegisterVariable (&r_mipmaps_bias, NULL);
    Cvar_RegisterVariable (&r_antialias, NULL);
    Cvar_RegisterVariable (&r_interpolate_animation, NULL);
    Cvar_RegisterVariable (&r_interpolate_transform, NULL);
    Cvar_RegisterVariable (&r_model_contrast, NULL);
    Cvar_RegisterVariable (&r_model_brightness, NULL);
	Cvar_RegisterVariable (&r_skybox, NULL);

	Cvar_RegisterVariable (&gl_keeptjunctions, NULL);

	// RIOT - Vertex lighting

    Cvar_RegisterVariable (&vlight, NULL);
    Cvar_RegisterVariable (&vlight_pitch, NULL);
    Cvar_RegisterVariable (&vlight_yaw, NULL);
    Cvar_RegisterVariable (&vlight_highcut, NULL);
    Cvar_RegisterVariable (&vlight_lowcut, NULL);

/*
	Cvar_RegisterVariable (&gl_finish, NULL);
	Cvar_RegisterVariable (&gl_clear, NULL);
	Cvar_RegisterVariable (&gl_texsort, NULL);

 	if (gl_mtexable)
		Cvar_SetValueByRef (&gl_texsort, 0.0);

	Cvar_RegisterVariable (&gl_cull, NULL);
	Cvar_RegisterVariable (&gl_smoothmodels, NULL);
	Cvar_RegisterVariable (&gl_affinemodels, NULL);
	Cvar_RegisterVariable (&gl_polyblend, NULL);
	Cvar_RegisterVariable (&gl_flashblend, NULL);
	Cvar_RegisterVariable (&gl_playermip, NULL);
	Cvar_RegisterVariable (&gl_nocolors, NULL);

	Cvar_RegisterVariable (&gl_reporttjunctions, NULL);

	Cvar_RegisterVariable (&gl_doubleeyes, NULL);
*/
    if(vlight.value)
        VLight_ResetAnormTable();

	R_InitParticles ();
	R_InitParticleTexture ();

    Sky_Init (); //johnfitz
	//Fog_Init (); //johnfitz

	R_CalculatePspVertexNormals ();

#ifdef GLTEST
	Test_Init ();
#endif

	/*
	playertextures = texture_extension_number;
	texture_extension_number += 16;
	*/


}

/*
===============
R_TranslatePlayerSkin

Translates a skin texture by the per-player color lookup
===============
*/
void R_TranslatePlayerSkin (int playernum)
{
	int		top, bottom;
	byte	translate[256];
	unsigned	translate32[256];
	int		i, j, s;
	model_t	*model;
	aliashdr_t *paliashdr;
	byte	*original;
	unsigned	pixels[512*256];//, *out;
	unsigned	scaled_width, scaled_height;
	int			inwidth, inheight;
	byte		*inrow;
	unsigned	frac, fracstep;
//	extern	byte		**player_8bit_texels_tbl;

	/*GL_DisableMultitexture();*/

	top = cl.scores[playernum].colors & 0xf0;
	bottom = (cl.scores[playernum].colors &15)<<4;

	for (i=0 ; i<256 ; i++)
		translate[i] = i;

	for (i=0 ; i<16 ; i++)
	{
		if (top < 128)	// the artists made some backwards ranges.  sigh.
			translate[TOP_RANGE+i] = top+i;
		else
			translate[TOP_RANGE+i] = top+15-i;

		if (bottom < 128)
			translate[BOTTOM_RANGE+i] = bottom+i;
		else
			translate[BOTTOM_RANGE+i] = bottom+15-i;
	}

	//
	// locate the original skin pixels
	//
	currententity = &cl_entities[1+playernum];
	model = currententity->model;
	if (!model)
		return;		// player doesn't have a model yet
	if (model->type != mod_alias)
		return; // only translate skins on alias models

	paliashdr = (aliashdr_t *)Mod_Extradata (model);
	s = paliashdr->skinwidth * paliashdr->skinheight;
	if (currententity->skinnum < 0 || currententity->skinnum >= paliashdr->numskins) {
		Con_Printf("(%d): Invalid player skin #%d\n", playernum, currententity->skinnum);
		original = (byte *)paliashdr + paliashdr->texels[0];
	} else
		original = (byte *)paliashdr + paliashdr->texels[currententity->skinnum];
	if (s & 3)
		Sys_Error ("R_TranslateSkin: s&3");

	inwidth = paliashdr->skinwidth;
	inheight = paliashdr->skinheight;

	// because this happens during gameplay, do it fast
	// instead of sending it through gl_upload 8
    /*GL_Bind(playertextures + playernum);*/

#if 0
	byte	translated[320*200];

	for (i=0 ; i<s ; i+=4)
	{
		translated[i] = translate[original[i]];
		translated[i+1] = translate[original[i+1]];
		translated[i+2] = translate[original[i+2]];
		translated[i+3] = translate[original[i+3]];
	}


	// don't mipmap these, because it takes too long
	GL_Upload8 (translated, paliashdr->skinwidth, paliashdr->skinheight, qfalse, qfalse, qtrue);
#else
	scaled_width = 512;
	scaled_height = 256;

	/*if (VID_Is8bit())*/
	{ // 8bit texture upload
		byte *out2;

		out2 = (byte *)pixels;
		memset(pixels, 0, sizeof(pixels));
		fracstep = inwidth*0x10000/scaled_width;
		for (i=0 ; i<(signed)scaled_height ; i++, out2 += scaled_width)
		{
			inrow = original + inwidth*(i*inheight/scaled_height);
			frac = fracstep >> 1;
			for (j=0 ; j<(signed)scaled_width ; j+=4)
			{
				out2[j] = translate[inrow[frac>>16]];
				frac += fracstep;
				out2[j+1] = translate[inrow[frac>>16]];
				frac += fracstep;
				out2[j+2] = translate[inrow[frac>>16]];
				frac += fracstep;
				out2[j+3] = translate[inrow[frac>>16]];
				frac += fracstep;
			}
		}

		/*GL_Upload8((byte *)pixels, scaled_width, scaled_height, qfalse, qfalse);*/
		return;
	}


	for (i=0 ; i<256 ; i++)
		translate32[i] = d_8to24table[translate[i]];
/*
	out = pixels;
	fracstep = inwidth*0x10000/scaled_width;
	for (i=0 ; i<scaled_height ; i++, out += scaled_width)
	{
		inrow = original + inwidth*(i*inheight/scaled_height);
		frac = fracstep >> 1;
		for (j=0 ; j<scaled_width ; j+=4)
		{
			out[j] = translate32[inrow[frac>>16]];
			frac += fracstep;
			out[j+1] = translate32[inrow[frac>>16]];
			frac += fracstep;
			out[j+2] = translate32[inrow[frac>>16]];
			frac += fracstep;
			out[j+3] = translate32[inrow[frac>>16]];
			frac += fracstep;
		}
	}
	glTexImage2D (GL_TEXTURE_2D, 0, gl_solid_format, scaled_width, scaled_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	*/
#endif

}


/*
===============
R_NewMap
===============
*/
void R_NewMap (void)
{
	int		i;

	for (i=0 ; i<256 ; i++)
		d_lightstylevalue[i] = 264;		// normal light value

	memset (&r_worldentity, 0, sizeof(r_worldentity));
	r_worldentity.model = cl.worldmodel;

// clear out efrags in case the level hasn't been reloaded
// FIXME: is this one short?
	for (i=0 ; i<cl.worldmodel->numleafs ; i++)
		cl.worldmodel->leafs[i].efrags = NULL;

	r_viewleaf = NULL;
	R_ClearParticles ();

	GL_BuildLightmaps ();

	Sky_NewMap (); //johnfitz -- skybox in worldspawn
	//Fog_NewMap (); //johnfitz -- global fog in worldspawn

    // Reset these back to normal.
#ifdef SUPPORTS_KUROK
    if(IS_KUROK)
        Cbuf_AddText ("viewsize 120\n chase_active 0\n");
#endif

	// identify sky texture
	skytexturenum = -1;
	mirrortexturenum = -1;

	for (i=0 ; i<cl.worldmodel->numtextures ; i++)
	{
		if (!cl.worldmodel->textures[i])
			continue;
		if (!strncmp(cl.worldmodel->textures[i]->name,"sky",3) )
			skytexturenum = i;
#ifdef SUPPORTS_KUROK
	  	if (IS_KUROK)
		{
			if (!strncmp(cl.worldmodel->textures[i]->name,"glass",5) )
				mirrortexturenum = i;
		}
#endif
	  	else
		{
			if (!strncmp(cl.worldmodel->textures[i]->name,"window02_1",10) )
				mirrortexturenum = i;
		}
 		cl.worldmodel->textures[i]->texturechain = NULL;
	}
}


/*
====================
R_TimeRefresh_f

For program optimization
====================
*/
void R_TimeRefresh_f (void)
{
	int			i;
	double		start, stop, time;
//	int			startangle;
//	vrect_t		vr;


	start = Sys_DoubleTime ();
	for (i=0 ; i<128 ; i++)
	{
		r_refdef.viewangles[1] = i/128.0*360.0;
		R_RenderView ();
	}

	stop = Sys_DoubleTime ();
	time = stop-start;
	if (time > 0)
		Con_Printf ("%f seconds (%f fps)\n", time, 128/time);

	GL_EndRendering ();
}

void D_FlushCaches (void)
{
}


