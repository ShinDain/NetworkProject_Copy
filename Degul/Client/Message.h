#pragma warning(disable:4819)
#pragma once

//Client to Server
#define MSG_NOINFO                 0
#define MSG_REQ_SCOREBOARD         1
#define MSG_LOBBY_ENTER            2
#define MSG_LOBBY_READY            3
#define MSG_LOBBY_START            4
#define MSG_LOBBY_CHAT_TOSERVER    5
#define MSG_BALLDATA_TOSERVER      6

//Server to Client
#define MSG_LOBBY_STATUS           1
#define MSG_LOBBY_CHAT_TOCLIENT    2
#define MSG_PLAYDATA_TOCLIENT      3
#define MSG_PLAY_START             4
#define MSG_PLAY_END               5
#define MSG_PLAY_KILLLOG           6
#define MSG_PLAY_SCOREBOARD        7
#define MSG_PLAY_TIME              8
#define MSG_PLAY_ITEM              9
#define MSG_SCOREBOARD            10
#define MSG_SET_INDEX             11
#define MSG_ERR_GAME_ONPROGRESS   12

#define BUFFER_SIZE 1002

extern char str_arr5[4][20];
extern char msgIDtoStr_toServer[7][64];
extern char msgIDtoStr_toClient[13][64];
