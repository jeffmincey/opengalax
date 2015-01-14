/*
 *   opengalax touchscreen daemon
 *   Copyright 2012 Pau Oliva Fora <pof@eslack.org>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation; either version 2
 *   of the License, or (at your option) any later version.
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/uinput.h>
#include <sys/stat.h>

#define XA_MAX 0xF
#define YA_MAX 0xF

#define XB_MAX	0x7F
#define YB_MAX	0x7F

#define X_AXIS_MAX (XA_MAX+1)*(XB_MAX+1)
#define Y_AXIS_MAX (YA_MAX+1)*(YB_MAX+1)

#define CMD_OK 0xFA
#define CMD_ERR 0xFE

#define PRESS 0x81
#define RELEASE 0x80

#define BTN1_RELEASE 0
#define BTN1_PRESS 1
#define BTN2_RELEASE 2
#define BTN2_PRESS 3

#define DEBUG 1

#define die(str, args...) do { \
	perror(str); \
	exit(EXIT_FAILURE); \
} while(0)

/* config file */
typedef struct {
	char serial_device[1024];
	char uinput_device[1024];
	int rightclick_enable;
	int rightclick_duration;
	int rightclick_range;
	int direction;
	int psmouse;
	int init_device;
} conf_data;

typedef struct {
	int xmin;
	int xmax;
	int ymin;
	int ymax;
} calibration_data;

int fd_serial, fd_uinput;
struct uinput_user_dev uidev;
int use_psmouse;

/* configfile.c */
int create_config_file (char* file);
conf_data config_parse (void);
calibration_data calibration_parse (void);

/* functions.c */
int running_as_root (void);
int time_elapsed_ms (struct timeval *start, struct timeval *end, int ms); 
int configure_uinput (void);
int setup_uinput (void);
int setup_uinput_dev (const char *ui_dev);
int open_serial_port (const char *fd_device); 
int init_panel (); 
void initialize_panel (int sig);
void signal_handler (int sig);
void signal_installer (void);
int file_exists (char *file);
char* default_pid_file (void); 
int create_pid_file (void); 
int remove_pid_file (void);

/* psmouse.c */

void uinput_open(const char *uinput_dev_name); 
int psmouse_connect();
void uinput_create(); 
int phys_wait_for_input(int *ptimeout); 
void psmouse_interrupt(unsigned char data);
void uinput_destroy();
void uinput_close();
void psmouse_disconnect();


/*

init sequence:
 	send: f5 f3 0a f3 64 f3 c8 f4
 	read: fa fa fa fa fa fa fa fa

return values:
	0xFA == OK
	0xFE == ERROR

Example PDUs:
	81 0F 3D 05 06
	80 0F 3D 05 06

byte 0:
	0x80 == PRESS
	0x81 == RELEASE

byte 1:
	X axis value, from 0 to 0x0F
byte 2:
	X axis value, from 0 to 0x7F

byte 3:
	Y axis value, from 0 to 0x0F
byte 4:
	Y axis value, from 0 to 0x7F

*/
