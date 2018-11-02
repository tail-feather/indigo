/**************************************************************
	ASCOL telescope control library

	(C)2018 by Rumen G.Bogdanovski
***************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libascol.h>

int ascol_debug = 0;
#define ASCOL_DEBUG(...) (ascol_debug && printf(__VA_ARGS__))
#define ASCOL_DEBUG_WRITE(res, cmd) (ASCOL_DEBUG("%s()=%2d --> %s", __FUNCTION__, res, cmd))
#define ASCOL_DEBUG_READ(res, resp) (ASCOL_DEBUG("%s()=%2d <-- %s\n", __FUNCTION__, res, resp))

static const char *oimv_descriptions[] = {
	"Left side pressure",
	"Right side pressure",
	"Left Segment 1 pressure",
	"Left Segment 2 pressure",
	"Left Segment 3 pressure",
	"Left Segment 4 pressure",
	"Left Segment 5 pressure",
	"Right Segment 1 pressure",
	"Right Segment 2 pressure",
	"Right Segment 3 pressure",
	"Right Segment 4 pressure",
	"Right Segment 5 pressure",
	"Nitrogen pressure",
	"Level of oil in \"IN\" tank",
	"Level of oil in \"OUT\" tank",
	"Temperature of oil in \"IN\" tank",
	"Temperature of motor M25"
};

static const char *oimv_units[] = {
	"bar",
	"bar",
	"bar",
	"bar",
	"bar",
	"bar",
	"bar",
	"bar",
	"bar",
	"bar",
	"bar",
	"bar",
	"bar",
	"%",
	"%",
	"deg C",
	"deg C"
};

static const char *glme_descriptions[] = {
	"Outdoor temperature",
	"Outdoor pressure",
	"Outdoor humidity",
	"Dew point",
	"Dome temperature",
	"Primary mirror temperature",
	"Secondary mirror temperature",
};

static const char *glme_units[] = {
	"deg C",
	"hPa",
	"%rh",
	"deg C",
	"deg C",
	"deg C",
	"deg C"
};

/* Oil state descriptions */
static const char *oil_state_descr_s[] = {
	"OFF",
	"START1",
	"START2",
	"START3",
	"ON",
	"OFF_DELAY"
};

static const char *oil_state_descr_l[] = {
	"Off",
	"Checking pressure of nitrogen",
	"Starting oil pump",
	"Stabilization of oil pressure",
	"On",
	"Time before stop"
};

/* Telescope state descriptions */
static const char *telescope_state_descr_s[] = {
	"INIT",
	"OFF",
	"OFF_WAIT",
	"STOP",
	"TRACK",
	"OFF_REQ",
	"SS_CLU1",
	"SS_SLEW",
	"SS_CLU2",
	"SS_CLU3",
	"ST_DECC1",
	"ST_CLU1",
	"ST_SLEW",
	"ST_DECC2",
	"ST_CLU2",
	"ST_DECC3",
	"ST_CLU3",
	"SS_DECC3",
	"SS_DECC2"
};

static const char *telescope_state_descr_l[] = {
	"initialization of sensors",
	"Off",
	"Wait on start converter",
	"Stop",
	"Tracking",
	"Time before off",
	"Clutch 1",
	"Slew to source coordinates",
	"Clutch 2",
	"Clutch 3",
	"Lateral inhibition before descent on sky coordinates",
	"Clutch 1",
	"Slew to sky coordinates",
	"Lateral inhibition",
	"Clutch 2",
	"Lateral inhibition after break-in descent",
	"Clutch 3",
	"Lateral inhibition after break-in descent",
	"Lateral inhibition"
};


static size_t strncpy_n(char *dest, const char *src, size_t n){
	size_t i;

	for (i = 0 ; i < n && src[i] != '\0' ; i++)
		dest[i] = src[i];

	if (i + 1 < n) dest[i+1] = '\0';

	return i;
 }


/* Utility functions */

int ascol_parse_devname(char *device, char *host, int *port) {
	char *strp;
	int n;

	n = sscanf(device,"tcp://%s",host);
	if (n < 1) {
		n = sscanf(device,"ascol://%s",host);
		if (n < 1) {
			sscanf(device,"%s",host);
		}
	}

	strp = host;
	strsep(&strp, ":");
	if (strp == NULL) {
		*port = DEFAULT_PORT;
	} else {
		*port = atoi(strp);
	}
	return 1;
}


int ascol_open(char *host, int port) {
	struct sockaddr_in srv_info;
	struct hostent *he;
	int sock;
	struct timeval timeout;

	timeout.tv_sec = 6;
	timeout.tv_usec = 0;

	if ((he = gethostbyname(host))==NULL) {
		return -1;
	}

	if ((sock = socket(AF_INET, SOCK_STREAM, 0))== -1) {
		return -1;
	}

	memset(&srv_info, 0, sizeof(srv_info));
	srv_info.sin_family = AF_INET;
	srv_info.sin_port = htons(port);
	srv_info.sin_addr = *((struct in_addr *)he->h_addr);
	if (connect(sock, (struct sockaddr *)&srv_info, sizeof(struct sockaddr))<0) {
		return -1;
	}

	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
		close(sock);
		return -1;
	}

	if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
		close(sock);
		return -1;
	}
	return sock;
}


int ascol_read(int devfd, char *reply, int len) {
	char c;
	int res;
	int count=0;

	while ((count < len) && ((res=read(devfd,&c,1)) != -1 )) {
		if (res == 1) {
			reply[count] = c;
			count++;
			if ((c == '\n') || (c == '\r')) {
				reply[count-1] = '\0';
				return count;
			}
		} else {
			return -1;
		}
	}
	return -1;
}


int ascol_get_oil_state(ascol_glst_t glst, char **long_descr, char **short_descr) {
	if ((glst.oil_state < 0) || (glst.oil_state > 5)) return ASCOL_PARAM_ERROR;
	if (long_descr) *long_descr = (char*)oil_state_descr_l[glst.oil_state];
	if (short_descr) *short_descr = (char*)oil_state_descr_s[glst.oil_state];
	return ASCOL_OK;
}

int ascol_get_telescope_state(ascol_glst_t glst, char **long_descr, char **short_descr) {
	if ((glst.telescope_state < 0) || (glst.telescope_state > 18)) return ASCOL_PARAM_ERROR;
	if (long_descr) *long_descr = (char*)telescope_state_descr_l[glst.telescope_state];
	if (short_descr) *short_descr = (char*)telescope_state_descr_s[glst.telescope_state];
	return ASCOL_OK;
}


/* Convert ASCOL HHMMSS.SS and DDMMSS.SS format in decimal degrees */

int ascol_dms2dd(double *dd, const char *dms) {
	int i;
	double deg, min, sec, sign = 1;
	char *buff, *b1;
	char buff1[3];

	buff = (char*)dms;    //clear the spaces
	while (isspace(buff[0])) buff++;
	i = strlen(buff)-1;
	while (isspace(buff[i])) i--;
	buff[i+1] = '\0';

	if (buff[0] == '-') { sign = -1; buff++; }
	if (buff[0] == '+') buff++;

	if (2 != strncpy_n(buff1, buff, 2)) return -1;
	buff1[2] = '\0';
	buff += 2;
	deg = (double)strtoul(buff1, &b1, 10);
	if((buff[0] == '\0') || (b1[0] != '\0')) return -1;

	if (2 != strncpy_n(buff1, buff, 2)) return -1;
	buff1[2] = '\0';
	buff += 2;
	min = (double)strtoul(buff1, &b1, 10);
	if ((buff[0] == '\0') || (b1[0] != '\0')) return -1;

	if ((buff = (char*)strtok(buff, "\0")) == NULL) return -1;
	if ((2 != strchr(buff, '.') - buff) && (2 != strlen(buff))) return -1;
	sec = (double)strtod(buff, &b1);
	if ((buff[0] == '\0') || (b1[0] != '\0')) return -1;

	if((min >= 60) || (min < 0) || (sec >= 60) || (sec < 0)) return -1;

	*dd = sign * (deg + min/60 + sec/3600);

	return 0;
}


int ascol_hms2dd(double *dd, const char *hms) {
	int i;
	double hour, min, sec, sign = 1;
	char *buff, *b1;
	char buff1[3];

	buff = (char*)hms;    //clear the spaces
	while (isspace(buff[0])) buff++;
	i = strlen(buff)-1;
	while (isspace(buff[i])) i--;
	buff[i+1] = '\0';

	if (2 != strncpy_n(buff1, buff, 2)) return -1;
	buff1[2] = '\0';
	buff += 2;
	hour = (double)strtoul(buff1, &b1, 10);
	if((buff[0] == '\0') || (b1[0] != '\0')) return -1;

	if (2 != strncpy_n(buff1, buff, 2)) return -1;
	buff1[2] = '\0';
	buff += 2;
	min = (double)strtoul(buff1, &b1, 10);
	if ((buff[0] == '\0') || (b1[0] != '\0')) return -1;

	if ((buff = (char*)strtok(buff, "\0")) == NULL) return -1;
	if ((2 != strchr(buff, '.') - buff) && (2 != strlen(buff))) return -1;
	sec = (double)strtod(buff, &b1);
	if ((buff[0] == '\0') || (b1[0] != '\0')) return -1;

	if((hour < 0) || (hour >= 24) || (min >= 60) || (min < 0) || (sec >= 60) || (sec < 0))
		return -1;

	*dd = (hour + min/60 + sec/3600) * 15.0;

	return 0;
}

/* Most commands are mapped to the following functions */

int ascol_0_param_cmd(int devfd, char *cmd_name) {
	char cmd[80] = {0};
	char resp[80] = {0};

	snprintf(cmd, 80, "%s\n", cmd_name);
	int res = ascol_write(devfd, cmd);
	ASCOL_DEBUG_WRITE(res, cmd);
	if (res != strlen(cmd)) return ASCOL_WRITE_ERROR;

	res = ascol_read(devfd, resp, 80);
	ASCOL_DEBUG_READ(res, resp);
	if (res <= 0) return ASCOL_READ_ERROR;

	if (strcmp("1",resp)) return ASCOL_COMMAND_ERROR;

	return ASCOL_OK;
}

int ascol_1_int_param_cmd(int devfd, char *cmd_name, int param) {
	char cmd[80] = {0};
	char resp[80] = {0};

	snprintf(cmd, 80, "%s %d\n", cmd_name, param);
	int res = ascol_write(devfd, cmd);
	ASCOL_DEBUG_WRITE(res, cmd);
	if (res != strlen(cmd)) return ASCOL_WRITE_ERROR;

	res = ascol_read(devfd, resp, 80);
	ASCOL_DEBUG_READ(res, resp);
	if (res <= 0) return ASCOL_READ_ERROR;

	if (strcmp("1",resp)) return ASCOL_COMMAND_ERROR;

	return ASCOL_OK;
}

int ascol_1_double_param_cmd(int devfd, char *cmd_name, double param, int precision) {
	char cmd[80] = {0};
	char resp[80] = {0};

	snprintf(cmd, 80, "%s %.*f\n", cmd_name, precision, param);
	int res = ascol_write(devfd, cmd);
	ASCOL_DEBUG_WRITE(res, cmd);
	if (res != strlen(cmd)) return ASCOL_WRITE_ERROR;

	res = ascol_read(devfd, resp, 80);
	ASCOL_DEBUG_READ(res, resp);
	if (res <= 0) return ASCOL_READ_ERROR;

	if (strcmp("1",resp)) return ASCOL_COMMAND_ERROR;

	return ASCOL_OK;
}


int ascol_2_double_param_cmd(int devfd, char *cmd_name, double param1, int precision1, double param2, int precision2) {
	char cmd[80] = {0};
	char resp[80] = {0};

	snprintf(cmd, 80, "%s %.*f %.*f\n", cmd_name, precision1, param1, precision2, param2);
	int res = ascol_write(devfd, cmd);
	ASCOL_DEBUG_WRITE(res, cmd);
	if (res != strlen(cmd)) return ASCOL_WRITE_ERROR;

	res = ascol_read(devfd, resp, 80);
	ASCOL_DEBUG_READ(res, resp);
	if (res <= 0) return ASCOL_READ_ERROR;

	if (strcmp("1",resp)) return ASCOL_COMMAND_ERROR;

	return ASCOL_OK;
}


int ascol_2_double_1_int_param_cmd(int devfd, char *cmd_name, double param1, int precision1, double param2, int precision2, int east) {
	char cmd[80] = {0};
	char resp[80] = {0};

	snprintf(cmd, 80, "%s %.*f %.*f %d\n", cmd_name, precision1, param1, precision2, param2, east);
	int res = ascol_write(devfd, cmd);
	ASCOL_DEBUG_WRITE(res, cmd);
	if (res != strlen(cmd)) return ASCOL_WRITE_ERROR;

	res = ascol_read(devfd, resp, 80);
	ASCOL_DEBUG_READ(res, resp);
	if (res <= 0) return ASCOL_READ_ERROR;

	if (strcmp("1",resp)) return ASCOL_COMMAND_ERROR;

	return ASCOL_OK;
}


int ascol_1_double_return_cmd(int devfd, char *cmd_name, double *val) {
	char cmd[80] = {0};
	char resp[80] = {0};
	double buf;

	snprintf(cmd, 80, "%s\n", cmd_name);
	int res = ascol_write(devfd, cmd);
	ASCOL_DEBUG_WRITE(res, cmd);
	if (res != strlen(cmd)) return ASCOL_WRITE_ERROR;

	res = ascol_read(devfd, resp, 80);
	ASCOL_DEBUG_READ(res, resp);
	if (res <= 0) return ASCOL_READ_ERROR;

	res = sscanf(resp, "%lf", &buf);
	if (res != 1) return ASCOL_RESPONCE_ERROR;

	if (val) *val = buf;

	ASCOL_DEBUG("%s()=%2d <=> %lf\n", __FUNCTION__, ASCOL_OK, *val);
	return ASCOL_OK;
}


int ascol_2_double_return_cmd(int devfd, char *cmd_name, double *val1, double *val2) {
	char cmd[80] = {0};
	char resp[80] = {0};
	double buf1;
	double buf2;

	snprintf(cmd, 80, "%s\n", cmd_name);
	int res = ascol_write(devfd, cmd);
	ASCOL_DEBUG_WRITE(res, cmd);
	if (res != strlen(cmd)) return ASCOL_WRITE_ERROR;

	res = ascol_read(devfd, resp, 80);
	ASCOL_DEBUG_READ(res, resp);
	if (res <= 0) return ASCOL_READ_ERROR;

	res = sscanf(resp, "%lf %lf", &buf1, &buf2);
	if (res != 2) return ASCOL_RESPONCE_ERROR;

	if (val1) *val1 = buf1;
	if (val2) *val2 = buf2;

	ASCOL_DEBUG("%s()=%2d <=> %lf %lf\n", __FUNCTION__, ASCOL_OK, *val1, *val2);
	return ASCOL_OK;
}

/* Comands that require output manipulation and can not be mapped by the functions above */

int ascol_GLLG(int devfd, char *password) {
	char cmd[80] = {0};
	char resp[80] = {0};

	sprintf(cmd, "GLLG %s\n", password);
	int res = ascol_write(devfd, cmd);
	ASCOL_DEBUG_WRITE(res, cmd);
	if (res != strlen(cmd)) return ASCOL_WRITE_ERROR;

	res = ascol_read(devfd, resp, 80);
	ASCOL_DEBUG_READ(res, resp);
	if (res <= 0) return ASCOL_READ_ERROR;

	if (strcmp("1",resp)) return ASCOL_COMMAND_ERROR;

	return ASCOL_OK;
}


int ascol_TRRD(int devfd, double *ra, double *de, char *east) {
	const char cmd[] = "TRRD\n";
	char resp[80] = {0};
	char ra_s[80];
	char de_s[80];
	int east_c;

	int res = ascol_write(devfd, cmd);
	ASCOL_DEBUG_WRITE(res, cmd);
	if (res != strlen(cmd)) return ASCOL_WRITE_ERROR;

	res = ascol_read(devfd, resp, 80);
	ASCOL_DEBUG_READ(res, resp);
	if (res <= 0) return ASCOL_READ_ERROR;

	res = sscanf(resp, "%s %s %d", ra_s, de_s, &east_c);
	if (res != 3) return ASCOL_RESPONCE_ERROR;

	res = 0;
	if (ra) res = ascol_hms2dd(ra, ra_s);
	if (res) return ASCOL_RESPONCE_ERROR;

	if (de) res = ascol_dms2dd(de, de_s);
	if (res) return ASCOL_RESPONCE_ERROR;

	if (east) *east = east_c;

	ASCOL_DEBUG("%s()=%2d <=> %lf %lf %d\n", __FUNCTION__, ASCOL_OK, *ra, *de, *east);
	return ASCOL_OK;
}


int ascol_OIMV(int devfd, ascol_oimv_t *oimv) {
	const char cmd[] = "OIMV\n";
	char resp[180] = {0};

	if (!oimv) return ASCOL_PARAM_ERROR;

	int res = ascol_write(devfd, cmd);
	ASCOL_DEBUG_WRITE(res, cmd);
	if (res != strlen(cmd)) return ASCOL_WRITE_ERROR;

	res = ascol_read(devfd, resp, 180);
	ASCOL_DEBUG_READ(res, resp);
	if (res <= 0) return ASCOL_READ_ERROR;

	res = sscanf(
		resp, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
		&(oimv->value[0]), &(oimv->value[1]), &(oimv->value[2]), &(oimv->value[3]), &(oimv->value[4]),
		&(oimv->value[5]), &(oimv->value[6]), &(oimv->value[7]), &(oimv->value[8]), &(oimv->value[9]),
		&(oimv->value[10]), &(oimv->value[11]),&(oimv->value[12]), &(oimv->value[13]), &(oimv->value[14]),
		&(oimv->value[15]), &(oimv->value[16])
	);
	if (res != ASCOL_OIMV_N) return ASCOL_RESPONCE_ERROR;

	oimv->description = (char **)oimv_descriptions;
	oimv->unit = (char **)oimv_units;

	ASCOL_DEBUG("%s()=%2d <=> ascol_oimv_t\n", __FUNCTION__, ASCOL_OK);
	return ASCOL_OK;
}


int ascol_GLME(int devfd, ascol_glme_t *glme) {
	const char cmd[] = "GLME\n";
	char resp[180] = {0};

	if (!glme) return ASCOL_PARAM_ERROR;

	int res = ascol_write(devfd, cmd);
	ASCOL_DEBUG_WRITE(res, cmd);
	if (res != strlen(cmd)) return ASCOL_WRITE_ERROR;

	res = ascol_read(devfd, resp, 180);
	ASCOL_DEBUG_READ(res, resp);
	if (res <= 0) return ASCOL_READ_ERROR;

	res = sscanf(
		resp, "%lf %lf %lf %lf %lf %lf %lf",
		&(glme->value[0]), &(glme->value[1]), &(glme->value[2]), &(glme->value[3]),
		&(glme->value[4]), &(glme->value[5]), &(glme->value[6])
	);
	if (res != ASCOL_GLME_N) return ASCOL_RESPONCE_ERROR;

	glme->description = (char **)glme_descriptions;
	glme->unit = (char **)glme_units;

	ASCOL_DEBUG("%s()=%2d <=> ascol_glme_t\n", __FUNCTION__, ASCOL_OK);
	return ASCOL_OK;
}


int ascol_GLST(int devfd, ascol_glst_t *glst) {
	const char cmd[] = "GLST\n";
	char resp[180] = {0};

	if (!glst) return ASCOL_PARAM_ERROR;

	int res = ascol_write(devfd, cmd);
	ASCOL_DEBUG_WRITE(res, cmd);
	if (res != strlen(cmd)) return ASCOL_WRITE_ERROR;

	res = ascol_read(devfd, resp, 180);
	ASCOL_DEBUG_READ(res, resp);
	if (res <= 0) return ASCOL_READ_ERROR;

	res = sscanf(
		resp, "%hu %hu %hu %hu %hu %*d %hu %hu %hu %hu %*d %*d %*d %*d %hu %hu %hu %hu %hu %hu %hu %*d",
		&(glst->oil_state), &(glst->telescope_state), &(glst->ra_axis_state), &(glst->de_axis_state), &(glst->focus_state),
		&(glst->dome_state), &(glst->slit_state), &(glst->flap_tube_state), &(glst->flap_coude_state), &(glst->selected_model_index),
		&(glst->state_bits), &(glst->alarm_bits[0]), &(glst->alarm_bits[1]), &(glst->alarm_bits[2]), &(glst->alarm_bits[3]),
		&(glst->alarm_bits[4])
	);
	if (res != ASCOL_GLST_N) return ASCOL_RESPONCE_ERROR;

	ASCOL_DEBUG("%s()=%2d <=> ascol_glst_t\n", __FUNCTION__, ASCOL_OK);
	return ASCOL_OK;
}