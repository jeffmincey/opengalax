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

#include "opengalax.h"

#define CONFIG_FILE "/etc/opengalax.conf"
#define MAXLEN 1024

static const conf_data default_config = {
	/* serial_device */ "/dev/serio_raw0",
	/* uinput_device */ "/dev/uinput",
	/* rightclick_enable */	0,
	/* rightclick_duration */ 350,
	/* rightclick_range */ 10,
	/* direction */ 0,
	/* psmouse */ 0,
	/* init_device */ 1,
};

static const calibration_data default_calibration = {
	/* xmin */ 0,
	/* xmax */ 2047,
	/* ymin */ 0,
	/* ymax */ 2047,
};

int create_config_file (char* file) {
	FILE* fd;

	fd = fopen(file, "w");
	if (fd == NULL)
		return 0;

	fprintf(fd, "# opengalax configuration file\n");
	fprintf(fd, "\n#### config data:\n");
	fprintf(fd, "serial_device=%s\n", default_config.serial_device);
	fprintf(fd, "uinput_device=%s\n", default_config.uinput_device);
	fprintf(fd, "rightclick_enable=%d\n", default_config.rightclick_enable);
	fprintf(fd, "rightclick_duration=%d\n", default_config.rightclick_duration);
	fprintf(fd, "rightclick_range=%d\n", default_config.rightclick_range);
	fprintf(fd, "# direction: 0 = normal, 1 = invert X, 2 = invert Y, 4 = swap X with Y\n");
	fprintf(fd, "direction=%d\n", default_config.direction);
	fprintf(fd, "# set psmouse=1 if you have a mouse connected into the same port\n");
	fprintf(fd, "# this usually requires i8042.nomux=1 and i8042.reset kernel parameters\n");
	fprintf(fd, "psmouse=%d\n", default_config.psmouse);
	fprintf(fd, "init_device=%d\n", default_config.init_device);
	
	fprintf(fd, "\n#### calibration data:\n");
	fprintf(fd, "# - values should range from 0 to 2047\n");
	fprintf(fd, "# - right/bottom must be bigger than left/top\n");
	fprintf(fd, "# left edge value:\n");
	fprintf(fd, "xmin=%d\n", default_calibration.xmin);
	fprintf(fd, "# right edge value:\n");
	fprintf(fd, "xmax=%d\n", default_calibration.xmax);
	fprintf(fd, "# top edge value:\n");
	fprintf(fd, "ymin=%d\n", default_calibration.ymin);
	fprintf(fd, "# bottom edge value:\n");
	fprintf(fd, "ymax=%d\n", default_calibration.ymax);
	fprintf(fd, "\n");

	fclose(fd);
	return 1;
}

conf_data config_parse (void) {

	char file[MAXLEN];
	char input[MAXLEN], temp[MAXLEN];
	FILE *fd;
	size_t len;
	conf_data config = default_config;

	sprintf( file, "%s", CONFIG_FILE);
	if (!file_exists(file)) {
		if (!create_config_file(file)) {
			fprintf (stderr,"Failed to create default config file: %s\n", file);
			exit (1);
		}
	}

	fd = fopen (file, "r");
	if (fd == NULL) {
		fprintf (stderr,"Could not open configuration file: %s\n", file);
		exit (1);
	}

	while ((fgets (input, sizeof (input), fd)) != NULL) {

		if ((strncmp ("serial_device=", input, 14)) == 0) {
			strncpy (temp, input + 14,MAXLEN-1);
			len=strlen(temp);
			temp[len-1]='\0';
			sprintf(config.serial_device, "%s", temp);
			memset (temp, 0, sizeof (temp));
		}

		if ((strncmp ("uinput_device=", input, 14)) == 0) {
			strncpy (temp, input + 14,MAXLEN-1);
			len=strlen(temp);
			temp[len-1]='\0';
			sprintf(config.uinput_device, "%s", temp);
			memset (temp, 0, sizeof (temp));
		}

		if ((strncmp ("rightclick_enable=", input, 18)) == 0) {
			strncpy (temp, input + 18,MAXLEN-1);
			len=strlen(temp);
			temp[len+1]='\0';
			config.rightclick_enable = atoi(temp);
		}

		if ((strncmp ("rightclick_duration=", input, 20)) == 0) {
			strncpy (temp, input + 20,MAXLEN-1);
			len=strlen(temp);
			temp[len+1]='\0';
			config.rightclick_duration = atoi(temp);
		}

		if ((strncmp ("rightclick_range=", input, 17)) == 0) {
			strncpy (temp, input + 17,MAXLEN-1);
			len=strlen(temp);
			temp[len+1]='\0';
			config.rightclick_range = atoi(temp);
		}

		if ((strncmp ("direction=", input, 10)) == 0) {
			strncpy (temp, input + 10,MAXLEN-1);
			len=strlen(temp);
			temp[len+1]='\0';
			config.direction = atoi(temp);
		}

		if ((strncmp ("psmouse=", input, 8)) == 0) {
			strncpy (temp, input + 8,MAXLEN-1);
			len=strlen(temp);
			temp[len+1]='\0';
			config.psmouse = atoi(temp);
		}

		if ((strncmp ("init_device=", input, 12)) == 0) {
			strncpy (temp, input + 12,MAXLEN-1);
			len=strlen(temp);
			temp[len+1]='\0';
			config.init_device = atoi(temp);
		}

	}

	fclose(fd);
	return config;
}

calibration_data calibration_parse (void) {

	char file[MAXLEN];
	char input[MAXLEN], temp[MAXLEN];
	FILE *fd;
	size_t len;
	calibration_data calibration = default_calibration;

	sprintf( file, "%s", CONFIG_FILE);
	if (!file_exists(file)) {
		if (!create_config_file(file)) {
			fprintf (stderr,"Failed to create default config file: %s\n", file);
			exit (1);
		}
	}

	fd = fopen (file, "r");
	if (fd == NULL) {
		fprintf (stderr,"Could not open configuration file: %s\n", file);
		exit (1);
	}

	while ((fgets (input, sizeof (input), fd)) != NULL) {

		if ((strncmp ("xmin=", input, 5)) == 0) {
			strncpy (temp, input + 5,MAXLEN-1);
			len=strlen(temp);
			temp[len+1]='\0';
			calibration.xmin = atoi(temp);
		}

		if ((strncmp ("xmax=", input, 5)) == 0) {
			strncpy (temp, input + 5,MAXLEN-1);
			len=strlen(temp);
			temp[len+1]='\0';
			calibration.xmax = atoi(temp);
		}

		if ((strncmp ("ymin=", input, 5)) == 0) {
			strncpy (temp, input + 5,MAXLEN-1);
			len=strlen(temp);
			temp[len+1]='\0';
			calibration.ymin = atoi(temp);
		}

		if ((strncmp ("ymax=", input, 5)) == 0) {
			strncpy (temp, input + 5,MAXLEN-1);
			len=strlen(temp);
			temp[len+1]='\0';
			calibration.ymax = atoi(temp);
		}
	}

	fclose(fd);
	return calibration;
}
