#ifndef DF_PLAYER_MINI_H
#define DF_PLAYER_MINI_H

// 7E FF 06 0F 00 01 01 xx xx EF
// 0	->	7E is start code
// 1	->	FF is version
// 2	->	06 is length
// 3	->	0F is command
// 4	->	00 is no receive
// 5~6	->	01 01 is argument
// 7~8	->	checksum = 0 - ( FF+06+0F+00+01+01 )
// 9	->	EF is end code

#define PLAY_MODE_REPEAT_TRACK   0
#define PLAY_MODE_REPEAT_FOLDER  1
#define PLAY_MODE_REPEAT_SINGLE  2
#define PLAY_MODE_RANDOM         3

void mp3_set_play_mode (unsigned char mode);

void mp3_set_reply (char state);

void mp3_fill_cmd_args (unsigned char cmd, unsigned short arg);
void mp3_fill_cmd (unsigned char cmd);

unsigned short mp3_get_checksum (unsigned char *thebuf);

void mp3_fill_checksum (); 

void mp3_play_physical_arg (unsigned short num);
void mp3_play_physical (); 

void mp3_next ();

void mp3_prev ();

//0x06 set volume 0-30
void mp3_set_volume (unsigned short volume);


//0x09 set device 1/2/3/4/5 U/SD/AUX/SLEEP/FLASH
void mp3_set_device (unsigned short device);

//
void mp3_sleep (); 

//
void mp3_reset (); 

//
void mp3_pause (); 

//
void mp3_stop (); 

//
void mp3_play (); 

//specify a mp3 file in mp3 folder in your tf card, "mp3_play (1);" mean play "mp3/0001.mp3"
void mp3_play_arg (unsigned short num);

//set single loop
void mp3_single_loop (char state);

void mp3_loop (char state);

void mp3_single_play (unsigned short num);


#endif

