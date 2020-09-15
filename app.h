#ifndef _APP_H_
#define _APP_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <mqueue.h>

#include "rknn_helper.h"
#include "drm_module.h"
#include "camera_helper.h"
#include "usb_host_helper.h"
#include "utils.h"
#include "rga_helper.h"
#include "uart_helper.h"
#include "fusion.h"
#include "config.h"
#include "configs.h"

extern "C" {
#include "device_name.h"
};

#define APP_ERR(x,arg...)	\
	ERR_MSG("[Application Error] " x, ##arg)

/*
 *
 *
 *
 *
 *
 *
 */
struct __attribute__ ((packed)) buff_info {
	uint32_t bytes;
	void *addr;
};

typedef struct {
	int fd;
	struct sockaddr_nl src_addr, dst_addr;
	struct nlmsghdr *nlh;
	struct iovec iov;
	struct msghdr msg;
} netlink_ctx_t;

typedef struct {
	int				fd;
	mqd_t				mfd;
	const char*		dev_name;
} icd_ctx_t;

typedef struct {
	size_t		width, height, bpp, size;
	const char*	dev_name;
	int			fd;
	int			format;
	drm_dev_t	*list;
} disp_ctx_t;

typedef struct {
	size_t		width, height, bpp, size;
	const char*	dev_name;
	uint8_t		*buf;
	int			format;
	int			fd;

	icd_ctx_t	uart;
} cam_ctx_t;

typedef struct {
	usb_state_t	ctx;

	size_t		size;
	uint8_t		*buf;
	uint16_t		vendor, product;
} host_ctx_t;

typedef struct {
	uint8_t	 		*buf;
	size_t	 		size;
	size_t			crop_y;
	rknn_context	ctx;
	const char*		model_name;
} fusion_ctx_t;

typedef struct {
	disp_ctx_t		disp;
	cam_ctx_t		eo;
	cam_ctx_t		ir;
	host_ctx_t		usb;
	fusion_ctx_t	fus;
	icd_ctx_t		icd;
	netlink_ctx_t	nl;

	uint8_t			*tmpbuf;

	pthread_cond_t thread_cond;
	pthread_mutex_t mutex_lock;

	pthread_t cam_thread;
	pthread_t disp_thread;
	pthread_t usb_thread;
	pthread_t icd_thread;
	pthread_t cmd_thread;
} daytime_ctx_t;

/**
* @brief Initialize Application Context
*
* @param ctx		context pointer to initilize
* @param cfg_name	configuration file name
*
* @return Initialize successful return 0, Otherwise return -1
*/
int app_init(daytime_ctx_t **ctx, const char *cfg_name);

/**
* @brief Read & Parse Application Configuration
*
* @param ctx		context pointer to save configuration
* @param cfg_name	filename to read/parse
*
* @return Read/Parse Successful return 0, Otherwise return non-zero
*/
int app_parse_config(daytime_ctx_t *ctx, const char *cfg_name);



int app_default_config(daytime_ctx_t *ctx);

#endif
