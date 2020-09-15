#include "config.h"

int parse_config(const char* filename, app_cfg_t *app_cfg)
{
	int ret;
	config_t cfg;

	config_init(&cfg);

	ret = config_read_file(&cfg, filename);
	if ( !ret )	{
		CFG_ERR("Failed to read configuration file\n");
		return -1;
	}

/* Read Display Informaion */
	ret = config_lookup_int(&cfg, "fusion.display.width", &(app_cfg->disp.width));
	if (!ret) {
		CFG_ERR("Cannot read display width\n");
		return -1;
	}	

	ret = config_lookup_int(&cfg, "fusion.display.height", &(app_cfg->disp.height));
	if (!ret) {
		CFG_ERR("Cannot read display height\n");
		return -1;
	}

	ret = config_lookup_int(&cfg, "fusion.display.bpp", &(app_cfg->disp.bpp));
	if (!ret) {
		CFG_ERR("Cannot read display bpp\n");
		return -1;
	}

	ret = config_lookup_string(&cfg, "fusion.display.device", &(app_cfg->disp.dev_name));
	if (!ret) {
		CFG_ERR("Cannot read display device name\n");
		return -1;
	}

	app_cfg->disp.size = app_cfg->disp.bpp * app_cfg->disp.width * app_cfg->disp.height;

	
	/* Read Camera Informaion */
	ret = config_lookup_int(&cfg, "fusion.camera.width", &(app_cfg->cam.width));
	if (!ret) {
		CFG_ERR("Cannot read camera width\n");
		return -1;
	}	
	
	ret = config_lookup_int(&cfg, "fusion.camera.height", &(app_cfg->cam.height));
	if (!ret) {
		CFG_ERR("Cannot read camera height\n");
		return -1;
	}

	ret = config_lookup_int(&cfg, "fusion.camera.bpp", &(app_cfg->cam.bpp));
	if (!ret) {
		CFG_ERR("Cannot read camera bpp\n");
		return -1;
	}

	ret = config_lookup_string(&cfg, "fusion.camera.device", &(app_cfg->cam.dev_name));
	if (!ret) {
		CFG_ERR("Cannot read camera device name\n");
		return -1;
	}

	app_cfg->cam.size = app_cfg->cam.bpp * app_cfg->cam.width * app_cfg->cam.height;

	/* Read USB Information */
	ret = config_lookup_int(&cfg, "fusion.usb.device_vendor", (int*)&(app_cfg->usb.vendor));
	if (!ret) {
		CFG_ERR("Cannot read usb vendor code\n");
		return -1;
	}

	ret = config_lookup_int(&cfg, "fusion.usb.device_product", (int*)&(app_cfg->usb.product));
	if (!ret) {
		CFG_ERR("Cannot read usb product code\n");
		return -1;
	}

	/* Read RKNN Information */
	ret = config_lookup_string(&cfg, "fusion.rknn.model_name", &(app_cfg->rknn_name));
	if (!ret) {
		CFG_ERR("Cannot read rknn model filename\n");
		return -1;
	}

	return 0;
}

void default_config(app_cfg_t *cfg)
{
	cfg->disp.width = 720;
	cfg->disp.height = 1280;
	cfg->disp.bpp = 2;
	cfg->disp.size = 720 * 1280 * 2;
	cfg->disp.dev_name = "/dev/dri/card0";

	cfg->cam.width = 640;
	cfg->cam.height = 480;
	cfg->cam.bpp = 2;
	cfg->cam.size = 640 * 480 * 2;
	cfg->cam.dev_name = "/dev/video0";

	cfg->usb.vendor = 0x1d6b;
	cfg->usb.product = 0x0105;

	cfg->rknn_name = "./model.rknn";
	cfg->cam_buf_len = 3;
}

void print_config(app_cfg_t cfg)
{
	printf("/////////////// CONFIG INFO ///////////////\n");
	printf("Display device name : \t%s\n", cfg.disp.dev_name);
	printf("Display width : \t%d\n", cfg.disp.width);
	printf("Display height : \t%d\n", cfg.disp.height);
	printf("Display BPP : \t\t%d\n\n", cfg.disp.bpp);

	printf("Camera device name : \t%s\n", cfg.cam.dev_name);
	printf("Camera width : \t\t%d\n", cfg.cam.width);
	printf("Camera height : \t%d\n", cfg.cam.height);
	printf("Camera BPP : \t\t%d\n\n", cfg.cam.bpp);

	printf("USB device vendor : \t0x%04x\n", cfg.usb.vendor);
	printf("USB device product : \t0x%04x\n\n", cfg.usb.product);

	printf("RKNN model filename : \t%s\n", cfg.rknn_name);
	printf("///////////////////////////////////////////\n");
}
