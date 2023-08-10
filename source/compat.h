/*
Copyright (C) 2023 Ivy 

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
// compat.h  -- Enhanced support for hipnotic, rogue, and other mods that make engine changes.

#ifndef _COMPAT_H_
#define _COMPAT_H_

// All supported "unique" mods
#define COMPAT_QUAKE        0
#define COMPAT_HIPNOTIC     1   // Scourge of Armagon
#define COMPAT_ROGUE        2   // Dissolution of Eternity
#define COMPAT_KUROK        3   // Kurok (untested)

extern int compat_gametype;

#define IS_QUAKE    ((compat_gametype) == (COMPAT_QUAKE))
#define IS_HIPNOTIC ((compat_gametype) == (COMPAT_HIPNOTIC))
#define IS_ROGUE    ((compat_gametype) == (COMPAT_ROGUE))
#define IS_KUROK    ((compat_gametype) == (COMPAT_KUROK))

#endif // _COMPAT_H_