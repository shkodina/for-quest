#include "DFPlayer_Mini_Mp3.h"

#include "softUart.h"

static unsigned char send_buf[10] = {
	0x7E, 0xFF, 06, 00, 00, 00, 00, 00, 00, 0xEF};

static unsigned char recv_buf[10];

static char is_reply = 1;

//
void mp3_set_reply (char state) {
	is_reply = state;
	send_buf[4] = is_reply;
}

//
static void fill_uint16_bigend (unsigned char *thebuf, unsigned short data) {
	*thebuf =	(unsigned char)(data>>8);
	*(thebuf+1) =	(unsigned char)data;
}


//calc checksum (1~6 byte)
unsigned short mp3_get_checksum (unsigned char *thebuf) {
	unsigned short sum = 0;
    char i;
	for (i=1; i<7; i++) {
		sum += thebuf[i];
	}
	return -sum;
}

//fill checksum to send_buf (7~8 byte)
void mp3_fill_checksum () {
	unsigned short checksum = mp3_get_checksum (send_buf);
	fill_uint16_bigend (send_buf+7, checksum);
}

//
void h_send_func () {
    char i;
	for (i=0; i<10; i++) {
		printf ("%x ", send_buf[i]);
	}
}

//
void s_send_func () {
    char i;
	for (i=0; i<10; i++) {
		SUART_PutChar(send_buf[i]);

	}
}

//

//
void mp3_send_cmd_arg (unsigned char cmd, unsigned short arg) {
	send_buf[3] = cmd;
	fill_uint16_bigend ((send_buf+5), arg);
	mp3_fill_checksum ();

    s_send_func();
}

//
void mp3_send_cmd (unsigned char cmd) {
	send_buf[3] = cmd;
	fill_uint16_bigend ((send_buf+5), 0);
	mp3_fill_checksum ();

    s_send_func();
}


//
void mp3_set_play_mode(unsigned char mode){
	mp3_send_cmd_arg (0x08, mode);
}

void mp3_play_physical_arg (unsigned short num) {
	mp3_send_cmd_arg (0x03, num);
}

//
void mp3_play_physical () {
	mp3_send_cmd (0x03);
}

//
void mp3_next () {
	mp3_send_cmd (0x01);
}

//
void mp3_prev () {
	mp3_send_cmd (0x02);
}

//0x06 set volume 0-30
void mp3_set_volume (unsigned short volume) {
	mp3_send_cmd_arg (0x06, volume);
}

//0x07 set EQ0/1/2/3/4/5    Normal/Pop/Rock/Jazz/Classic/Bass
void mp3_set_EQ (unsigned short eq) {
	mp3_send_cmd_arg (0x07, eq);
}

//0x09 set device 1/2/3/4/5 U/SD/AUX/SLEEP/FLASH
void mp3_set_device (unsigned short device) {
	mp3_send_cmd_arg (0x09, device);
}

//
void mp3_sleep () {
	mp3_send_cmd (0x0a);
}

//
void mp3_reset () {
	mp3_send_cmd (0x0c);
}

//
void mp3_play () {
	mp3_send_cmd (0x0d);
}

//
void mp3_pause () {
	mp3_send_cmd (0x0e);
}

//
void mp3_stop () {
	mp3_send_cmd (0x16);
}

//play mp3 file in mp3 folder in your tf card
void mp3_play_arg (unsigned short num) {
	mp3_send_cmd_arg (0x12, num);
}



//
void mp3_single_loop (char state) {
	mp3_send_cmd_arg (0x19, !state);
}

void mp3_loop (char state) {
	mp3_send_cmd_arg (0x11, !state);
}

//add 
void mp3_single_play (unsigned short num) {
	unsigned char i;
	mp3_play (num);
    for (i = 0;i++<255;);
	mp3_single_loop (1);
	//mp3_send_cmd (0x19, !state);
}




