/* $Id$
 *
 *      hptkill - areas killer for high Portable Tosser (hpt)
 *      by Serguei Revtov 2:5021/11.10 || 2:5021/19.1
 *      Some code was taken from hpt/src/areafix.c
 *
 * This file is part of HPT.
 *
 * HPT is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * HPT is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPT; see the file COPYING.  If not, write to the Free
 * Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *****************************************************************************
 */
#ifndef VERSION_H
#define VERSION_H

#include <fidoconf/version.h>

/* basic version number */
#define VER_MAJOR 1
#define VER_MINOR 4
#define VER_PATCH 0
#define VER_BRANCH BRANCH_STABLE

extern char *versionStr;

#endif
