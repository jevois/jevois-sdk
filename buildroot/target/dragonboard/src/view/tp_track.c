/*
 * \file        tp_track.c
 * \brief       
 *
 * \version     1.0.0
 * \date        2012年06月27日
 * \author      James Deng <csjamesdeng@allwinnertech.com>
 *
 * Copyright (c) 2012 Allwinner Technology. All Rights Reserved.
 *
 */

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <asm-generic/ioctl.h>

#include "drv_display_sun4i.h"
#include "dragonboard.h"
#include "script.h"
#include "df_view.h"

#define ION_IOC_MAGIC		'I'
#define ION_IOC_ALLOC		_IOWR(ION_IOC_MAGIC, 0, struct ion_allocation_data)
#define ION_IOC_FREE		_IOWR(ION_IOC_MAGIC, 1, struct ion_handle_data)
#define ION_IOC_MAP		_IOWR(ION_IOC_MAGIC, 2, struct ion_fd_data)
#define ION_IOC_SHARE		_IOWR(ION_IOC_MAGIC, 4, struct ion_fd_data)
#define ION_IOC_IMPORT		_IOWR(ION_IOC_MAGIC, 5, struct ion_fd_data)
#define ION_IOC_SYNC		_IOWR(ION_IOC_MAGIC, 7, struct ion_fd_data)
#define ION_IOC_CUSTOM		_IOWR(ION_IOC_MAGIC, 6, struct ion_custom_data)

#define ION_FLAG_CACHED 1		/* mappings of this buffer should be
					   cached, ion will do cache
					   maintenance when the buffer is
					   mapped for dma */
#define ION_FLAG_CACHED_NEEDS_SYNC 2	/* mappings of this buffer will created
					   at mmap time, if this is set
					   caches must be managed manually */

typedef struct {
	long 	start;
	long 	end;
}sunxi_cache_range;

typedef struct {
	void *handle;
	unsigned int phys_addr;
	unsigned int size;
}sunxi_phys_data;

#define ION_IOC_SUNXI_FLUSH_RANGE           5
#define ION_IOC_SUNXI_FLUSH_ALL             6
#define ION_IOC_SUNXI_PHYS_ADDR             7
#define ION_IOC_SUNXI_DMA_COPY              8

#define ION_HEAP_SYSTEM_MASK		(1 << ION_HEAP_TYPE_SYSTEM)
#define ION_HEAP_SYSTEM_CONTIG_MASK	(1 << ION_HEAP_TYPE_SYSTEM_CONTIG)
#define ION_HEAP_CARVEOUT_MASK		(1 << ION_HEAP_TYPE_CARVEOUT)

struct ion_allocation_data {
	size_t len;
	size_t align;
	unsigned int heap_id_mask;
	unsigned int flags;
	void *handle;
};

struct ion_handle_data {
	void *handle;
};

struct ion_fd_data {
	void *handle;
	int fd;
};

struct ion_custom_data {
	unsigned int cmd;
	unsigned long arg;
};

enum ion_heap_type {
	ION_HEAP_TYPE_SYSTEM,
	ION_HEAP_TYPE_SYSTEM_CONTIG,
	ION_HEAP_TYPE_CARVEOUT,
	ION_HEAP_TYPE_CHUNK,
	ION_HEAP_TYPE_CUSTOM, /* must be last so device specific heaps always
				 are at the end of this enum */
	ION_NUM_HEAPS = 16,
};

static int disp;
//static unsigned int layer_id;
static int screen_width;
static int screen_height;
//static int fb;
static unsigned char *buffer = NULL;
static disp_layer_info info;

static int draw_type;
static int tp_draw_color_idx;
static unsigned int tp_draw_color;

static int pre_x;
static int pre_y;

int tp_track_init(void)
{
	struct ion_allocation_data alloc_data;
	struct ion_handle_data handle_data;
	struct ion_fd_data fd_data;
	struct ion_custom_data custom_data;
	sunxi_phys_data phys_data;
	unsigned int arg[3];
	int fd, ret = 0;
    unsigned int args[4];

    if ((disp = open("/dev/disp", O_RDWR)) == -1) {
        db_error("can't open /dev/disp(%s)\n", strerror(errno));
        return -1;
    }

    args[0] = 0;
    screen_width  = ioctl(disp, DISP_CMD_GET_SCN_WIDTH, args);
    screen_height = ioctl(disp, DISP_CMD_GET_SCN_HEIGHT, args);

#if 0
    pre_x = screen_width >> 1;
    pre_y = screen_height >> 1;
#else
    pre_x = -1;
    pre_y = -1;
#endif

	/* 1. open ion device */
	fd = open("/dev/ion", O_RDONLY);
	if(fd < 0) {
		db_msg("####################err open /dev/ion\n");
		return -1;
	}
	/* 2. alloc buffer */
	alloc_data.len = screen_width * screen_height * 4;
	alloc_data.align = 0;
	alloc_data.heap_id_mask = ION_HEAP_CARVEOUT_MASK;
	alloc_data.flags = 0;
	ret = ioctl(fd, ION_IOC_ALLOC, &alloc_data);
	if(ret) {
		db_msg("######################ION_IOC_ALLOC err, ret %d, handle 0x%08x\n", ret, (unsigned int)alloc_data.handle);
		goto out1;
	}
	/* 3. get dmabuf fd */
	fd_data.handle = alloc_data.handle;
	ret = ioctl(fd, ION_IOC_MAP, &fd_data);
	if(ret) {
		db_msg("##########################ION_IOC_MAP err, ret %d, dmabuf fd 0x%08x\n", ret, (unsigned int)fd_data.fd);
		goto out2;
	}
	/* 4. mmap to user space */
	buffer = mmap(NULL, alloc_data.len, PROT_READ|PROT_WRITE, MAP_SHARED, fd_data.fd, 0);
	if(MAP_FAILED == buffer) {
		db_msg("#####################mmap err, ret %d\n", (unsigned int)buffer);
		goto out3;
	}
	memset(buffer, 0, alloc_data.len);

	/* optional: get buffer phys_addr */
	custom_data.cmd = ION_IOC_SUNXI_PHYS_ADDR;
	phys_data.handle = alloc_data.handle;
	custom_data.arg = (unsigned long)&phys_data;
	ret = ioctl(fd, ION_IOC_CUSTOM, &custom_data);
	if(ret) {
		db_msg("###############################ION_IOC_SUNXI_PHYS_ADDR err, ret %d\n", ret);
		goto out1;
	}
	db_msg("ION_IOC_SUNXI_PHYS_ADDR succes, phys_addr 0x%08x, size 0x%08x\n", phys_data.phys_addr, phys_data.size);

	//设置图层参数	
	memset(&info, 0, sizeof(disp_layer_info));

	info.mode	= DISP_LAYER_WORK_MODE_NORMAL;
	info.fb.addr[0]       = (__u32)phys_data.phys_addr; //FB地址
	info.fb.addr[1]		  = (__u32)(info.fb.addr[0] + screen_width * screen_height / 3 * 1);
	info.fb.addr[2]		  = (__u32)(info.fb.addr[0] + screen_width * screen_height / 3 * 2);
	
	info.fb.format        = DISP_FORMAT_ARGB_8888; //DISP_FORMAT_YUV420_P
	info.fb.size.width    = screen_width;
	info.fb.size.height	  = screen_height;
	
	info.fb.src_win.x     = 0;
	info.fb.src_win.y     = 0;
	info.fb.src_win.width = screen_width;
	info.fb.src_win.height= screen_height;

	info.screen_win.x	= 0;
	info.screen_win.y 	= 0;
	info.screen_win.width = screen_width;
	info.screen_win.height = screen_height;
	
	
	info.alpha_mode       = 0; //pixel alpha
	info.alpha_value      = 0;
	info.pipe             = 1;

	arg[0] = 0;//screen_id
	arg[1] = 3;//layer_id
	arg[2] = (unsigned int)&info;
	ret = ioctl(disp, DISP_CMD_LAYER_SET_INFO, (void*)arg);	
	if (ret != 0) {
			db_msg("fail to set layer info\n");
		}

	arg[0] = 0;//screen_id
	arg[1] = 3;//layer_id
	arg[2] = 0;
	arg[3] = 0;
	ret = ioctl(disp, DISP_CMD_LAYER_ENABLE, (void*)arg);
	if (ret != 0) {
		db_msg("fail to layer enable\n");
	}

    /* get draw type */
    if (script_fetch("tp", "draw_type", &draw_type, 1) || 
            (draw_type != 0 && draw_type != 1)) {
        draw_type = 0;
    }

    if (script_fetch("df_view", "tp_draw_color", &tp_draw_color_idx, 1) || 
            tp_draw_color < COLOR_WHITE_IDX || 
            tp_draw_color > COLOR_BLACK_IDX) {
        tp_draw_color_idx = COLOR_WHITE_IDX;
    }
    tp_draw_color = 0xff << 24 | 
                    color_table[tp_draw_color_idx].r << 16 | 
                    color_table[tp_draw_color_idx].g << 8  |
                    color_table[tp_draw_color_idx].b;
    db_msg("tp draw color: 0x%x\n", tp_draw_color);

    return 0;

out4:
	/* unmmap user buffer */
	ret = munmap(buffer, alloc_data.len);
	if(ret)
		db_msg("munmap err, ret %d\n", ret);
out3:
	/* close dmabuf fd */
	close(fd_data.fd);
out2:
	/* free buffer */
	handle_data.handle = alloc_data.handle;
	ret = ioctl(fd, ION_IOC_FREE, &handle_data);
	if(ret)
		db_msg("ION_IOC_FREE err, ret %d\n", ret);
out1:
	/* close ion device */
	close(fd);
	return ret;
}

void tp_track_draw_pixel(int x, int y, unsigned int color)
{
    unsigned int *pixel;

    pixel = (unsigned int *)(buffer + screen_width * y * 4 + x * 4);
    *pixel = color;
}

void tp_track_draw_line(int x1, int y1, int x2, int y2, unsigned int color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int ux = ((dx > 0) << 1) - 1; // x的增量方向，取或-1
    int uy = ((dy > 0) << 1) - 1; // y的增量方向，取或-1
    int x = x1, y = y1, eps;      // eps为累加误差

    eps = 0;
    dx = abs(dx); 
    dy = abs(dy); 
    if (dx > dy) 
    {
        for (x = x1; x != x2; x += ux)
        {
            tp_track_draw_pixel(x, y, color);
            eps += dy;
            if ((eps << 1) >= dx)
            {
                y += uy; eps -= dx;
            }
        }
    }
    else
    {
        for (y = y1; y != y2; y += uy)
        {
            tp_track_draw_pixel(x, y, color);
            eps += dx;
            if ((eps << 1) >= dy)
            {
                x += ux; eps -= dy;
            }
        }
    }             
}

int tp_track_draw(int x, int y, int press)
{
    if (x < 0 || x > screen_width || y < 0 || y > screen_height)
        return -1;

    if (draw_type) {
        tp_track_draw_pixel(x, y, tp_draw_color);
    }
    else {
        if (press == -1) {
            if (pre_x != -1 && pre_y != -1) {
                tp_track_draw_line(pre_x, pre_y, x, y, tp_draw_color);
            }
            pre_x = pre_y = -1;
        }
        else if (press == 0) {
            pre_x = x;
            pre_y = y;
        }
        else if (press == 1) {
            if (pre_x != -1 && pre_y != -1) {
                tp_track_draw_line(pre_x, pre_y, x, y, tp_draw_color);
            }
            pre_x = x;
            pre_y = y;
        }
    }

    return 0;
}

void tp_track_start(int x, int y)
{
    pre_x = x;
    pre_y = y;
}

void tp_track_clear(void)
{
    memset(buffer, 0, screen_width * screen_height * 4);
}
