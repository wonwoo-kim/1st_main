#ifndef _FUSION_H
#define _FUSION_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <arm_neon.h>

#include "utils.h"
#include "configs.h"
#include "uart_helper.h"
#include "rknn_helper.h"

#define QUEUE_SIZE 20

/*
 * CL_Engine_Datasheet_Ver2.2(적외선검출기 640 17um).pdf 참조
 *	+----------------------------------------------------------------+
 * | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 |  Byte 7  |
 *	+----------------------------------------------------------------+
 * |  0xFF  |  0x00  |     Address     |       Data      | Checksum |
 *	+----------------------------------------------------------------+
 *	Checksum = Add Byte2 ~ Byte 6 
 */ 

typedef struct {
	uint8_t sync;
	uint8_t address;
	uint8_t cmd;
	uint8_t cmd2;
	uint8_t data;
	uint8_t data2;
	uint8_t checksum;
} __attribute__((packed)) ir_pkt;

/**
* @brief I/F Board -> Fusion Board command structure
*/


typedef struct {
	uint8_t header[3];
	struct {
		uint8_t header[2];
		uint8_t address[1];
		uint8_t data[4];
	} __attribute__((packed)) msg;
	uint8_t tail[2];
} __attribute__((packed)) uplink_cmd_t;
#if 0
typedef union {
	// Packed Message
	uint8_t header[3];
	uint8_t message[7];
	uint8_t tail[2];
	
	// Bytestream Message
	uint8_t raw[12];
} __attribute__((packed))
#endif
/**
* @brief Fusion Board -> I/F Board command structure
*/
typedef uplink_cmd_t downlink_cmd_t;

const uint8_t UPLINK_HEADER[3] = {0xA1, 0xB2, 0x01};

const uint8_t DOWNLINK_HEADER[3] = {0xA1, 0xB2, 0xC3};


/////////////// UP-LINK (I/F -> FUSION) //////////////
typedef enum
{
	FUSION = 0xFE,
	IR = 0xF0,
	VISIBLE = 0xFF
} ID;

typedef enum
{
	VIDEO_OUTPUT_MODE = 0x01,
	VIDEO_OUTPUT_METHOD = 0x02,
	OSD_ENABLE = 0x03,
	DATA_LOGGING = 0x04,
	REGISTRATION_Y = 0x10,
	REGISTRATION_X = 0x11,
	REGISTRATION_SAVE = 0x14,
	READ_REGISTRATION_Y = 0x15,
	READ_REGISTRATION_X = 0x16,
	FUSION_LEVEL_AUTO = 0x30,
	SET_FUSION_LEVEL = 0x31,
	REQ_SELF_TEST = 0x50,
	VERTICAL_FLIP = 0x60,
	HORIZONTAL_FLIP = 0x61,
	OUTPUT_RESOLUTION = 0x70,

	POSITIVE_ACK,
	NAGATIVE_ACK
} UPLINK_ADDRESS_TYPE;

typedef enum 
{
	FUSION_SELECT	= 0x00,
	VISIBLE_SELECT	= 0x01,
	IR_SELECT		= 0x02
} DEVICE_TYPE;

typedef enum
{
	DIGITAL_OUTPUT	= 0x00,
	ANALOG_OUTPUT	= 0x01,
	DIGITAL_ANALOG_OUTPUT = 0x02
} VIDEO_OUTPUT_TYPE;

// OSD_ENABLE
// DATA_LOGGING

typedef enum
{
	FUSION_MANUAL	= 0x00,
	FUSION_AUTO		= 0x01
} FUSION_METHOD_TYPE;

// FUSION_LEVEL

typedef enum
{
	OFF = 0x00,
	ON = 0x01
} ON_OFF;

typedef enum
{
	USE_FUSION		= 0x00,
	USE_VISIBLE		= 0x01,
	USE_IR			= 0x02,
} FUSION_TYPE;


typedef struct 
{
	rknn_context fusion_model;

	uint8_t *eo;
	uint8_t *ir;
	uint8_t *out;

	uint16_t width;
	uint16_t height;

	uint16_t crop_y;
} fus_ctx_t;

int fusion_eo_ir(fus_ctx_t *ctx, float level, int flag);
int fusion_npu(fus_ctx_t *ctx, float level);
int fusion_legacy(uint8_t *eo, uint8_t *ir, uint8_t *out,
						size_t width, size_t height, size_t crop_y, uint8_t level);

void icd_init(const char *device, speed_t baudrate);
void icd_callback(int status);
void icd_deinit(void);
void queue_write(void *buf, ssize_t bytes);
int check_cmd(uint8_t *queue, ssize_t size);

#endif
