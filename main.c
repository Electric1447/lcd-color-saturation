/*
 * Copyright (c) 2020 Itai Levin (Electric1447)
 */
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include <psp2/apputil.h>
#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/shellutil.h> 
#include <psp2/types.h>
#include <psp2/registrymgr.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vita2d.h>


#define BLACK   RGBA8(  0,   0,   0, 255)
#define WHITE   RGBA8(255, 255, 255, 255)
#define DGREY   RGBA8( 55,  55,  55, 255)
#define LGREY   RGBA8(191, 191, 191, 255)
#define CYAN    RGBA8(  0, 255, 255, 255)
#define RED     RGBA8(255,   0,   0, 255)
#define GREEN   RGBA8(  0, 255,   0, 255)
#define YELLOW  RGBA8(255, 255,   0, 255)
#define ORANGE  RGBA8(255, 165,   0, 255)


#define REG_DISPLAY_CATEGORY "/CONFIG/DISPLAY/"
#define REG_KEY_CSM "color_space_mode"
#define REG_KEY_RRM "rgb_range_mode"

#define IDLE 		0
#define INSTALLED 	1
#define ERROR 		2

#define STATUS_INSTALLED 		2
#define STATUS_NOT_INSTALLED 	0
#define STATUS_PARTIAL 			1


int status_color[] = {RED, YELLOW, GREEN};

char *status_text[] = {
	"NOT INSTALLED (stock)",
	"PARTIAL",
	"INSTALLED"
};

int state = IDLE;
int csm, rrm;


void initSceAppUtil() {
	
	// Init SceAppUtil
	SceAppUtilInitParam init_param;
	SceAppUtilBootParam boot_param;
	memset(&init_param, 0, sizeof(SceAppUtilInitParam));
	memset(&boot_param, 0, sizeof(SceAppUtilBootParam));
	sceAppUtilInit(&init_param, &boot_param);

}

int regMgrGetKeyInt(char *path, char *keyName) {
	int val = -1, ret = 0;

	ret = sceRegMgrGetKeyInt(path, keyName, &val);

	if (ret < 0)
		return ret;
	else
		return val;
}

/*
 * Change the lcd color_space_mode & rgb_range_mode.
 */
void changeLcdMode(int status) {
	switch (status) {
		case STATUS_PARTIAL:
		case STATUS_NOT_INSTALLED:
			sceRegMgrSetKeyInt(REG_DISPLAY_CATEGORY, REG_KEY_CSM, 1);
			sceRegMgrSetKeyInt(REG_DISPLAY_CATEGORY, REG_KEY_RRM, 1);
			break;
		case STATUS_INSTALLED:
			sceRegMgrSetKeyInt(REG_DISPLAY_CATEGORY, REG_KEY_CSM, 0);
			sceRegMgrSetKeyInt(REG_DISPLAY_CATEGORY, REG_KEY_RRM, 0);
			break;
	}
	
	state = INSTALLED;
	sceKernelDelayThread(300 * 1000);
}

int main(int argc, char *argv[]) {
    initSceAppUtil();
	SceCtrlData pad;
	
	vita2d_init();
	vita2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0xFF));
	
	vita2d_pgf *pgf;
	pgf = vita2d_load_default_pgf();
	
	// Get registry keys
	csm = regMgrGetKeyInt(REG_DISPLAY_CATEGORY, REG_KEY_CSM); // color_space_mode
	rrm = regMgrGetKeyInt(REG_DISPLAY_CATEGORY, REG_KEY_RRM); // rgb_range_mode
	
	int status = csm + rrm;
	
	// Check if there are any errors 
	if (csm < 0 || rrm < 0 || csm >= 2 || rrm >= 2)
		state = ERROR;
	
	// Main loop
	while(1) {
        sceCtrlPeekBufferPositive(0, &pad, 1);
        vita2d_start_drawing();
        vita2d_clear_screen();
		
		if ((pad.buttons & SCE_CTRL_CROSS) && (state != ERROR)) {
			if (!state) changeLcdMode(status);
			else scePowerRequestColdReset();
		}
	
		vita2d_draw_rectangle(0, 0, 940, 54, DGREY);
		
		vita2d_pgf_draw_text(pgf, 20,  36, RGBA8(255,255,255,255), 1.2f, "Vita 2000 LCD Color Saturation VPK by Electric1447");
		vita2d_pgf_draw_text(pgf, 20, 524, LGREY, 1.0f, "https://github.com/Electric1447/lcd-color-saturation");
		
		switch (state) {
			case IDLE:
				vita2d_pgf_draw_textf(pgf, 20, 94, status_color[status], 1.0f, "LCD status: %s", status_text[status]);

				vita2d_pgf_draw_textf(pgf,  20, 134, CYAN, 1.0f, "%s", REG_KEY_CSM);
				vita2d_pgf_draw_textf(pgf, 240, 134, CYAN, 1.0f, "=    %d", csm);
				
				vita2d_pgf_draw_textf(pgf,  20, 154, CYAN, 1.0f, "%s", REG_KEY_RRM);
				vita2d_pgf_draw_textf(pgf, 240, 154, CYAN, 1.0f, "=    %d", rrm);
				
				vita2d_pgf_draw_text(pgf, 20, 220, GREEN, 1.0f, !status ? "Ready to install" : "Ready to revert back to stock");
				
				vita2d_pgf_draw_text(pgf, 20, 260, WHITE, 1.0f, "Press X to install");
				vita2d_pgf_draw_text(pgf, 20, 280, WHITE, 1.0f, "Press O to exit");
				break;
			case INSTALLED:
				vita2d_pgf_draw_textf(pgf, 20, 94, status_color[status], 1.0f, "LCD status: %s", status_text[status]);

				vita2d_pgf_draw_textf(pgf,  20, 134, CYAN, 1.0f, "%s", REG_KEY_CSM);
				vita2d_pgf_draw_textf(pgf, 240, 134, CYAN, 1.0f, "=    %d", csm);
				
				vita2d_pgf_draw_textf(pgf,  20, 154, CYAN, 1.0f, "%s", REG_KEY_RRM);
				vita2d_pgf_draw_textf(pgf, 240, 154, CYAN, 1.0f, "=    %d", rrm);
				
				vita2d_pgf_draw_textf(pgf, 20, 220, GREEN, 1.0f, "%s. please restart for changes to take effect.", !status ? "Installation successful" : "Uninstalled successfully");
				
				vita2d_pgf_draw_text(pgf, 20, 260, WHITE, 1.0f, "Press X to restart");
				vita2d_pgf_draw_text(pgf, 20, 280, WHITE, 1.0f, "Press O to exit");
				break;
			case ERROR:
				vita2d_pgf_draw_text(pgf, 20, 94, RED, 1.0f, "REGISTRY ERROR!");

				vita2d_pgf_draw_textf(pgf,  20, 134, ORANGE, 1.0f, "%s", REG_KEY_CSM);
				vita2d_pgf_draw_textf(pgf, 240, 134, ORANGE, 1.0f, "=    %d", csm);
				
				vita2d_pgf_draw_textf(pgf,  20, 154, ORANGE, 1.0f, "%s", REG_KEY_RRM);
				vita2d_pgf_draw_textf(pgf, 240, 154, ORANGE, 1.0f, "=    %d", rrm);
				
				vita2d_pgf_draw_text(pgf, 20, 260, WHITE, 1.0f, "Press O to exit");
				break;
		}
		
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
