/*
 * GLOBAL.C:
 *
 *      Global variables.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#define MIGNORE
#include "mextern.h"
#include "board.h"

int             Tablesize;
int             Cmdnum;
long            Time;
struct lasttime Shutdown;
int             Spy[PMAX];
int             Numlockedout;
lockout         *Lockout;

struct {
        creature        *ply;
        iobuf           *io;
        extra           *extr;
} Ply[PMAX];

struct {
        short           hpstart;
        short           mpstart;
        short           hp;
        short           mp;
        short           ndice;
        short           sdice;
        short           pdice;
} class_stats[13] = {
        { 1,  1,  1,  1,  1,  1,  1},
        { 55,  40,  5,  2,  1,  6,  0},  /* assassin */
        { 57,  40,  7,  1,  2,  3,  1},  /* barbarian */
        { 54,  50,  4,  3,  1,  4,  0},  /* cleric */
        { 56,  50,  6,  1,  1,  5,  0},  /* fighter */
        { 54,  50, 4,  3,  1,  3,  0},  /* mage */
        { 55,  50,  5,  2,  1,  4,  0},  /* paladin */
        { 56,  40,  6,  2,  2,  2,  0},  /* ranger */
        { 55,  50,  5,  2,  2,  2,  1},  /* thief */
        { 400, 250, 4, 4,  2,  4,  0},  /* invincible */
        { 50, 50, 5, 5,  5,  5,  5},  /* caretaker */
        { 50, 50,  5,  5,  5,  5, 5}, /* sub_dm */
        { 50, 50, 7, 4,  5,  5,  5}   /* DM */
};

/*
int bonus[35] = { -4, -4, -4, -3, -3, -2, -2, -1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                   2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
*/
int bonus[64] = { -4, -4, -4, -3, -3, -2, -2, -1, -1, -1, 0, 0, 0, 0, 1, 1, 
                   1, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
                   5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7,
                   7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
};

char class_str[][15] = { "Á¦ÀÛ", "ÀÚ°´", "±Ç¹ý°¡", "ºÒÁ¦ÀÚ",
        "°Ë»ç", "µµ¼ú»ç", "¹«»ç", "Æ÷Á¹", "µµµÏ", "¹«Àû", "ÃÊÀÎ",
        "¿î¿µÀÚ", "°ü¸®ÀÚ" };

char race_str[][15] = { "Unknown", "³­ÀåÀÌÁ·", "¿ë½ÅÁ·", "¿ä±«Á·", "Åä½ÅÁ·",
                        "ÀÎ°£Á·", "µµ±úºñÁ·", "°ÅÀÎÁ·", "¶¥±Í½ÅÁ·", "°³±¸¸®Á·" };

char race_adj[][15] = { "Unknown", "³­ÀåÀÌÁ·", "¿ë½ÅÁ·", "¿ä±«Á·",
                  "Åä½ÅÁ·", "ÀÎ°£Á·", "µµ±úºñÁ·", "°ÅÀÎÁ·", "¶¥±Í½ÅÁ·", "°³±¸¸®Á·" };

int family_num[16];
char family_str[16][15];
char fmboss_str[16][15];
int family_gold[16];

short level_cycle[][10] = {
        { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0   },
        { CON, PTY, STR, INT, DEX, INT, DEX, PTY, STR, DEX },
        { INT, DEX, PTY, CON, STR, CON, DEX, STR, PTY, STR },
        { STR, DEX, CON, PTY, INT, PTY, INT, DEX, CON, INT },
        { PTY, INT, DEX, CON, STR, CON, INT, STR, DEX, STR },
        { STR, DEX, PTY, CON, INT, CON, INT, DEX, PTY, INT },
        { DEX, INT, CON, STR, PTY, STR, INT, PTY, CON, PTY },
        { PTY, STR, INT, CON, DEX, CON, DEX, STR, INT, DEX },
        { INT, CON, PTY, STR, DEX, STR, CON, DEX, PTY, DEX },
        { STR, DEX, INT, CON, PTY, STR, DEX, INT, CON, PTY },
        { STR, DEX, INT, CON, PTY, STR, DEX, INT, CON, PTY },
        { STR, DEX, INT, CON, PTY, STR, DEX, INT, CON, PTY },
        { STR, DEX, INT, CON, PTY, STR, DEX, INT, CON, PTY }
};

short thaco_list[][20] = {
        { 20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20 }, 
/*a*/   { 18,18,18,17,17,16,16,15,15,14,14,13,13,12,12,11,10,10, 9, 9 },
/*b*/   { 20,19,18,17,16,15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 3, 2 },
/*c*/   { 20,20,19,18,18,17,16,16,15,14,14,13,13,12,12,11,10,10, 9, 8 },
/*f*/   { 20,19,18,17,16,15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 3, 3 },
/*m*/   { 20,20,19,19,18,18,18,17,17,16,16,16,15,15,14,14,14,13,13,11 },
/*p*/   { 19,19,18,18,17,16,16,15,15,14,14,13,13,12,11,11,10, 9, 8, 7 },
/*r*/   { 19,19,18,17,16,16,15,15,14,14,13,12,12,11,11,10, 9, 9, 8, 7 },
/*t*/   { 20,20,19,19,18,18,17,17,16,16,15,15,14,14,13,13,12,12,11,11 },
		{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ -5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5 },
		{ -5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5 },
		{ -5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5 }
};

long quest_exp[] = {
        120, 500, 1000, 1000, 5000, 8000, 10000, 20000,
        50000, 80000, 100000, 200000, 500000, 800000, 2500, 2500,
        2500, 5, 5, 5, 125, 125, 125, 125,
        125, 125, 125, 125, 125, 125, 125, 125,
        125, 125, 125, 125, 125, 125, 125, 125,
        125, 125, 125, 125, 125, 125, 125, 125,
        125, 125, 125, 125, 125, 125, 125, 125,
        125, 125, 125, 125, 125, 125, 125, 125,
        125, 125, 125, 125, 125, 125, 125, 125,
        125, 125, 125, 125, 125, 125, 125, 125,
        125, 125, 125, 125, 125, 125, 125, 125,
        125, 125, 125, 125, 125, 125, 125, 125,
        125, 125, 125, 125, 125, 125, 125, 125,
        125, 125, 125, 125, 125, 125, 125, 125,
        125, 125, 125, 125, 125, 125, 125, 125,
        125, 125, 125, 125, 125, 125, 125, 125
};
 
long needed_exp[] = {
128, 256, 384, 512, /* 2 */
640, 768, 896, 1024, /* 6 */
1280, 1536, 1792, 2048, /* 10 */
2560, 3072, 3584, 4096, /* 14 */
5120, 6144, 7168, 8192, /* 18 */
10240, 12288, 14336, 16384, /* 22 */
20480, 24576, 28672, 32768, /* 26 */
40960, 49152, 57344, 65536, /* 30 */
74152, 82768, 91384, 100000, /* 34 */
111602, 123205, 134807, 146410, /* 38 */
161647, 176885, 192122, 207360, /* 42 */
234062, 260765, 287468, 314171, /* 46 */
350876, 387581, 424286, 460992, /* 50 */
510275, 559558, 608841, 658125, /* 54 */
715469, 772814, 830159, 887504, /* 58 */
966331, 1045159, 1123987, 1202815, /* 62 */
1327015, 1451215, 1575415, 1699616, /* 66 */
1825576, 1951536, 2077496, 2120345, /* 70 */
2272342, 2421228, 2570114, 2729000, /* 74 */
2875534, 3022069, 3178604, 3325139, /* 78 */
3475134, 3825129, 3975124, 4125120, /* 82 */
4272005, 4428890, 4570775, 4722661, /* 86 */
4894263, 5045866, 5197469, 5529072, /* 90 */
5857897, 6196723, 6435549, 6774375, /* 94 */
7005781, 7337187, 7676859, 7984959, /* 98 */
8229756, 9079416, 9441769, 10000000, /* 102 */
12728240, 14066499, 16511819, 18072765, /* 106 */
20758586, 22579273, 24545614, 26669264, /* 110 */
28962805, 30439829, 32115015, 34920766, /* 114 */
36333635, 39053662, 43108492, 48528256, /* 118 */
54345799, 60596921, 65320644, 71559502, /* 122 */
76359857, 86000000, 100000000, 190000000, /* 126 */
};

/*

        512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 100000,
     146410, 207360, 314171, 460992, 658125, 887504, 1202815,
    1699616, 2203457, 2799000, 3505139, 4465120, 5792661, 7319072,
    8874375, 2000000};
*/

char lev_title[][8][20] = {
        { "", "", "", "", "", "", "", "" },
        { "±øÆÐ", "°­µµ", "»ìÀÎÀÚ", "µµ»ìÀÚ",
          "¿ÕÀÚ°´", "ÀÀÂ¡ÀÚ", "»ì¼º", "»ì½Å" },
        { "ÃÊº¸", "¼ö·Ã»ý", "¹«Çù", "Ã¶±Ç",
          "±Ç¼º", "±ÇÈ²", "ÁöÁ¸", "¹«½Å" },
        { "¶¯Áß", "»ç¹Ì½Â", "¼Ò½Â", "°¨Âû½Â",
          "ÁÖÁö", "´ë»ç", "±¹»ç", "ºÎÃ³" },
        { "¹éÁ¤", "Ä®ÀâÀÌ", "¹«ÀÎ", "¿ëº´",
          "°Ë°´", "°Ë¼º", "°ËÈ²", "¹«¸²ÆÄÃµ" },
        { "½ÉºÎ¸§²Û", "µµÁ¦ÀÚ", "¸¶¼ú»ç", "µµ°´",
          "¸¶ÀÎ", "µµÀÎ", "½Å¼±", "¸¶Á¸" },
        { "°ñ¸ñ´ëÀå", "¹«°´", "Çù°´", "ÀÇÇù",
          "Á¤ÀüÀÚ", "¿ëÀü»ç", "¼öÈ£ÀÚ", "¼ºÀü»ç" },
        { "ÂÌµû±¸", "¼øÂûº´", "°¨Âû¿ø", "µµ¼ºÁö±â",
          "Æ÷±³", "Æ÷µµ´ëÀå", "°¨Âû¾î»ç", "°¨ÂûÀå±º" },
        { "¹Ù´ÃµµµÏ", "°³µµµÏ", "Á»µµµÏ", "¼ÒµµµÏ",
          "¿ÕµµµÏ", "µµ¼º", "µµ½Å", "½Å¼ö" },
        { "¹«Àû", "¹«Àû", "¹«Àû", "¹«Àû", "¹«Àû",
          "¹«Àû", "¹«Àû", "¹«Àû"},
        { "ÃÊÀÎ", "ÃÊÀÎ", "ÃÊÀÎ", "ÃÊÀÎ", "ÃÊÀÎ",
          "ÃÊÀÎ", "ÃÊÀÎ", "ÃÊÀÎ"},
        { "µµ¿ì¹Ì", "µµ¿ì¹Ì", "µµ¿ì¹Ì", "µµ¿ì¹Ì",
          "µµ¿ì¹Ì", "µµ¿ì¹Ì", "µµ¿ì¹Ì", "µµ¿ì¹Ì" },
        { "¹Ùº¸", "¸ÛÃ»ÀÌ", "¶Ç¶óÀÌ", "¸ÓÀú¸®",
          "¶î¶îÀÌ", "¿Õ¹Ùº¸", "¹éÄ¡È²Á¦", "¹Ùº¸µéÀÇ½Å"  },
};
struct {
        char    *cmdstr; 
        int     cmdno; 
        int     (*cmdfn)(); 
} cmdlist[] = { 
        { "³ª°¡´Â±æ", 1, move },
        { "±¤Àå", 1, move },
        { "Çâ·Î", 1, move },
        { "¼ö·ÃÀå", 1, move },
        { "Çö°¨¿¡°Ô", 1, move },
        { "¸éÈ¸Çã°¡", 1, move },
        { "¹ÝÇÏµµÀÎ", 1, move},
        { "¿ù±¤¹Ý", 1, move }, 
        { "¹Ý¾ß¹Ù¶ó¹Ð", 1, move },
        { "8", 1, move },
        { "ºÏ", 1, move },
        { "¤²", 1, move },
        { "Éª£¢", 1, move },
        { "2", 1, move },
        { "³²", 1, move },
        { "¤¤", 1, move },
        { "É¥£¢", 1, move },
        { "6", 1, move },
        { "µ¿", 1, move },
        { "¤§", 1, move },
        { "É¦£¢", 1, move },
        { "4", 1, move },
        { "¼­", 1, move },
        { "¤µ", 1, move },
        { "É¬£¢", 1, move },
        { "ºÏµ¿", 1, move },
        { "¤²¤§", 1, move },
        { "ºÏ¼­", 1, move },
        { "ºÏµ¿", 1, move },
        { "³²¼­", 1, move },
        { "¤²¤µ", 1, move },
        { "³²µ¿", 1, move },
        { "¤¤¤§", 1, move },
        { "³²¼­", 1, move },
        { "¤¤¤µ", 1, move },
        { "9", 1, move },
        { "À§", 1, move },
        { "¤·", 1, move },
        { "É®£¢", 1, move },
        { "3", 1, move },
        { "¹Ø", 1, move },
        { "¤±", 1, move },
        { "É©£¢", 1, move },
        { "¹Û", 1, move },
        { "³ª°¡", 1, move }, /* leave */
        { "ºÁ", 2, look },     /* look */  
        { "º¸´Ù", 2, look },
        { "º¸¾Æ", 100, action },
        { "Á¶»ç", 2, look }, /* examine */
        { "³¡", 3, quit }, /* quit */
        { "¸»", 4, say },
        { "\"", 4, say },
        { "'", 4, say },
        { "ÁÖ¿ö", 5, get }, /* get */
        { "ÁÖ", 5, get },
        { "°¡Á®", 5, get }, /* take */
        { "²¨³»", 5, get }, /* Ãß°¡ */
        { "¼ÒÁöÇ°", 6, inventory }, /* inventory */
        { "¹ö·Á", 7, drop }, /* drop */
        { "³Ö¾î", 7, drop }, /* put */
        { "´©±¸", 8, who }, /* who */
        { "ÀÔ¾î", 9, wear }, /* wear */
        { "¹þ¾î", 10, remove_obj }, /* remove */
        { "Àåºñ", 11, equipment }, /* equipment */
        { "Àå", 11, equipment },
        { "Áã¾î", 12, hold }, /* hold */
        { "Àâ¾Æ", 12, hold },
        { "¹«Àå", 13, ready }, /* wield */
        { "µµ¿ò¸»", 14, help }, /* µµ¿ò¸» */
        { "?", 14, help },
        { "°Ç°­", 15, health }, /* health */
        { "Á¡¼ö", 15, health }, /* score */
        { "Á¤º¸", 16, info }, /* information */
        { "¾ê±â", 17, send }, /* send */
        { "ÀÌ¾ß±â", 17, send }, /* tell */
        { "µû¶ó", 18, follow }, /* follow */
        { "³»º¸³»", 19, lose },
        { "±×·ì", 20, group }, /* group */
        { "¹«¸®", 20, group }, /* party */
        { "ÃßÀû", 21, track },
        { "¿³ºÁ", 22, peek }, /* peek */
        { "°ø°Ý", 23, attack }, /* attack */
        { "°ø", 23, attack },
        { "ÃÄ", 23, attack }, /* kill */
        { "¶§·Á", 23, attack }, /* k */
        { "°Ë»ö", 24, search }, /* search */
        { "Ã£¾Æ", 24, search },
        { "Ç¥Çö",25, emote},
        { "¼û°Ü", 26, hide },
        { "¼û¾î", 26, hide },
        { "¼³Á¤", 27, set }, /* set */
        { "ÇØÁ¦", 28, clear }, /* clear */
        { "¿ÜÃÄ", 29, yell }, /* yell */
        { "°¡", 30, go }, /* go */
        { "µé¾î°¡", 30, go },
        { "¿­¾î", 31, openexit }, /* open */
        { "´Ý¾Æ", 32, closeexit }, /* close */
        { "Ç®¾î", 33, unlock }, /* Ç®¾î */
        { "Àá±Å", 34, lock }, /* lock */
        { "µû", 35, picklock }, /* pick */
        { "ÈÉÃÄ", 36, steal }, /* steal */
        { "µµ¸Á", 37, flee }, /* flee */
        { "µµ", 37, flee },
        { "ÁÖ¹®", 38, cast }, /* cast */
        { "¹è¿ö", 39, study },
        { "¿¬¸¶", 39, study },
        { "ÀÐ¾î", 40, readscroll },
        { "Ç°¸ñ", 41, list },
        { "»ç", 42, buy },
        { "ÆÈ¾Æ", 43, sell },
        { "°¡Ä¡", 44, value },
        { "°¡°Ý", 44, value },
        { "±â½À", 45, backstab },
        { "¼ö·Ã", 46, train },
        { "Áà", 47, give },
        { "¼ö¸®", 48, repair },
        { "½Ã°£", 49, prt_time },
        { "±³¶õ", 50, circle },
        { "¸Í°ø", 51, bash },
        { "ÀúÀå", 52, savegame }, /* save */
        { "ÆíÁöº¸³»±â", 53, postsend },
        { "ÆíÁö»èÁ¦", 55, postdelete },
        { "ÆíÁö¹Þ±â", 54, postread },
        { "´ëÈ­", 56, talk },
        { "±×·ì¸»",57, gtalk }, /* gtalk */
        { "¹«¸®¸»",57, gtalk },
        { "=", 57, gtalk },
        { "¸¶¼Å", 58, drink }, /* drink */
        { "¸Ô¾î", 58, drink },
        { "Àâ´ã", 59, broadsend },
        { "Àâ", 59, broadsend },
		{ "È¯È£", 70, broadsend2 },
        { "zap", 60, zap },
        { "È¯¿µ", 61, welcome },
        { "¹æÈ¥¼ú", 62, turn },
		{ "º¸°ü¹°", 63, bank_inv },
		{ "ÀÜ¾×", 63, bank },
		{ "ÀÔ±Ý", 63, deposit },
		{ "Ãâ±Ý", 63, withdraw },
		{ "¹Þ¾Æ", 63, output_bank },
        { "È°º¸¹ý", 64, haste },
        { "½Å¿ø¹ý", 65, pray }, /* pray */
        { "°æ°è", 66, prepare },
        { "»ç¿ë", 67, use }, /* use */
        { "µè±â°ÅºÎ", 68, ignore }, /* ignore */
        { "»ç¿ëÀÚ°Ë»ö", 69, whois },
		{ "¹¦»ç", 73, description },
/*        { "Àº½Å¼ú", 70, sneak },    */
        { "°¡¸£ÃÄ", 71, teach },
		{ "¼±ÀüÆ÷°í", 72, call_war },
/*
        { "°¡ÀÔ",72, pledge },
        { "Å»Åð",73, rescind },
*/
        { "±¸ÀÔ",74, purchase },
        { "¼±ÅÃ",75, selection },
        { "±³È¯",76, trade},
        { "¸ñ¸Å´Þ±â", 77, ply_suicide }, /* suicide */
        { "¾ÏÈ£", 78, passwd}, /* passwd */
        { "ÅõÇ¥",79,vote},
        { "»ç¿ëÀÚÁ¤º¸", 80, pfinger},
        { "±ÍÈ¯", 81, return_square },
        { "±Í", 81, return_square },
        { "ÁÙÀÓ¸»", 82, ply_aliases },
        { "ÁÙ", 82, ply_aliases },
        { "ÅÂ¿ö", 83, burn },
        { "¼Ò°¢", 83, burn },
        { "ÄªÈ£", 84, set_title },
        { "ÄªÈ£»èÁ¦", 84, clear_title },
        { "Á¦·Ã", 85, forge },
	{ "¹«±â¸¸µé±â", 85, newforge }, 
        { "Á÷¾÷ÀüÈ¯", 86, change_class },
        { "±â°øÁý°á", 87, power },
        { "»ì±âÃæÀü", 88, accurate },
        { "Èí¼º´ë¹ý", 89, absorb },
        { "Âü¼±", 90, meditate },
        { "Ç÷µµºÀ¼â", 91, magic_stop },
        { "½á", 92, writeboard },
        { "±Û»èÁ¦", 93, del_board },
        { "°Ô½ÃÆÇ", 94, look_board },
        { "¸í¸í", 95, chg_name },
        { "°¨Á¤", 96, info_obj },
        { "ºñ±³", 96, obj_compare },
        { "Â÷±â", 97, kick },
        { "µ¶»ìÆ÷", 98, poison_mon },
        { "Àá·Â°Ý¹ß", 99, up_dmg },
        { "°¨Á¤Ç¥Çö", 100, action },
        { "³ë·ÁºÁ", 100, action },
        { "²ô´ö", 100, action },
        { "ÀÀ", 100, action },
        { "¾Æ´Ï", 100, action },
        { "°¨", 100, action },
        { "°¨»ç", 100, action },
        { "¹Ì¼Ò", 100, action },
        { "Ã»È¥", 100, action },
        { "¶³¾î", 100, action },
        { "ÇØ", 100, action },
        { "ÇÏÇ°", 100, action },
        { "¿ô¾î", 100, action },
        { "¹Ì¾È", 100, action },
        { "¾Ç¼ö", 100, action },
        { "ÇÏÀÌÆÄÀÌºê", 100, action },
        { "¹Ú¼ö", 100, action },
        { "Èí¿¬", 100, action },
        { "´ã¹è", 100, action },
        { "Àý", 100, action },
        { "Âñ·¯", 100, action },
        { "Ãã", 100, action },
        { "³ë·¡", 100, action },
        { "¿ï¾î", 100, action },
        { "´Þ·¡", 100, action },
        { "´çÈ²", 100, action },
        { "»ý°¢", 100, action },
        { "ºÎ²ô·¯", 100, action },
        { "³î·Á", 100, action },
        { "¼³·¹", 100, action },
        { "Àß°¡", 100, action },
        { "¹ÙÀÌ", 100, action },
        { "¾È³ç", 100, action },
        { "»Ç»Ç", 100, action },
        { "À®Å©", 100, action },
        { "±¸°É", 100, action },
	{ "±¸¹Ú", 100, action },
	{ "¾È¾Æ", 100, action },
	{ "²¸¾È¾Æ", 100, action },
/****************************************************************/
/*  { "Àü¼ö", 149, trans_exp },    */
/****************************************************************/
	{ "Çâ»ó", 149, buy_states },
	{ "°áÈ¥", 150, marriage },
	{ "»ç¶û¸»", 150, m_send },
    { "ÀÌÈ¥", 150, divorce },   
    { "±â¾ï", 153, moon_set }, 
	{ "¸Þ¸ð", 151, memo },
	{ "ÃÊ´ë", 152, invite },
    { "»óÅÂ", 154, enemy_status },
	{ "ÆÐ°Å¸®´©±¸", 148, family_who },
	{ "ÆÐ°Å¸®°øÁö", 148, family_news },
	{ "ÆÐ°Å¸®°¡ÀÔ", 148, family }, 
	{ "°¡ÀÔÇã°¡", 148, boss_family },
	{ "ÆÐ°Å¸®Å»Åð", 148, out_family },
	{ "ÆÐ°Å¸®Ãß¹æ", 148, fm_out },
	{ "ÆÐ°Å¸®¿ø", 148, family_member },
	{ "ÆÐ°Å¸®¸»", 148, family_talk },
	{ "]", 148, family_talk },
	{ "¸ðµçÆÐ°Å¸®", 148, list_family },
	{ "´ë´ä", 153, resend },
	{ "/", 153, resend },
        { "*teleport", 101, dm_teleport },
        { "*¼ø°£ÀÌµ¿", 101, dm_teleport },
        { "*¹æ¹øÈ£", 102, dm_rmstat },
        { "*rm", 102, dm_rmstat },
        { "*¹æ", 102, dm_rmstat },
        { "*reload", 103, dm_reload_rom },
        { "*·Îµå", 103, dm_reload_rom },
        { "*save", 104, dm_resave },
        { "*¼¼ÀÌºê", 104, dm_resave },
        { "*create", 105, dm_create_obj },
        { "*¹¹µçÁö´Ù¸¸µé¾î", 105, dm_create_obj},
        { "*c", 105, dm_create_obj },
        { "*perm", 106, dm_perm },
        { "*¿µ¿ø", 106, dm_perm },
        { "*invis", 107, dm_invis },
        { "*¹Ù¶÷Ã³·³»ç¶óÁ®", 107, dm_invis },
        { "*i", 107, dm_invis },
        { "*s", 108, dm_send },
        { "*send", 108, dm_send },
        { "*°øÁö", 108, dm_send },
        { "*purge", 109, dm_purge },
        { "*Ã»¼Ò", 109, dm_purge },
        { "*Ã»", 109, dm_purge },
        { "*ac", 110, dm_ac },
        { "*¹æ¾î·Â", 110, dm_ac },
        { "*users", 111, dm_users },
        { "*´©", 111, dm_users },
        { "*´©±¸", 111, dm_users },
        { "*echo", 112, dm_echo },
        { "*¸»", 112, dm_echo },
        { "*flushrooms", 113, dm_flushsave },
        { "*¸ðµçÀúÀå", 113, dm_flushsave },
        { "*shutdown", 114, dm_shutdown },
        { "*Á¾·á", 114, dm_shutdown },
        { "*f", 115, dm_force },
        { "*force", 115, dm_force },
        { "*¹¹µçÁö´Ù½ÃÄÑ", 115, dm_force },
        { "*flushcrtobj", 116, dm_flush_crtobj },
        { "*Àç¼³Á¤", 116, dm_flush_crtobj },
        { "*Àç", 116, dm_flush_crtobj },
        { "*monster", 117, dm_create_crt },
        { "*±«¹°", 117, dm_create_crt },
        { "*±«", 117, dm_create_crt },
        { "*status", 118, dm_stat },
        { "*»óÅÂ", 118, dm_stat },
        { "*add", 119, dm_add_rom },
        { "*¹æÁ¦ÀÛ", 119, dm_add_rom },
        { "*¹¹µçÁö´ÙÇØ", 120, dm_set },
        { "*log", 121, dm_log },
        { "*Á¢¼Ó", 121, dm_log },
        { "*spy", 122, dm_spy },
        { "*¹¹µçÁö´Ù¿³ºÁ", 122, dm_spy },
        { "*lock", 123, dm_loadlockout },
        { "*Á¦ÇÑ", 123, dm_loadlockout },
        { "*finger", 124, dm_finger },
        { "*ÇÎ°Å", 124, dm_finger },
        { "*list", 125, dm_list },
        { "*´©±¸µçÁö´ÙºÁ", 125, dm_list }, 
        { "*info", 126, dm_info },
        { "*Á¤º¸", 126, dm_info },
        { "*parameter", 127, dm_param },
        { "*¼öÄ¡", 127, dm_param },
        { "*silence", 128, dm_silence},
        { "*º¡¾î¸®", 128, dm_silence},
        { "*broad", 129, dm_broadecho},
        { "*¹æ¼Û", 129, dm_broadecho },
        { "*replace",130, dm_replace},
        { "*±³Ã¼", 130, dm_replace},
        { "*name",131, dm_nameroom},
        { "*¹æÀÌ¸§", 131, dm_nameroom},
        { "*append",132, dm_append},
        { "*Ãß°¡", 132, dm_append},
        { "*prepend",133, dm_prepend},
        { "*¼­¾ð", 133, dm_prepend},     
        { "*gcast",134, dm_cast},
        { "*ÀüÁÖ¹®", 134, dm_cast},
        { "*group",135, dm_group},
        { "*±×·ì", 135, dm_group},
        { "*notepad",136, notepad},
        { "*¸Þ¸ð", 136, notepad},
        { "*delete",137, dm_delete},
        { "*Áö¿ì±â", 137, dm_delete},
        { "*oname", 138, dm_obj_name },
        { "*¹¹µçÁö´Ù¹Ù²ã", 138, dm_obj_name },
        { "*cname", 139, dm_crt_name },
        { "*±«¹°ÀÌ¸§", 139, dm_crt_name },
        { "*active", 140, list_act },
        { "*È°¼º", 140, list_act },
        { "*dust", 141, dm_dust },
        { "*³ªµµ°¡²ûÈ­³½´Ù", 141, dm_dust },
        { "*cfollow", 142, dm_follow },
        { "*µû¸£±â", 142, dm_follow },
        { "*¶³¾îÁ®¶ó", 148, dm_moonstone },
        { "*Ä§°ø", 148, dm_monster },
        { "*dmhelp", 143, dm_help },
        { "*µµ¿ò¸»", 143, dm_help },
        { "*attack", 144, dm_attack },
        { "*°ø°Ý", 144, dm_attack },
        { "*enemy", 145, list_enm },
        { "*Àû", 145, list_enm },
        { "*charm", 146, list_charm },
        { "*ÃÖ¸é", 146, list_charm },
        { "*»ç¿ëÀÚÀúÀå", 147, dm_save_all_ply },
        { "´­·¯", -2, 0 },
        { "¹Ð¾î", -2, 0 },
        { "@", 0, 0 }
};

char article[][10] = {
                  "the",
                  "from",
                  "to",
                  "with",
                  "an",
                  "in",
                  "for",
                  "@"
};

struct {
                  char    *splstr;
                  int     splno;
                  int     (*splfn)();
                  int	spllv;
} spllist[] = {
                  { "È¸º¹", SVIGOR, vigor, 1 },
                  { "»èÇ³", SHURTS, offensive_spell, 2 },
                  { "¹ß±¤", SLIGHT, light, 2 },
                  { "ÇØµ¶", SCUREP, curepoison, 1 },
                  { "¼ºÇöÁø", SBLESS, bless, 2 },
                  { "¼öÈ£Áø", SPROTE, protection, 2 },
                  { "È­±Ã", SFIREB, offensive_spell, 3 },
                  { "ÀºµÐ¹ý", SINVIS, invisibility, 5 },
                  { "µµ·Â¹Ý", SRESTO, restore, 4 },
                  { "ÀºµÐ°¨Áö¼ú", SDINVI, detectinvis, 3 },
                  { "ÁÖ¹®°¨Áö¼ú", SDMAGI, detectmagic, 3 },
                  { "ÃàÁö¹ý", STELEP, teleport, 4 },
                  { "È¥µ¿", SBEFUD, befuddle, 3 },
                  { "³úÀü", SLGHTN, offensive_spell, 4 },
                  { "µ¿¼³ÁÖ", SICEBL, offensive_spell, 5 },
                  { "ºùÀÇ", SENCHA, enchant, 4 },
                  { "±ÍÈ¯", SRECAL, recall, 3 },
                  { "¼ÒÈ¯", SSUMMO, summon, 3 },
                  { "¿ø±âÈ¸º¹", SMENDW, mend, 3 },
                  { "¿ÏÄ¡", SFHEAL, heal, 5 },
                  { "ÃßÀû", STRACK, magictrack, 4 },
                  { "ºÎ¾ç¼ú", SLEVIT, levitate, 3 },
                  { "¹æ¿­Áø", SRFIRE, resist_fire, 3 },
                  { "ºñ»ó¼ú", SFLYSP, fly, 4 },
                  { "º¸¸¶Áø", SRMAGI, resist_magic, 3 },
                  { "±ÇÇ³¼ú", SSHOCK, offensive_spell, 5 },
                  { "Áöµ¿¼ú", SRUMBL, offensive_spell, 2 },
                  { "È­¼±µµ", SBURNS, offensive_spell, 2 },
                  { "Åº¼ö°ø", SBLIST, offensive_spell, 2 },
                  { "Ç³¸¶Çö", SDUSTG, offensive_spell, 3 },
                  { "ÆÄÃÊ½Ä", SWBOLT, offensive_spell, 3 },
                  { "ÆøÁø", SCRUSH, offensive_spell, 3 },
                  { "³«¼®", SENGUL, offensive_spell, 5 },
                  { "È­Ç³¼ú", SBURST, offensive_spell, 3 },
                  { "È­·æ´ëÃµ", SSTEAM, offensive_spell, 3 },
                  { "ÅäÇÕ¼ú", SSHATT, offensive_spell, 4 },
                  { "ÁÖÀÛÇö", SIMMOL, offensive_spell, 4 },
                  { "¿­»çÃµ", SBLOOD, offensive_spell, 4 },
                  { "ÆÄÃµÇ³", STHUND, offensive_spell, 5 },
                  { "Áö¿ÁÆÐ", SEQUAK, offensive_spell, 5 },
                  { "ÅÂ¾ç¾È", SFLFIL, offensive_spell, 5 },
                  { "¼±¾Ç°¨Áö", SKNOWA, know_alignment, 3 },
                  { "ÀúÁÖÇØ¼Ò", SREMOV, remove_curse, 4 },
                  { "¹æÇÑÁø", SRCOLD, resist_cold, 3 },
                  { "¼ö»ý¼ú", SBRWAT, breathe_water, 3 },
                  { "Áö¹æÈ£", SSSHLD, earth_shield, 3 },
                  { "Ãµ¸®¾È", SLOCAT, locate_player, 4 },
                  { "¹éÄ¡¼ú", SDREXP, drain_exp, 5 },
                  { "Ä¡·á", SRMDIS, rm_disease, 3 },
                  { "°³¾È¼ú", SRMBLD, rm_blind, 3 },
                  { "°øÆ÷", SFEARS, fear, 4 },
                  { "ÀüÈ¸º¹", SRVIGO, room_vigor, 4 },
                  { "Àü¼Û", STRANO, object_send, 4 },
                  { "½Ç¸í", SBLIND, blind, 5 },
                  { "ºÀÇÕ±¸", SSILNC, silence, 5 },
                  { "ÀÌÈ¥´ë¹ý", SCHARM, charm, 5 },
/*                { "º¯¼ö³ªÇÑ±Ç", SNAHAN, offensive_spell },     */
                  { "@", -1,0,0 }
};

struct osp_t ospell[] = {
                  { SHURTS,  WIND,  3, 1, 8,  0, 1 },     /* hurt */
                  { SRUMBL, EARTH,  3, 1, 8,  0, 1 },     /* rumble */
                  { SBURNS,  FIRE,  3, 1, 7,  1, 1 },     /* burn */
                  { SBLIST, WATER,  3, 1, 8,  0, 1 },     /* blister */

                  { SDUSTG,  WIND,  7, 2, 5,  7, 2 },     /* dustgust */
                  { SCRUSH, EARTH,  7, 2, 5,  7, 2 },     /* stonecrush */
                  { SFIREB,  FIRE,  7, 2, 5,  8, 2 },     /* fireball */
                  { SWBOLT, WATER,  7, 2, 5,  8, 2 },     /* waterbolt */

                  { SSHOCK,  WIND, 10, 2, 5, 13, 2 },     /* shockbolt */
                  { SENGUL, EARTH, 10, 2, 5, 13, 2 },     /* engulf */
                  { SBURST,  FIRE, 10, 2, 5, 13, 2 },     /* burstflame */
                  { SSTEAM, WATER, 10, 2, 5, 13, 2 },     /* steamblast */

                  { SLGHTN,  WIND, 15, 3, 4, 18, 3 },     /* lightning */
                  { SSHATT, EARTH, 15, 3, 4, 19, 3 },     /* shatterstone */
                  { SIMMOL,  FIRE, 15, 3, 4, 18, 3 },     /* immolate */
                  { SBLOOD, WATER, 15, 3, 4, 18, 3 },     /* bloodboil */

                  { STHUND,  WIND, 25, 4, 5, 30, 3 },     /* thuderbolt */
                  { SEQUAK, EARTH, 25, 4, 5, 30, 3 },     /* earthquake */
                  { SFLFIL,  FIRE, 25, 4, 5, 30, 3 },     /* flamefill */
                  { SICEBL, WATER, 25, 4, 5, 30, 3 },     /* iceblade */
/*                { SNAHAN,  WIND, 10, 2, 5, 13, 2 },  */ /* º¯¼ö³ªÇÑ±Ç */
                  { -1, 0, 0, 0, 0, 0, 0 }
};

char number[][10] = {
                  "zero",
                  "one",
                  "two",
                  "three",
                  "four",
                  "five",
                  "six",
                  "seven",
                  "eight",
                  "nine",
                  "ten",
                  "eleven",
                  "twelve",
                  "thirteen",
                  "fourteen",
                  "fifteen",
                  "sixteen",
                  "seventeen",
                  "eighteen",
                  "nineteen",
                  "twenty"
};


