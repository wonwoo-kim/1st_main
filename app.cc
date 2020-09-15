#include "app.h"

int app_init(daytime_ctx_t **ctx, const char *cfg_name)
{
	int ret;
	struct mq_attr attr;
	daytime_ctx_t *tmp;

	tmp = (daytime_ctx_t*)malloc(sizeof(daytime_ctx_t));
	if (tmp == NULL) {
		APP_ERR("Failed to allocate memory for daytime_ctx_t\n");
		return -1;
	}
#if 0
	ret = app_parse_config(tmp, cfg_name);
	if (ret != 0) {
		APP_ERR("Failed to parse config\n");
		app_default_config(tmp);
	}
#else
	app_default_config(tmp);
#endif

	tmp->eo.buf = (uint8_t*)malloc(tmp->eo.size);
	tmp->ir.buf = (uint8_t*)malloc(tmp->eo.size);
	tmp->usb.buf = (uint8_t*)malloc(tmp->usb.size);
	tmp->fus.buf = (uint8_t*)malloc(tmp->eo.size);
	tmp->tmpbuf = (uint8_t*)malloc(tmp->eo.size);

	ret = camera_init_helper(tmp->eo.dev_name, &(tmp->eo.fd), 3, 
									tmp->eo.width, tmp->eo.height, V4L2_PIX_FMT_YUV420);
	if (ret != 0) {
		APP_ERR("Failed to initialize camera\n");
		return -1;
	}
#if 1
	ret = usb_host_init(&(tmp->usb.ctx), tmp->usb.vendor, tmp->usb.product);
	if (ret != 0) {
		APP_ERR("Failed to initialize USB host\n");
		return -1;
	}
#endif
	ret = rknn_init_helper(tmp->fus.model_name, &(tmp->fus.ctx));
	if (ret != 0) {
		APP_ERR("Failed to initialize rknn\n");
		return -1;
	}

	ret = drm_init(tmp->disp.dev_name, &(tmp->disp.list));
	if (ret != 0) {
		APP_ERR("Failed to initialize display\n");
		return -1;
	}

	ret = pthread_mutex_init(&tmp->mutex_lock, NULL);
	if (ret != 0) {
		APP_ERR("Failed to initialize mutex\n");
		return -1;
	}

	ret = pthread_cond_init(&tmp->thread_cond, NULL);
	if (ret != 0) {
		APP_ERR("Failed to initialize condition variable\n");
		return -1;
	}

	attr.mq_flags = 0; 
	attr.mq_maxmsg = 100;
	attr.mq_msgsize = 100;
	attr.mq_curmsgs = 0;

	tmp->icd.mfd = mq_open("/my_mq", O_RDWR | O_CREAT, 0666, &attr);
	if (tmp->icd.mfd == -1) {
		APP_ERR("Failed to open message queue\n");
		return -1;
	}

	// PC <-> FUSION
	ret = uart_init_helper(tmp->icd.dev_name, B38400, NULL, &tmp->icd.fd);
	if (ret != 0) {
		APP_ERR("Failed to initialize ICD UART\n");
		return -1;
	}

	// FUSION <-> EO
	ret = uart_init_helper(tmp->eo.uart.dev_name, B38400, NULL, &tmp->eo.uart.fd);
	if (ret != 0) {
		APP_ERR("Failed to initialize ICD UART\n");
		return -1;
	}
#if 0
	// FUSION <-> IR
	ret = uart_init_helper(tmp->ir.uart.dev_name, B38400, NULL, &tmp->ir.uart.fd);
	if (ret != 0) {
		APP_ERR("Failed to initialize ICD UART\n");
		return -1;
	}
#endif
	ret = rga_init_helper();
	if (ret != 0) {
		APP_ERR("Failed to initialize RGA\n");
		return -1;
	}

	*ctx = tmp;

	return 0;
}

int app_run(daytime_ctx_t *ctx)
{


	return 0;
}

int app_parse_config(daytime_ctx_t *ctx, const char *cfg_name)
{
	int ret;
	config_t cfg;

	config_init(&cfg);

	ret = config_read_file(&cfg, cfg_name);
	if (ret == CONFIG_FALSE) {
		APP_ERR("Failed to read config file\n");
		return -1;
	}

	/* Read Display Informaion */
	ret = config_lookup_int(&cfg, "fusion.display.width", (int*)&(ctx->disp.width));
	if (ret == CONFIG_FALSE) {
		APP_ERR("Cannot read display width\n");
		return -1;
	}	

	ret = config_lookup_int(&cfg, "fusion.display.height", (int*)&(ctx->disp.height));
	if (ret == CONFIG_FALSE) {
		APP_ERR("Cannot read display height\n");
		return -1;
	}

	ret = config_lookup_int(&cfg, "fusion.display.bpp", (int*)&(ctx->disp.bpp));
	if (ret == CONFIG_FALSE) {
		APP_ERR("Cannot read display bpp\n");
		return -1;
	}

	ret = config_lookup_string(&cfg, "fusion.display.device", &(ctx->disp.dev_name));
	if (ret == CONFIG_FALSE) {
		APP_ERR("Cannot read display device name\n");
		return -1;
	}

	ctx->disp.size = ctx->disp.bpp * ctx->disp.width * ctx->disp.height;

	
	/* Read Camera Informaion */
	ret = config_lookup_int(&cfg, "fusion.camera.width", (int*)&(ctx->eo.width));
	if (ret == CONFIG_FALSE) {
		APP_ERR("Cannot read camera width\n");
		return -1;
	}	
	
	ret = config_lookup_int(&cfg, "fusion.camera.height", (int*)&(ctx->eo.height));
	if (ret == CONFIG_FALSE) {
		APP_ERR("Cannot read camera height\n");
		return -1;
	}

	ret = config_lookup_int(&cfg, "fusion.camera.bpp", (int*)&(ctx->eo.bpp));
	if (ret == CONFIG_FALSE) {
		APP_ERR("Cannot read camera bpp\n");
		return -1;
	}

	ret = config_lookup_string(&cfg, "fusion.camera.device", &(ctx->eo.dev_name));
	if (ret == CONFIG_FALSE) {
		APP_ERR("Cannot read camera device name\n");
		return -1;
	}

	ctx->eo.size = ctx->eo.bpp * ctx->eo.width * ctx->eo.height;

	/* Read USB Information */
	ret = config_lookup_int(&cfg, "fusion.usb.device_vendor", (int*)&(ctx->usb.vendor));
	if (ret == CONFIG_FALSE) {
		APP_ERR("Cannot read usb vendor code\n");
		return -1;
	}

	ret = config_lookup_int(&cfg, "fusion.usb.device_product", (int*)&(ctx->usb.product));
	if (ret == CONFIG_FALSE) {
		APP_ERR("Cannot read usb product code\n");
		return -1;
	}

	/* Read RKNN Information */
	ret = config_lookup_string(&cfg, "fusion.rknn.model_name", &(ctx->fus.model_name));
	if (ret == CONFIG_FALSE) {
		APP_ERR("Cannot read rknn model filename\n");
		return -1;
	}

	return 0;
}

int app_default_config(daytime_ctx_t *ctx)
{
	ctx->eo.width	= 1024;
	ctx->eo.height	= 798;
	ctx->eo.bpp 	= 2;
	ctx->eo.format = V4L2_PIX_FMT_YUV420;
	ctx->eo.size	= ctx->eo.width * ctx->eo.height * ctx->eo.bpp;
	ctx->eo.dev_name = "/dev/video0";

	ctx->ir.width	= 640;
	ctx->ir.height	= 480;
	ctx->ir.bpp 	= 2;
	ctx->ir.size	= ctx->ir.width * ctx->ir.height * ctx->ir.bpp;

	ctx->disp.width	= 1920;
	ctx->disp.height	= 1080;
	ctx->disp.bpp 		= 4;
	ctx->disp.format	= RK_FORMAT_BGRA_8888;
	ctx->disp.size		= ctx->disp.width * ctx->disp.height * ctx->disp.bpp;
	ctx->disp.dev_name = "/dev/dri/card0";

	ctx->usb.vendor	= 0x1d6b;
	ctx->usb.product	= 0x0105;
	ctx->usb.size		= ctx->ir.size / ctx->ir.bpp;

	ctx->fus.model_name	= "deepfusion.rknn";
	ctx->fus.size			= ctx->eo.width * ctx->eo.height;
	ctx->fus.crop_y		= 0;
	
	ctx->icd.dev_name	= "/dev/ttyS2";
	ctx->eo.uart.dev_name	= "/dev/ttyS3";
	ctx->ir.uart.dev_name	= "/dev/ttyS2";
}
