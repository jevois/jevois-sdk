/*
 * osd.h
 *
 * Copyright (C) 2001 Ralph  Metzler <ralph@convergence.de>
 *                  & Marcus Metzler <marcus@convergence.de>
 *                    for convergence integrated media GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Lesser Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _DVBOSD_H_
#define _DVBOSD_H_

#include <linux/compiler.h>

typedef enum {
  OSD_Close = 1, 
  OSD_Open,      
  OSD_Show,      
  OSD_Hide,      
  OSD_Clear,     
  OSD_Fill,      
  OSD_SetColor,  
  OSD_SetPalette,
  OSD_SetTrans,  
  OSD_SetPixel,  
  OSD_GetPixel,  
  OSD_SetRow,    
  OSD_SetBlock,  
  OSD_FillRow,   
  OSD_FillBlock, 
  OSD_Line,      
  OSD_Query,     
  OSD_Test,      
  OSD_Text,      
  OSD_SetWindow,
  OSD_MoveWindow,
  OSD_OpenRaw, 
} OSD_Command;

typedef struct osd_cmd_s {
  OSD_Command cmd;
  int x0;
  int y0;
  int x1;
  int y1;
  int color;
  void __user * data;
} osd_cmd_t;

/* OSD_OpenRaw: set 'color' to desired window type */
typedef enum {
  OSD_BITMAP1,           /* 1 bit bitmap */
  OSD_BITMAP2,           /* 2 bit bitmap */
  OSD_BITMAP4,           /* 4 bit bitmap */
  OSD_BITMAP8,           /* 8 bit bitmap */
  OSD_BITMAP1HR,         /* 1 Bit bitmap half resolution */
  OSD_BITMAP2HR,         /* 2 bit bitmap half resolution */
  OSD_BITMAP4HR,         /* 4 bit bitmap half resolution */
  OSD_BITMAP8HR,         /* 8 bit bitmap half resolution */
  OSD_YCRCB422,          /* 4:2:2 YCRCB Graphic Display */
  OSD_YCRCB444,          /* 4:4:4 YCRCB Graphic Display */
  OSD_YCRCB444HR,        /* 4:4:4 YCRCB graphic half resolution */
  OSD_VIDEOTSIZE,        /* True Size Normal MPEG Video Display */
  OSD_VIDEOHSIZE,        /* MPEG Video Display Half Resolution */
  OSD_VIDEOQSIZE,        /* MPEG Video Display Quarter Resolution */
  OSD_VIDEODSIZE,        /* MPEG Video Display Double Resolution */
  OSD_VIDEOTHSIZE,       /* True Size MPEG Video Display Half Resolution */
  OSD_VIDEOTQSIZE,       /* True Size MPEG Video Display Quarter Resolution*/
  OSD_VIDEOTDSIZE,       /* True Size MPEG Video Display Double Resolution */
  OSD_VIDEONSIZE,        /* Full Size MPEG Video Display */
  OSD_CURSOR             /* Cursor */
} osd_raw_window_t;

typedef struct osd_cap_s {
  int  cmd;
#define OSD_CAP_MEMSIZE         1  /* memory size */
  long val;
} osd_cap_t;


#define OSD_SEND_CMD            _IOW('o', 160, osd_cmd_t)
#define OSD_GET_CAPABILITY      _IOR('o', 161, osd_cap_t)

#endif
