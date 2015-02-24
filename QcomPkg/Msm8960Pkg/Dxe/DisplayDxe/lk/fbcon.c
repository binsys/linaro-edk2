/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2013, The Linux Foundation. All rights reserved.
 * Copyright (c) 2011-2014, Xiaomi Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <Base.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/ArmLib.h>
#include <Library/PrintLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>

#include <Library/qcom_lk.h>

#include <fbcon.h>
#include <splash.h>
//#include <platform.h>

#include "font5x12.h"

extern int is_cmd_mode_enabled(void);
extern void mipi_dsi_cmd_mode_trigger(void);

struct pos {
	int x;
	int y;
};

static struct fbcon_config *config = NULL;

#define RGB565_BLACK		0x0000
#define RGB565_WHITE		0xffff

#define RGB888_BLACK            0x000000
#define RGB888_WHITE            0xffffff

#define FONT_WIDTH		5
#define FONT_HEIGHT		12

static UINT16			BGCOLOR;
static UINT16			FGCOLOR;

static struct pos		cur_pos;
static struct pos		max_pos;

static void fbcon_drawglyph(UINT16 *pixels, UINT16 paint, unsigned stride,unsigned *glyph)
{
	unsigned x, y, data;
	stride -= FONT_WIDTH;

	data = glyph[0];
	for (y = 0; y < (FONT_HEIGHT / 2); ++y) {
		for (x = 0; x < FONT_WIDTH; ++x) {
			if (data & 1)
				*pixels = paint;
			data >>= 1;
			pixels++;
		}
		pixels += stride;
	}

	data = glyph[1];
	for (y = 0; y < (FONT_HEIGHT / 2); y++) {
		for (x = 0; x < FONT_WIDTH; x++) {
			if (data & 1)
				*pixels = paint;
			data >>= 1;
			pixels++;
		}
		pixels += stride;
	}
}

static void fbcon_flush(void)
{
	if (config->update_start)
		config->update_start();
	if (config->update_done)
		while (!config->update_done());
}

/* TODO: Take stride into account */
static void fbcon_scroll_up(void)
{
	unsigned short *dst = config->base;
	unsigned short *src = dst + (config->width * FONT_HEIGHT);
	unsigned count = config->width * (config->height - FONT_HEIGHT);

	while(count--) {
		*dst++ = *src++;
	}

	count = config->width * FONT_HEIGHT;
	while(count--) {
		*dst++ = BGCOLOR;
	}

	fbcon_flush();
}

/* TODO: take stride into account */
void fbcon_clear(void)
{
	unsigned count = config->width * config->height;
	SetMem((void *)(config->base),count * ((config->bpp) / 8),BGCOLOR);
	//memset(config->base, BGCOLOR, count * ((config->bpp) / 8));
}


static void fbcon_set_colors(unsigned bg, unsigned fg)
{
	BGCOLOR = bg;
	FGCOLOR = fg;
}

void fbcon_putc(char c)
{
	UINT16 *pixels;

	/* ignore anything that happens before fbcon is initialized */
	if (!config)
		return;

	if((unsigned char)c > 127)
		return;
	if((unsigned char)c < 32) {
		if(c == '\n')
			goto newline;
		else if (c == '\r')
			cur_pos.x = 0;
		return;
	}

	pixels = config->base;
	pixels += cur_pos.y * FONT_HEIGHT * config->width;
	pixels += cur_pos.x * (FONT_WIDTH + 1);
	fbcon_drawglyph(pixels, FGCOLOR, config->stride,
			font5x12 + (c - 32) * 2);

	cur_pos.x++;
	if (cur_pos.x < max_pos.x)
		return;

newline:
	cur_pos.y++;
	cur_pos.x = 0;
	if(cur_pos.y >= max_pos.y) {
		cur_pos.y = max_pos.y - 1;
		fbcon_scroll_up();
	} else
		fbcon_flush();
}

void fbcon_setup(struct fbcon_config *_config)
{
	UINT32 bg;
	UINT32 fg;

	ASSERT(_config);

	config = _config;

	switch (config->format) {
	case FB_FORMAT_RGB565:
		fg = RGB565_WHITE;
		bg = RGB565_BLACK;
		break;
        case FB_FORMAT_RGB888:
                fg = RGB888_WHITE;
                bg = RGB888_BLACK;
                break;
	default:
		DEBUG((EFI_D_WARN, "unknown framebuffer pixel format\n"));
		ASSERT(0);
		break;
	}

	fbcon_set_colors(bg, fg);

	cur_pos.x = 0;
	cur_pos.y = 0;
	max_pos.x = config->width / (FONT_WIDTH+1);
	max_pos.y = (config->height - 1) / FONT_HEIGHT;

}

struct fbcon_config* fbcon_display(void)
{
    return config;
}


void fbcon_putImage(struct fbimage *fbimg, BOOLEAN flag);

void display_image_on_screen()
{
	struct fbimage default_fbimg, *fbimg;
	BOOLEAN flag = TRUE;

	fbcon_clear();

		flag = FALSE;
		fbimg = &default_fbimg;
		fbimg->header.width = SPLASH_IMAGE_HEIGHT;
		fbimg->header.height = SPLASH_IMAGE_WIDTH;

		fbimg->image = (unsigned char *)imageBuffer_rgb888;


			unsigned i;
			for(i=0; i<(fbimg->header.width*fbimg->header.height); i++) {
				int r,g,b;
				char* color = (char*) fbimg->image + i*3;

				b = color[0];
				g = color[1];
				r = color[2];

				color[0] = r;
				color[1] = g;
				color[2] = b;
			}

	

	fbcon_putImage(fbimg, flag);
}

void fbcon_putPixel(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{

    unsigned total_x;
    unsigned total_y;
    unsigned bytes_per_bpp;

	total_x = config->width;
	total_y = config->height;
	bytes_per_bpp = ((config->bpp) / 8);

	unsigned char * pixsss = (unsigned char *)config->base;
		
	//Start at left top (0,0) 
	//End at right bottom (total_x-1,total_y-1)
	//RGBRGBRGB
	pixsss[total_x*y*bytes_per_bpp+x*bytes_per_bpp+0] = r;
	pixsss[total_x*y*bytes_per_bpp+x*bytes_per_bpp+1] = g;
	pixsss[total_x*y*bytes_per_bpp+x*bytes_per_bpp+2] = b;

}

void fbcon_putImage(struct fbimage *fbimg, BOOLEAN flag)
{
    unsigned i = 0;
    unsigned total_x;
    unsigned total_y;
    unsigned bytes_per_bpp;
    unsigned image_base;
    unsigned width, pitch, height;
    unsigned char *logo_base;
    struct logo_img_header *header;


	if (!config) {
		DEBUG((EFI_D_WARN, "NULL configuration, image cannot be displayed\n"));
		return;
	}

	if(fbimg) {
		header = &fbimg->header;
		width = pitch = header->width;
		height = header->height;
		logo_base = (unsigned char *)fbimg->image;
	}

	total_x = config->width;
	total_y = config->height;
	bytes_per_bpp = ((config->bpp) / 8);

	if (bytes_per_bpp == 3)
	{
		if(flag) {
			if (header->width == config->width && header->height == config->height)
				return;
			else {
				logo_base = (unsigned char *)config->base + LOGO_IMG_OFFSET;
				if (header->width > config->width) {
					width = config->width;
					pitch = header->width;
					logo_base += (header->width - config->width) / 2 * bytes_per_bpp;
				} else {
					width = pitch = header->width;
				}

				if (header->height > config->height) {
					height = config->height;
					logo_base += (header->height - config->height) / 2 * pitch * bytes_per_bpp;
				} else {
					height = header->height;
				}
			}
		}

		image_base = ((((total_y/2) - (height / 2) ) *
				(config->width)) + (total_x/2 - (width / 2)));
		//for (i = 0; i < height; i++) {
		//	CopyMem ((void *)(config->base + ((image_base + (i * (config->width))) * bytes_per_bpp)),
		//		(void *)(logo_base + (i * pitch * bytes_per_bpp)), (width * bytes_per_bpp));
		//}

	}
	
	for(i = 0;i<total_x;i++)
	{
		fbcon_putPixel(i,1,0xff,0x00,0x00);
		fbcon_putPixel(i,2,0xff,0x00,0x00);
		
		fbcon_putPixel(i,4,0x00,0xff,0x00);
		fbcon_putPixel(i,5,0x00,0xff,0x00);
		
		fbcon_putPixel(i,7,0x00,0x00,0xff);
		fbcon_putPixel(i,8,0x00,0x00,0xff);
	}
	
	//unsigned char * pixsss = (unsigned char *)config->base;
	
	//pixsss[0]=0xff;
	//pixsss[1]=0x00;
	//pixsss[2]=0x00;
	
	//pixsss[3]=0xff;
	//pixsss[4]=0x00;
	//pixsss[5]=0x00;
	
	//pixsss[6]=0xff;
	//pixsss[7]=0x00;
	//pixsss[8]=0x00;
	
	// x y r g b
	// base = y*720*3+x*3

	fbcon_flush();

	if(is_cmd_mode_enabled())
        mipi_dsi_cmd_mode_trigger();
}
