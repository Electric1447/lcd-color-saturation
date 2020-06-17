/*
 * Copyright (c) 2020 Itai Levin (Electric1447)
 */
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include <string.h>

#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/registrymgr.h>
#include <vita2d.h>


#define BLACK   RGBA8(  0,   0,   0, 255)
#define WHITE   RGBA8(255, 255, 255, 255)
#define DGREY   RGBA8( 55,  55,  55, 255)
#define LGREY   RGBA8(191, 191, 191, 255)
#define CYAN    RGBA8(  0, 255, 255, 255)
#define RED     RGBA8(255,   0,   0, 255)
#define GREEN   RGBA8(  0, 255,   0, 255)
#define ORANGE  RGBA8(255, 165,   0, 255)


#define REG_DISPLAY_CATEGORY "/CONFIG/DISPLAY/"
#define REG_KEY_CSM "color_space_mode"

#define IDLE 		0
#define INSTALLED 	1
#define ERROR 		2

#define STATUS_INSTALLED 		1
#define STATUS_NOT_INSTALLED 	0


int status_color[] = {RED, GREEN};

char *status_text[] = {
	"NOT INSTALLED (stock)",
	"INSTALLED"
};

int state = IDLE;
int csm;


/*
 * Get registery key
 */
int regMgrGetKeyInt(char *path, char *keyName) {
	int val = -1, ret = 0;

	ret = sceRegMgrGetKeyInt(path, keyName, &val);

	if (ret < 0)
		return ret;
	else
		return val;
}

/*
 * Change the lcd color_space_mode
 */
void changeLcdMode() {
	sceRegMgrSetKeyInt(REG_DISPLAY_CATEGORY, REG_KEY_CSM, 1 - csm);
	sceAVConfigSetDisplayColorSpaceMode(1 - csm);
	sceKernelDelayThread(300 * 1000);
}

int main(int argc, char *argv[]) {
	SceCtrlData pad;
	
	vita2d_init();
	vita2d_set_clear_color(BLACK);
	vita2d_pgf *pgf;
	pgf = vita2d_load_default_pgf();
	vita2d_texture *colortest = vita2d_load_PNG_file("app0:/sce_sys/livearea/contents/startup.png");
	
	// Main loop
	while(1) {
		
		// Get registry key
		csm = regMgrGetKeyInt(REG_DISPLAY_CATEGORY, REG_KEY_CSM); // color_space_mode
		
		sceCtrlPeekBufferPositive(0, &pad, 1);
		vita2d_start_drawing();
		vita2d_clear_screen();
		
		if (pad.buttons & SCE_CTRL_CROSS)
			changeLcdMode();
	
		vita2d_draw_rectangle(0, 0, 960, 54, DGREY);
		vita2d_draw_texture(colortest, 680, 386);
		
		vita2d_pgf_draw_text(pgf, 20,  36, WHITE, 1.2f, "Vita 2000 LCD Color Saturation VPK by Electric1447");
		vita2d_pgf_draw_text(pgf, 20, 524, LGREY, 1.0f, "https://github.com/Electric1447/lcd-color-saturation");
		
		vita2d_pgf_draw_text(pgf,  20,  94, ORANGE, 1.0f, "Will only make a difference for Vita 2000 (Slim)");
		vita2d_pgf_draw_textf(pgf, 20, 124, status_color[csm], 1.0f, "LCD status: %s", status_text[csm]);
		vita2d_pgf_draw_textf(pgf, 20, 164, CYAN, 1.0f, "%s   =  %d", REG_KEY_CSM, csm);
		
		vita2d_pgf_draw_text(pgf, 20, 240, WHITE, 1.0f, "Press X to install\nPress O to exit.");
		
		vita2d_end_drawing();
		vita2d_swap_buffers();

		if (pad.buttons & SCE_CTRL_CIRCLE) 
			break;
	}
	
	vita2d_fini();
	vita2d_free_pgf(pgf);
	
	sceKernelExitProcess(0);
	return 0;
}
