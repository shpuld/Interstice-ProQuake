/*
Copyright (C) 1996-1997 Id Software, Inc.

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
// common.c -- misc functions used in client and server

#include "quakedef.h"

#define NUM_SAFE_ARGVS  7

static char     *largv[MAX_NUM_ARGVS + NUM_SAFE_ARGVS + 1];
static char     *argvdummy = " ";

static char     *safeargvs[NUM_SAFE_ARGVS] = {"-stdvid", "-nolan", "-nosound", "-nocdaudio", "-nojoy", "-nomouse", "-dibonly"};

cvar_t  registered = {"registered","0"};
cvar_t  cmdline = {"cmdline","0", false, true};

qboolean        com_modified;   // set true if using non-id files

static qboolean		proghack;

static int             static_registered = 1;  // only for startup check, then set

qboolean		msg_suppress_1 = 0;

void COM_InitFilesystem (void);

// if a packfile directory differs from this, it is assumed to be hacked
#define PAK0_COUNT              339
#define PAK0_CRC                32981

char	com_token[1024];
int		com_argc;
char	**com_argv;

#define CMDLINE_LENGTH	256
static char	com_cmdline[CMDLINE_LENGTH];

int compat_gametype;

/*

All of Quake's data access is through a hierchal file system, but the contents
of the file system can be transparently merged from several sources.

The "base directory" is the path to the directory holding the quake.exe and all
game directories. The sys_* files pass this to host_init in quakeparms_t->basedir.
This can be overridden with the "-basedir" command line parm to allow code
debugging in a different directory. The base directory is only used during
filesystem initialization.

The "game directory" is the first tree on the search path and directory that all
generated files (savegames, screenshots, demos, config files) will be saved to.
This can be overridden with the "-game" command line parameter.
The game directory can never be changed while quake is executing.
This is a precacution against having a malicious server instruct clients to
write files over areas they shouldn't.

The "cache directory" is only used during development to save network bandwidth,
especially over ISDN / T1 lines.  If there is a cache directory specified, when
a file is found by the normal search path, it will be mirrored into the cache
directory, then opened there.


FIXME:
The file "parms.txt" will be read out of the game directory and appended to the
current command line arguments to allow different games to initialize startup
parms differently. This could be used to add a "-sspeed 22050" for the high
quality sound edition. Because they are added at the end, they will not override
an explicit setting on the original command line.

*/
// Special command line options

qboolean		mod_deeplair = false;		// Deep lair mod: alternate HUD
qboolean		mod_conhide  = false;		// Conceal the console more
qboolean		mod_nosoundwarn = false;	// Don't warn about missing sounds


// this graphic needs to be in the pak file to use registered features
static unsigned short pop[] =
{
 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
,0x0000,0x0000,0x6600,0x0000,0x0000,0x0000,0x6600,0x0000
,0x0000,0x0066,0x0000,0x0000,0x0000,0x0000,0x0067,0x0000
,0x0000,0x6665,0x0000,0x0000,0x0000,0x0000,0x0065,0x6600
,0x0063,0x6561,0x0000,0x0000,0x0000,0x0000,0x0061,0x6563
,0x0064,0x6561,0x0000,0x0000,0x0000,0x0000,0x0061,0x6564
,0x0064,0x6564,0x0000,0x6469,0x6969,0x6400,0x0064,0x6564
,0x0063,0x6568,0x6200,0x0064,0x6864,0x0000,0x6268,0x6563
,0x0000,0x6567,0x6963,0x0064,0x6764,0x0063,0x6967,0x6500
,0x0000,0x6266,0x6769,0x6a68,0x6768,0x6a69,0x6766,0x6200
,0x0000,0x0062,0x6566,0x6666,0x6666,0x6666,0x6562,0x0000
,0x0000,0x0000,0x0062,0x6364,0x6664,0x6362,0x0000,0x0000
,0x0000,0x0000,0x0000,0x0062,0x6662,0x0000,0x0000,0x0000
,0x0000,0x0000,0x0000,0x0061,0x6661,0x0000,0x0000,0x0000
,0x0000,0x0000,0x0000,0x0000,0x6500,0x0000,0x0000,0x0000
,0x0000,0x0000,0x0000,0x0000,0x6400,0x0000,0x0000,0x0000
};

/*


All of Quake's data access is through a hierchal file system, but the contents of the file system can be transparently merged from several sources.

The "base directory" is the path to the directory holding the quake.exe and all game directories.  The sys_* files pass this to host_init in quakeparms_t->basedir.  This can be overridden with the "-basedir" command line parm to allow code debugging in a different directory.  The base directory is
only used during filesystem initialization.

The "game directory" is the first tree on the search path and directory that all generated files (savegames, screenshots, demos, config files) will be saved to.  This can be overridden with the "-game" command line parameter.  The game directory can never be changed while quake is executing.  This is a precacution against having a malicious server instruct clients to write files over areas they shouldn't.

The "cache directory" is only used during development to save network bandwidth, especially over ISDN / T1 lines.  If there is a cache directory
specified, when a file is found by the normal search path, it will be mirrored
into the cache directory, then opened there.



FIXME:
The file "parms.txt" will be read out of the game directory and appended to the current command line arguments to allow different games to initialize startup parms differently.  This could be used to add a "-sspeed 22050" for the high quality sound edition.  Because they are added at the end, they will not override an explicit setting on the original command line.

*/


#ifdef BUILD_MP3_VERSION

// fast safe printbuffers courtesy of lord havoc.
int va_snprintf (char *function, char *buffer, size_t buffersize, const char *format, ...)
{
	va_list args;
	int		result;

	va_start (args, format);
	result = va_vsnprintf (function, buffer, buffersize, format, args);
	va_end (args);

	return result;
}

int va_vsnprintf (char *function, char *buffer, size_t buffersize, const char *format, va_list args)
{
	size_t result;

	result = vsnprintf (buffer, buffersize, format, args);

	if (result < 0 || result >= buffersize)
	{
		static qboolean inside;

		// Beware recursion here
		if (!inside)
		{
			inside = true;
			Con_SafePrintf ("%s: excessive string length, max = %d\n", function, buffersize);
		}
		inside = false;
		buffer[buffersize - 1] = '\0';
		return -1;
	}
	return result;
}
#endif


/*
================
COM_Quakebar
================
*/
char *COM_Quakebar (int len)
{
	static char bar[42];
	int i;

	len = QMIN(len, sizeof(bar) - 2);

	bar[0] = '\36';
	for (i = 1; i < len - 1; i++)
		bar[i] = '\36';
	bar[len-1] = '\36';
	bar[len] = 0;

	return bar;
}


/*
// Always ensures string is terminated

void Q_strncpyz (char *dest, char *src, size_t size)
{
	strncpy (dest, src, size - 1);
	dest[size-1] = 0;
}

// Always ensures string is terminated
void Q_snprintfz (char *dest, size_t size, char *fmt, ...)
{
	va_list		argptr;

	va_start (argptr, fmt);
	vsnprintf (dest, size, fmt, argptr);
	va_end (argptr);

	dest[size - 1] = 0;
}
*/

/*
============================================================================

					BYTE ORDER FUNCTIONS

============================================================================
*/

qboolean        bigendien;

short   (*BigShort) (short l);
int     (*BigLong) (int l);
float   (*BigFloat) (float l);

short   ShortSwap (short l)
{
	byte    b1,b2;

	b1 = l&255;
	b2 = (l>>8)&255;

	return (b1<<8) + b2;
}

int    LongSwap (int l)
{
	byte    b1,b2,b3,b4;

	b1 = l&255;
	b2 = (l>>8)&255;
	b3 = (l>>16)&255;
	b4 = (l>>24)&255;

	return ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4;
}

float FloatSwap (float f)
{
	union
	{
		float   f;
		byte    b[4];
	} dat1, dat2;


	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
}


/*
==============================================================================

			MESSAGE IO FUNCTIONS

Handles byte ordering and avoids alignment errors
==============================================================================
*/

// writing functions

void MSG_WriteChar (sizebuf_t *sb, int c)
{
	byte    *buf;

#ifdef PARANOID
	if (c < -128 || c > 127)
		Sys_Error ("MSG_WriteChar: range error");
#endif

	buf = SZ_GetSpace (sb, 1);
	buf[0] = c;
}

void MSG_WriteByte (sizebuf_t *sb, int c)
{
	byte    *buf;

#ifdef PARANOID
	if (c < 0 || c > 255)
		Sys_Error ("MSG_WriteByte: range error");
#endif

	buf = SZ_GetSpace (sb, 1);
	buf[0] = c;
}

void MSG_WriteShort (sizebuf_t *sb, int c)
{
	byte    *buf;

#ifdef PARANOID
	if (c < ((short)0x8000) || c > (short)0x7fff)
		Sys_Error ("MSG_WriteShort: range error");
#endif

	buf = SZ_GetSpace (sb, 2);
	buf[0] = c&0xff;
	buf[1] = c>>8;
}

void MSG_WriteLong (sizebuf_t *sb, int c)
{
	byte    *buf;

	buf = SZ_GetSpace (sb, 4);
	buf[0] = c&0xff;
	buf[1] = (c>>8)&0xff;
	buf[2] = (c>>16)&0xff;
	buf[3] = c>>24;
}

void MSG_WriteFloat (sizebuf_t *sb, float f)
{
	union
	{
		float   f;
		int     l;
	} dat;


	dat.f = f;
	dat.l = LittleLong (dat.l);

	SZ_Write (sb, &dat.l, 4);
}

void MSG_WriteString (sizebuf_t *sb, char *s)
{
	if (!s)
		SZ_Write (sb, "", 1);
	else
		SZ_Write (sb, s, strlen(s)+1);
}

void MSG_WriteCoord (sizebuf_t *sb, float f)
{
	MSG_WriteShort (sb, (int)(f*8));
}

void MSG_WriteAngle (sizebuf_t *sb, float f)
{
	//MSG_WriteByte (sb, (int)floor(f * 256 / 360 + 0.5) & 255); // Baker 3.76 - LordHavoc precision aiming fix
	MSG_WriteByte (sb, ((int)f*256/360) & 255);
}

// joe: added from FuhQuake
void R_PreMapLoad (char *mapname)
{
	extern cvar_t host_mapname;
	Cvar_SetStringByRef (&host_mapname, mapname);
}


// reading functions
int                     msg_readcount;
qboolean        msg_badread;

void MSG_BeginReading (void)
{
	msg_readcount = 0;
	msg_badread = false;
}

// returns -1 and sets msg_badread if no more characters are available
int MSG_ReadChar (void)
{
	int     c;

//	if (msg_readcount+1 > net_message.cursize)
	if (msg_readcount >= net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = (signed char)net_message.data[msg_readcount];
	msg_readcount++;

	return c;
}

int MSG_ReadByte (void)
{
	int     c;

//	if (msg_readcount+1 > net_message.cursize)
	if (msg_readcount >= net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = (unsigned char)net_message.data[msg_readcount];
	msg_readcount++;

	return c;
}

#ifdef PROQUAKE_EXTENSION
// JPG - need this to check for ProQuake messages
int MSG_PeekByte (void)
{
//	if (msg_readcount+1 > net_message.cursize)
	if (msg_readcount >= net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	return (unsigned char)net_message.data[msg_readcount];
}
#endif

int MSG_ReadShort (void)
{
	int     c;

	if (msg_readcount+2 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = (short)(net_message.data[msg_readcount]
	+ (net_message.data[msg_readcount+1]<<8));

	msg_readcount += 2;

	return c;
}

int MSG_ReadLong (void)
{
	int     c;

	if (msg_readcount+4 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = net_message.data[msg_readcount]
	+ (net_message.data[msg_readcount+1]<<8)
	+ (net_message.data[msg_readcount+2]<<16)
	+ (net_message.data[msg_readcount+3]<<24);

	msg_readcount += 4;

	return c;
}

float MSG_ReadFloat (void)
{
	union
	{
		byte    b[4];
		float   f;
		int     l;
	} dat;

	dat.b[0] =      net_message.data[msg_readcount];
	dat.b[1] =      net_message.data[msg_readcount+1];
	dat.b[2] =      net_message.data[msg_readcount+2];
	dat.b[3] =      net_message.data[msg_readcount+3];
	msg_readcount += 4;

	dat.l = LittleLong (dat.l);

	return dat.f;
}

char *MSG_ReadString (void)
{
	static char     string[2048];
	int             l,c;

	l = 0;
	do
	{
		c = MSG_ReadChar ();
		if (c == -1 || c == 0)
			break;
		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);

	string[l] = 0;

	return string;
}

float MSG_ReadCoord (void)
{
	return MSG_ReadShort() * (1.0/8);
}

float MSG_ReadAngle (void)
{
	return MSG_ReadChar() * (360.0/256);
}



//===========================================================================

void SZ_Alloc (sizebuf_t *buf, int startsize)
{
	if (startsize < 256)
		startsize = 256;
	buf->data = Hunk_AllocName (startsize, "sizebuf");
	buf->maxsize = startsize;
	buf->cursize = 0;
}

void SZ_Free (sizebuf_t *buf)
{
//      Z_Free (buf->data);
//      buf->data = NULL;
//      buf->maxsize = 0;
	buf->cursize = 0;
}

void SZ_Clear (sizebuf_t *buf)
{
	buf->cursize = 0;
}

void *SZ_GetSpace (sizebuf_t *buf, int length)
{
	void    *data;

	if (buf->cursize + length > buf->maxsize)
	{
		if (!buf->allowoverflow)
			Sys_Error ("SZ_GetSpace: overflow without allowoverflow set");

		if (length > buf->maxsize)
			Sys_Error ("SZ_GetSpace: %i is > full buffer size", length);

		buf->overflowed = true;
		Con_Printf ("SZ_GetSpace: overflow");
		SZ_Clear (buf);
	}

	data = buf->data + buf->cursize;
	buf->cursize += length;

	return data;
}

void SZ_Write (sizebuf_t *buf, void *data, int length)
{
	memcpy (SZ_GetSpace(buf,length),data,length);
}

void SZ_Print (sizebuf_t *buf, char *data)
{
	int             len;

	len = strlen(data)+1;

// byte * cast to keep VC++ happy
	if (buf->data[buf->cursize-1])
		memcpy ((byte *)SZ_GetSpace(buf, len),data,len); // no trailing 0
	else
		memcpy ((byte *)SZ_GetSpace(buf, len-1)-1,data,len); // write over trailing 0
}

//============================================================================

/*
============
COM_SkipPath
============
*/
char *COM_SkipPath (char *pathname)
{
	char    *last;

	last = pathname;
	while (*pathname)
	{
		if (*pathname == '/' || *pathname == '\\')
			last = pathname+1;
		pathname++;
	}
	return last;
}

/*
============
COM_StripExtension
============
*/
void COM_StripExtension (char *in, char *out)
{
	char	*dot;

	if (!(dot = strrchr(in, '.')))
	{
		strlcpy (out, in, strlen(in) + 1);
		return;
	}

	while (*in && in != dot)
		*out++ = *in++;

	*out = 0;
}

/*
============
COM_FileExtension
============
*/
char *COM_FileExtension (char *in)
{
	static char exten[8];
	int             i;

	if (!(in = strrchr(in, '.')))
		return "";
	in++;

	for (i=0 ; i<7 && *in ; i++,in++)
		exten[i] = *in;
	exten[i] = 0;

	return exten;
}

#ifdef HTTP_DOWNLOAD

/*
============
COM_GetFolder
============
*/
void COM_GetFolder (char *in, char *out)
{
	char *last = NULL;

	while (*in)
	{
		if (*in == '/')
			last = out;
		*out++ = *in++;
	}
	if (last)
		*last = 0;
	else
		*out = 0;
}
#endif


/*
============
COM_FileBase
============
*/
void COM_FileBase (char *in, char *out)
{
	char *s, *s2;

	s = in + strlen(in) - 1;

	while (s != in && *s != '.')
		s--;

	for (s2 = s ; *s2 && *s2 != '/' ; s2--)
	;

	if (s-s2 < 2)
		strcpy (out,"?model?");
	else
	{
		s--;
		strncpy (out,s2+1, s-s2);
		out[s-s2] = 0;
	}
}


/*
==================
COM_ForceExtension

If path doesn't have an extension or has a different extension, append(!) specified extension
Extension should include the .
==================
*/
void COM_ForceExtension (char *path, char *extension)
{
	char    *src;

	src = path + strlen(path) - 1;

	while (*src != '/' && src != path)
	{
		if (*src-- == '.')
		{
			COM_StripExtension (path, path);
			strlcat (path, extension, sizeof(path));
			return;
		}
	}

	strlcat (path, extension, MAX_OSPATH);
}

/*
==================
COM_DefaultExtension

If path doesn't have an extension, append extension
Extension should include the .
==================
*/
void COM_DefaultExtension (char *path, char *extension)
{
	char    *src;
//
// if path doesn't have a .EXT, append extension
// (extension should include the .)
//
	src = path + strlen(path) - 1;

	while (*src != '/' && src != path)
	{
		if (*src == '.')
			return;                 // it has an extension
		src--;
	}

	strlcat (path, extension, MAX_OSPATH);
}


/*
==============
COM_Parse

Parse a token out of a string
==============
*/
char *COM_Parse (char *data)
{
	int	c, len;

	len = 0;
	com_token[0] = 0;

	if (!data)
		return NULL;

// skip whitespace
skipwhite:
	while ( (c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;                    // end of file;
		data++;
	}

// skip // comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}


// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				com_token[len] = 0;
				return data;
			}
			com_token[len] = c;
			len++;
		}
	}

// parse single characters
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c==':')
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data+1;
	}

// parse a regular word
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
		// joe, from ProQuake: removed ':' so that ip:port works
		if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' /* || c==':' */)	// JPG 3.20 - so that ip:port works
			break;
	} while (c>32);

	com_token[len] = 0;
	return data;
}


/*
================
COM_CheckParm

Returns the position (1 to argc-1) in the program's argument list
where the given parameter apears, or 0 if not present
================
*/
int COM_CheckParm (char *parm)
{
	int             i;

	for (i=1 ; i<com_argc ; i++)
	{
		if (!com_argv[i])
			continue;               // NEXTSTEP sometimes clears appkit vars.
		if (!strcmp (parm,com_argv[i]))
			return i;
	}

	return 0;
}

/*
================
COM_CheckRegistered

Looks for the pop.lmp file and verifies it.
Sets the "registered" cvar.
Immediately exits out if an alternate game was attempted to be started without
being registered.
================
*/
void COM_CheckRegistered (void)
{
	int             h;
	unsigned short  check[128];
	int                     i;

#ifdef SUPPORTS_KUROK
	if (IS_KUROK)
	{
		Cvar_SetStringByRef (&cmdline, com_cmdline);
		Cvar_SetValueByRef (&registered, 1);
		static_registered = 1;
		return;
	}
#endif

	COM_OpenFile("gfx/pop.lmp", &h);
	static_registered = 0;

	if (h == -1)
	{
#if WINDED
	Sys_Error ("This dedicated server requires a full registered copy of Quake");
#endif
		Con_Printf ("Playing shareware version.\n");
		if (com_modified)
			Sys_Error ("You must have the registered version to use modified games");
		return;
	}

	Sys_FileRead (h, check, sizeof(check));
	COM_CloseFile (h);

	for (i=0 ; i<128 ; i++)
		if (pop[i] != (unsigned short)BigShort (check[i]))
			Sys_Error ("Corrupted data file.");

	Cvar_SetStringByRef (&cmdline, com_cmdline);
	// Baker: 3.99k this is horrible!!!!! Never do that again -> cmdline.string = com_cmdline;

	Cvar_SetValueByRef (&registered, 1);
	static_registered = 1;
	Con_Printf ("Playing registered version.\n");
}


void COM_Path_f (void);


/*
================
COM_InitArgv
================
*/
void COM_InitArgv (int argc, char **argv)
{
	qboolean        safe;
	int             i, j, n;

// reconstitute the command line for the cmdline externally visible cvar
	n = 0;

	for (j=0 ; j<MAX_NUM_ARGVS && j< argc ; j++)
	{
		i = 0;

		while ((n < (CMDLINE_LENGTH - 1)) && argv[j][i])
		{
			com_cmdline[n++] = argv[j][i++];
		}

		if (n < (CMDLINE_LENGTH - 1))
			com_cmdline[n++] = ' ';
		else
			break;
	}

	com_cmdline[n] = 0;

	safe = false;

	for (com_argc=0 ; com_argc<MAX_NUM_ARGVS && com_argc < argc ; com_argc++)
	{
		largv[com_argc] = argv[com_argc];
		if (!strcmp ("-safe", argv[com_argc]))
			safe = true;
	}

	if (safe)
	{
	// force all the safe-mode switches. Note that we reserved extra space in
	// case we need to add these, so we don't need an overflow check
		for (i=0 ; i<NUM_SAFE_ARGVS ; i++)
		{
			largv[com_argc] = safeargvs[i];
			com_argc++;
		}
	}

	largv[com_argc] = argvdummy;
	com_argv = largv;

	//ProQuake 4.10 new extras

	if (COM_CheckParm ("-deeplair")) 		mod_deeplair = true;
	if (COM_CheckParm ("-conhide"))  		mod_conhide = true;
	if (COM_CheckParm ("-nosoundwarn"))  	mod_nosoundwarn = true;

}


/*
================
COM_Init
================
*/
void COM_Init (char *basedir)
{
	byte    swaptest[2] = {1,0};

	// force set little endian to be cool and fast
	bigendien = false;
	BigShort = ShortSwap;
	BigLong = LongSwap;
	BigFloat = FloatSwap;

	Cvar_RegisterVariable (&registered, NULL);
	Cvar_RegisterVariable (&cmdline, NULL);  // Baker 3.99c: needed for test2 command

	Cmd_AddCommand ("path", COM_Path_f);

	COM_InitFilesystem ();
	COM_CheckRegistered ();
}


/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
char    *va(char *format, ...)
{
	va_list         argptr;
	static char             string[1024];

	va_start (argptr, format);
	vsnprintf(string, sizeof(string),  format,argptr);

	va_end (argptr);

	return string;
}

char *CopyString (char *in)
{
	char	*out;

	out = Z_Malloc (strlen(in)+1);
	strcpy (out, in);

	return out;
}

/// just for debugging
int     memsearch (byte *start, int count, int search)
{
	int             i;

	for (i=0 ; i<count ; i++)
		if (start[i] == search)
			return i;
	return -1;
}

/*
=============================================================================

QUAKE FILESYSTEM

=============================================================================
*/

int     com_filesize;


//
// in memory
//

typedef struct
{
	char    name[MAX_QPATH];
	int             filepos, filelen;
} packfile_t;

typedef struct pack_s
{
	char    filename[MAX_OSPATH];
	int             handle;
	int             numfiles;
	packfile_t      *files;
} pack_t;


// on disk

typedef struct
{
	char    name[56];
	int             filepos, filelen;
} dpackfile_t;

typedef struct
{
	char    id[4];
	int             dirofs;
	int             dirlen;
} dpackheader_t;

#define MAX_FILES_IN_PACK       2048

char    com_cachedir[MAX_OSPATH];
char    com_gamedir[MAX_OSPATH] = "";	// JPG 3.20 - added initialization

typedef struct searchpath_s
{
	char    filename[MAX_OSPATH];
	pack_t  *pack;          // only one of filename / pack will be used
	struct searchpath_s *next;
} searchpath_t;

searchpath_t    *com_searchpaths = NULL;	// JPG 3.20 - added NULL
#ifdef PROQUAKE_EXTENSION
searchpath_t	*com_verifypaths = NULL;	// JPG 3.20 - use original game directory for verify path
#endif

/*
============
COM_Path_f
============
*/
void COM_Path_f (void)
{
	searchpath_t    *s;

	Con_Printf ("Current search path:\n");
	for (s=com_searchpaths ; s ; s=s->next)
	{
		if (s->pack)
		{
			Con_Printf ("%s (%i files)\n", s->pack->filename, s->pack->numfiles);
		}
		else
			Con_Printf ("%s\n", s->filename);
	}
}

/*
============
COM_WriteFile

The filename will be prefixed by the current game directory
============
*/
void COM_WriteFile (char *filename, void *data, int len)
{
	int             handle;
	char    name[MAX_OSPATH];
#ifdef SUPPORTS_GAMEDIR_SWITCHING
	Sys_mkdir (com_gamedir); //johnfitz -- if we've switched to a nonexistant gamedir, create it now so we don't crash
#endif
	snprintf(name, sizeof(name),  "%s/%s", com_gamedir, filename);

	handle = Sys_FileOpenWrite (name);
	if (handle == -1)
	{
		Sys_Printf ("COM_WriteFile: failed on %s\n", name);
		return;
	}

	Sys_Printf ("COM_WriteFile: %s\n", name);
	Sys_FileWrite (handle, data, len);
	Sys_FileClose (handle);
}

/*
============
COM_CreatePath

Only used for CopyFile
============
*/
void    COM_CreatePath (char *path)
{
	char    *ofs;

	for (ofs = path+1 ; *ofs ; ofs++)
	{
		if (*ofs == '/')
		{       // create the directory
			*ofs = 0;
			Sys_mkdir (path);
			*ofs = '/';
		}
	}
}

/*
===========
COM_CopyFile

Copies a file over from the net to the local cache, creating any directories
needed.  This is for the convenience of developers using ISDN from home.
===========
*/
void COM_CopyFile (char *netpath, char *cachepath)
{
	int             in, out;
	int             remaining, count;
	char    buf[4096];

	remaining = Sys_FileOpenRead (netpath, &in);
	COM_CreatePath (cachepath);     // create directories up to the cache file
	out = Sys_FileOpenWrite (cachepath);

	while (remaining)
	{
		if (remaining < sizeof(buf))
			count = remaining;
		else
			count = sizeof(buf);
		Sys_FileRead (in, buf, count);
		Sys_FileWrite (out, buf, count);
		remaining -= count;
	}

	Sys_FileClose (in);
	Sys_FileClose (out);
}

/*
===========
COM_FindFile

Finds the file in the search path.
Sets com_filesize and one of handle or file
===========
*/
int COM_FindFile (char *filename, int *handle, int *file)
{
	searchpath_t    *search;
	char            netpath[MAX_OSPATH];
	char            cachepath[MAX_OSPATH];
	pack_t          *pak;
	int                     i;
	int                     findtime, cachetime;

	if (file && handle)
		Sys_Error ("COM_FindFile: both handle and file set");
	if (!file && !handle)
		Sys_Error ("COM_FindFile: neither handle or file set");

//
// search through the path, one element at a time
//
	search = com_searchpaths;
	if (proghack)
	{	// gross hack to use quake 1 progs with quake 2 maps
		if (!strcmp(filename, "progs.dat"))
			search = search->next;
	}

	for ( ; search ; search = search->next)
	{
	// is the element a pak file?
		if (search->pack)
		{
		// look through all the pak file elements
			pak = search->pack;
			for (i=0 ; i<pak->numfiles ; i++)
				if (!strcmp (pak->files[i].name, filename))
				{       // found it!
					Sys_Printf ("PackFile: %s : %s\n",pak->filename, filename);
					if (handle)
					{
						*handle = pak->handle;
						Sys_FileSeek (pak->handle, pak->files[i].filepos);
					}
					else
					{       // open a new file on the pakfile
						Sys_FileOpenRead(pak->filename, file);
						if ((*file) >= 0)
							Sys_FileSeek(*file, pak->files[i].filepos);
					}
					com_filesize = pak->files[i].filelen;
					return com_filesize;
				}
		}
		else
		{
	// check a file in the directory tree
			if (!static_registered)
			{       // if not a registered version, don't ever go beyond base
				if ( strchr (filename, '/') || strchr (filename,'\\'))
					continue;
			}

			snprintf(netpath, sizeof(netpath),  "%s/%s",search->filename, filename);

			findtime = Sys_FileTime (netpath);
			if (findtime == -1)
				continue;

		// see if the file needs to be updated in the cache
			if (!com_cachedir[0])
				strcpy (cachepath, netpath);
			else
			{
#if defined(_WIN32)
				if ((strlen(netpath) < 2) || (netpath[1] != ':'))
					snprintf(cachepath, sizeof(cachepath), "%s%s", com_cachedir, netpath);
				else
					snprintf(cachepath, sizeof(cachepath), "%s%s", com_cachedir, netpath+2);
//#elif defined (MACOSX) || defined (LINUX)
#else // Above line is insufficient since we do PSP and Flash now too
				snprintf(cachepath, sizeof(cachepath), "%s%s", com_cachedir, netpath);
#endif //^^ Windows prefixes drive names (no idea what OSX or Linux do for multiple drives).  In truth the above needs work for Windows networked drives prefixes (see aguirRe Quake for solution)

				cachetime = Sys_FileTime (cachepath);

				if (cachetime < findtime)
					COM_CopyFile (netpath, cachepath);
				strcpy (netpath, cachepath);
			}

			Sys_Printf ("FindFile: %s\n",netpath);
			com_filesize = Sys_FileOpenRead (netpath, &i);
			if (handle)
				*handle = i;
			else
			{
				Sys_FileClose (i);
				Sys_FileOpenRead(netpath, file);
			}
			return com_filesize;
		}

	}

	Sys_Printf ("FindFile: can't find %s\n", filename);

	if (handle)
		*handle = -1;
	else
		*file = -1;
	com_filesize = -1;
	return -1;
}


/*
===========
COM_OpenFile

filename never has a leading slash, but may contain directory walks
returns a handle and a length
it may actually be inside a pak file
===========
*/
int COM_OpenFile (char *filename, int *handle)
{
	return COM_FindFile (filename, handle, NULL);
}

/*
===========
COM_FOpenFile

If the requested file is inside a packfile, a new FILE * will be opened
into the file.
===========
*/
int COM_FOpenFile (char *filename, int *file)
{
	return COM_FindFile (filename, NULL, file);
}

/*
============
COM_CloseFile

If it is a pak file handle, don't really close it
============
*/
void COM_CloseFile (int h)
{
	searchpath_t    *s;

	for (s = com_searchpaths ; s ; s=s->next)
		if (s->pack && s->pack->handle == h)
			return;

	Sys_FileClose (h);
}


/*
============
COM_LoadFile

Filename are relative to the quake directory.
Always appends a 0 byte.
============
*/
static cache_user_t *loadcache;
static byte    *loadbuf;
static int             loadsize;
byte *COM_LoadFile (char *path, int usehunk)
{
	int             h;
	byte    *buf;
	char    base[32];
	int             len;

	buf = NULL;     // quiet compiler warning

// look for it in the filesystem or pack files
	len = COM_OpenFile (path, &h);
	if (h == -1)
		return NULL;

// extract the filename base name for hunk tag
	COM_FileBase (path, base);

	if (usehunk == 1)
		buf = Hunk_AllocName (len+1, base);
	else if (usehunk == 2)
		buf = Hunk_TempAlloc (len+1);
	else if (usehunk == 0)
		buf = Z_Malloc (len+1);
	else if (usehunk == 3)
		buf = Cache_Alloc (loadcache, len+1, base);
	else if (usehunk == 4)
	{
		if (len+1 > loadsize)
			buf = Hunk_TempAlloc (len+1);
		else
			buf = loadbuf;
	}
	else
		Sys_Error ("COM_LoadFile: bad usehunk");

	if (!buf)
		Sys_Error ("COM_LoadFile: not enough space for %s", path);

	((byte *)buf)[len] = 0;

	Draw_BeginDisc ();
	Sys_FileRead (h, buf, len);
	COM_CloseFile (h);
	Draw_EndDisc ();

	return buf;
}

byte *COM_LoadHunkFile (char *path)
{
	return COM_LoadFile (path, 1);
}

byte *COM_LoadTempFile (char *path)
{
	return COM_LoadFile (path, 2);
}

void COM_LoadCacheFile (char *path, struct cache_user_s *cu)
{
	loadcache = cu;
	COM_LoadFile (path, 3);
}

// uses temp hunk if larger than bufsize
byte *COM_LoadStackFile (char *path, void *buffer, int bufsize)
{
	byte    *buf;

	loadbuf = (byte *)buffer;
	loadsize = bufsize;
	buf = COM_LoadFile (path, 4);

	return buf;
}

/*
=================
COM_LoadPackFile

Takes an explicit (not game tree related) path to a pak file.

Loads the header and directory, adding the files at the beginning
of the list so they override previous pack files.
=================
*/
pack_t *COM_LoadPackFile (char *packfile)
{
	int		i, numpackfiles;
	dpackheader_t   header;
	packfile_t              *newfiles;
	pack_t                  *pack;
	int                             packhandle;
	dpackfile_t             info[MAX_FILES_IN_PACK];
	unsigned short          crc;

	if (Sys_FileOpenRead (packfile, &packhandle) == -1)
	{
//              Con_Printf ("Couldn't open %s\n", packfile);
		return NULL;
	}
	Sys_FileRead (packhandle, (void *)&header, sizeof(header));
	if (header.id[0] != 'P' || header.id[1] != 'A' || header.id[2] != 'C' || header.id[3] != 'K')
		Sys_Error ("%s is not a packfile", packfile);
	header.dirofs = LittleLong (header.dirofs);
	header.dirlen = LittleLong (header.dirlen);

	numpackfiles = header.dirlen / sizeof(dpackfile_t);

	if (numpackfiles > MAX_FILES_IN_PACK)
		Sys_Error ("%s has %i files", packfile, numpackfiles);

	if (numpackfiles != PAK0_COUNT)
		com_modified = true;    // not the original file

	newfiles = Hunk_AllocName (numpackfiles * sizeof(packfile_t), "packfile");

	Sys_FileSeek (packhandle, header.dirofs);
	Sys_FileRead (packhandle, (void *)info, header.dirlen);

// crc the directory to check for modifications
	CRC_Init (&crc);
	for (i=0 ; i<header.dirlen ; i++)
		CRC_ProcessByte (&crc, ((byte *)info)[i]);
	if (crc != PAK0_CRC)
		com_modified = true;

// parse the directory
	for (i=0 ; i<numpackfiles ; i++)
	{
		strcpy (newfiles[i].name, info[i].name);
		newfiles[i].filepos = LittleLong(info[i].filepos);
		newfiles[i].filelen = LittleLong(info[i].filelen);
	}

	pack = Hunk_Alloc (sizeof (pack_t));
	strcpy (pack->filename, packfile);
	pack->handle = packhandle;
	pack->numfiles = numpackfiles;
	pack->files = newfiles;

	// FitzQuake has this commented out

	Con_Printf ("Added packfile %s (%i files)\n", packfile, numpackfiles);
	return pack;
}


/*
================
COM_AddGameDirectory

Sets com_gamedir, adds the directory to the head of the path,
then loads and adds pak1.pak pak2.pak ...
================
*/
void COM_AddGameDirectory (char *dir)
{
	int                             i;
	searchpath_t    *search;
	pack_t                  *pak;
	char                    pakfile[MAX_OSPATH];

	strcpy (com_gamedir, dir);

// add the directory to the search path
	search = Hunk_Alloc (sizeof(searchpath_t));
	strcpy (search->filename, dir);
	search->next = com_searchpaths;
	com_searchpaths = search;

// add any pak files in the format pak0.pak pak1.pak, ...
// Changed so it will search for the next pak if one is missing which we need to do for
// Kurok to be standalone AND be able to run Quake alongside it.

	for (i=0 ; i<99; i++)
	{
		snprintf(pakfile, sizeof(pakfile),  "%s/pak%i.pak", dir, i);
		pak = COM_LoadPackFile (pakfile);

// Don't stop if a pak is missing when running Kurok.
            if (!pak)
                break;

		search = Hunk_Alloc (sizeof(searchpath_t));
		search->pack = pak;
		search->next = com_searchpaths;
		com_searchpaths = search;
	}

//
// add the contents of the parms.txt file to the end of the command line
//

}

/*
=================
COM_InitFilesystem
=================
*/
void COM_InitFilesystem (void)
{
	int             i, j;
	char    basedir[MAX_OSPATH];
	searchpath_t    *search;

//
// -basedir <path>
// Overrides the system supplied base directory (under GAMENAME)
//
	i = COM_CheckParm ("-basedir");
	if (i && i < com_argc-1)
		strcpy (basedir, com_argv[i+1]);
	else
		strcpy (basedir, host_parms.basedir);

	j = strlen (basedir);

	if (j > 0)
	{
		if ((basedir[j-1] == '\\') || (basedir[j-1] == '/'))
			basedir[j-1] = 0;
	}

//
// -cachedir <path>
// Overrides the system supplied cache directory (NULL or /qcache)
// -cachedir - will disable caching.
//
	i = COM_CheckParm ("-cachedir");
	if (i && i < com_argc-1)
	{
		if (com_argv[i+1][0] == '-')
			com_cachedir[0] = 0;
		else
			strcpy (com_cachedir, com_argv[i+1]);
	}
	else if (host_parms.cachedir)
		strcpy (com_cachedir, host_parms.cachedir);
	else
		com_cachedir[0] = 0;

// start up with GAMENAME by default (id1)
	COM_AddGameDirectory (va("%s/"GAMENAME, basedir) );

	if (COM_CheckParm ("-quoth"))
		COM_AddGameDirectory ("quoth");

#ifdef PROQUAKE_EXTENSION
	com_verifypaths = com_searchpaths;	// JPG 3.20
#endif

//
// -game <gamedir>
// Adds basedir/gamedir as an override game
//
	i = COM_CheckParm ("-game");
	if (i && i < com_argc-1)
	{
		com_modified = true;
		COM_AddGameDirectory (va("%s/%s", basedir, com_argv[i+1]));
	}

//
// -path <dir or packfile> [<dir or packfile>] ...
// Fully specifies the exact serach path, overriding the generated one
//
	i = COM_CheckParm ("-path");
	if (i)
	{
		com_modified = true;
		com_searchpaths = NULL;
		while (++i < com_argc)
		{
			if (!com_argv[i] || com_argv[i][0] == '+' || com_argv[i][0] == '-')
				break;

			search = Hunk_Alloc (sizeof(searchpath_t));
			if ( !strcmp(COM_FileExtension(com_argv[i]), "pak") )
			{
				search->pack = COM_LoadPackFile (com_argv[i]);
				if (!search->pack)
					Sys_Error ("Couldn't load packfile: %s", com_argv[i]);
			}
			else
				strcpy (search->filename, com_argv[i]);
			search->next = com_searchpaths;
			com_searchpaths = search;
		}
	}

	if (COM_CheckParm ("-proghack"))
		proghack = true;
}

#ifdef PROQUAKE_EXTENSION
// JPG 3.20 - model checking
/*
================
COM_ModelCRC
================
*/
void COM_ModelCRC (void)
{
	searchpath_t    *search;
	byte *data;
	int len, i;

	search = com_searchpaths;
	com_searchpaths = com_verifypaths;
	for (i = 1 ; sv.model_precache[i] ; i++)
	{
		if (sv.model_precache[i][0] != '*')
		{
			data = COM_LoadFile(sv.model_precache[i], 2);				// 2 = temp alloc on hunk
			if (!data)
				Host_Error("COM_ModelCRC: Could not load %s", sv.model_precache[i]);
			len = (*(int *)(data - 12)) - 16;							// header before data contains size
			sv.model_crc[i] = Security_CRC(data, len);
		}
	}
	com_searchpaths = search;
}
#endif

//======================================

// snprintf and vsnprintf are NOT portable. Use their DP counterparts instead

int dpsnprintf (char *buffer, size_t buffersize, const char *format, ...)
{
	va_list args;
	int result;

	va_start (args, format);
	result = dpvsnprintf (buffer, buffersize, format, args);
	va_end (args);

	return result;
}


int dpvsnprintf (char *buffer, size_t buffersize, const char *format, va_list args)
{
	int result;

	result = vsnprintf (buffer, buffersize, format, args);
	if (result < 0 || (size_t)result >= buffersize)
	{
		buffer[buffersize - 1] = '\0';
		return -1;
	}

	return result;
}



//========================================================

// strlcat and strlcpy, from OpenBSD


/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*	$OpenBSD: strlcat.c,v 1.11 2003/06/17 21:56:24 millert Exp $	*/
/*	$OpenBSD: strlcpy.c,v 1.8 2003/06/17 21:56:24 millert Exp $	*/

#if !defined(FLASH)
//========================================================
// strlcat and strlcpy, from OpenBSD
/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*	$OpenBSD: strlcat.c,v 1.11 2003/06/17 21:56:24 millert Exp $	*/
/*	$OpenBSD: strlcpy.c,v 1.8 2003/06/17 21:56:24 millert Exp $	*/


#ifndef HAVE_STRLCAT
size_t
strlcat (char *dst, const char *src, size_t siz)
{
	register char *d = dst;
	register const char *s = src;
	register size_t n = siz;
	size_t dlen;

	/* Find the end of dst and adjust bytes left but don't go past end */
	while (n-- != 0 && *d != '\0')
		d++;
	dlen = d - dst;
	n = siz - dlen;

	if (n == 0)
		return(dlen + strlen(s));
	while (*s != '\0') {
		if (n != 1) {
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';

	return(dlen + (s - src));	/* count does not include NUL */
}
#endif  // #ifndef HAVE_STRLCAT


#ifndef HAVE_STRLCPY
size_t
strlcpy(char *dst, const char *src, size_t siz)
{
	register char *d = dst;
	register const char *s = src;
	register size_t n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0 && --n != 0) {
		do {
			if ((*d++ = *s++) == 0)
				break;
		} while (--n != 0);
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst */
		while (*s++)
			;
	}

	return(s - src - 1);	/* count does not include NUL */
}

#endif  // #ifndef HAVE_STRLCPY


#endif

#if !defined(FLASH) && !defined(PSP)
// Baker: strip leading spaces from string
char *strltrim(char *s) {
	char *t;

	assert(s != NULL);
	for (t = s; isspace(*t); ++t)
		continue;
	memmove(s, t, strlen(t)+1);	/* +1 so that '\0' is moved too */
	return s;
}

#endif



//======================================
// LordHavoc: added these because they are useful

void COM_ToLowerString(char *in, char *out)
{
	while (*in)
	{
		if (*in >= 'A' && *in <= 'Z')
			*out++ = *in++ + 'a' - 'A';
		else
			*out++ = *in++;
	}

}
