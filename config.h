#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libconfig.h>
#include "utils.h"

typedef struct {
	int width;
	int height;
	int bpp;
	int size;
	const char* dev_name;
} frame_info_t;

typedef struct {
	uint32_t vendor;
	uint32_t product;
} usb_info_t;

typedef struct {
	frame_info_t	disp;
	frame_info_t	cam;
	usb_info_t		usb;
	uint8_t 			cam_buf_len;
	const char		*rknn_name;
} app_cfg_t;


#define CFG_ERR(x,arg...)	\
	ERR_MSG("[Config Error] " x, ##arg)

int parse_config(const char* filename, app_cfg_t *app_cfg);
void default_config(app_cfg_t *cfg);
void print_config(app_cfg_t cfg);

#endif
