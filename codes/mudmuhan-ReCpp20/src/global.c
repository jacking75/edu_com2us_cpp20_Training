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

char class_str[][15] = { "����", "�ڰ�", "�ǹ���", "������",
        "�˻�", "������", "����", "����", "����", "����", "����",
        "���", "������" };

char race_str[][15] = { "Unknown", "��������", "�����", "�䱫��", "�����",
                        "�ΰ���", "��������", "������", "���ͽ���", "��������" };

char race_adj[][15] = { "Unknown", "��������", "�����", "�䱫��",
                  "�����", "�ΰ���", "��������", "������", "���ͽ���", "��������" };

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
        { "����", "����", "������", "������",
          "���ڰ�", "��¡��", "�켺", "���" },
        { "�ʺ�", "���û�", "����", "ö��",
          "�Ǽ�", "��Ȳ", "����", "����" },
        { "����", "��̽�", "�ҽ�", "������",
          "����", "���", "����", "��ó" },
        { "����", "Į����", "����", "�뺴",
          "�˰�", "�˼�", "��Ȳ", "������õ" },
        { "�ɺθ���", "������", "������", "����",
          "����", "����", "�ż�", "����" },
        { "������", "����", "����", "����",
          "������", "������", "��ȣ��", "������" },
        { "�̵���", "������", "������", "��������",
          "����", "��������", "�������", "�����屺" },
        { "�ٴõ���", "������", "������", "�ҵ���",
          "�յ���", "����", "����", "�ż�" },
        { "����", "����", "����", "����", "����",
          "����", "����", "����"},
        { "����", "����", "����", "����", "����",
          "����", "����", "����"},
        { "�����", "�����", "�����", "�����",
          "�����", "�����", "�����", "�����" },
        { "�ٺ�", "��û��", "�Ƕ���", "������",
          "�����", "�չٺ�", "��ġȲ��", "�ٺ����ǽ�"  },
};
struct {
        char    *cmdstr; 
        int     cmdno; 
        int     (*cmdfn)(); 
} cmdlist[] = { 
        { "�����±�", 1, move },
        { "����", 1, move },
        { "���", 1, move },
        { "������", 1, move },
        { "��������", 1, move },
        { "��ȸ�㰡", 1, move },
        { "���ϵ���", 1, move},
        { "������", 1, move }, 
        { "�ݾ߹ٶ��", 1, move },
        { "8", 1, move },
        { "��", 1, move },
        { "��", 1, move },
        { "ɪ��", 1, move },
        { "2", 1, move },
        { "��", 1, move },
        { "��", 1, move },
        { "ɥ��", 1, move },
        { "6", 1, move },
        { "��", 1, move },
        { "��", 1, move },
        { "ɦ��", 1, move },
        { "4", 1, move },
        { "��", 1, move },
        { "��", 1, move },
        { "ɬ��", 1, move },
        { "�ϵ�", 1, move },
        { "����", 1, move },
        { "�ϼ�", 1, move },
        { "�ϵ�", 1, move },
        { "����", 1, move },
        { "����", 1, move },
        { "����", 1, move },
        { "����", 1, move },
        { "����", 1, move },
        { "����", 1, move },
        { "9", 1, move },
        { "��", 1, move },
        { "��", 1, move },
        { "ɮ��", 1, move },
        { "3", 1, move },
        { "��", 1, move },
        { "��", 1, move },
        { "ɩ��", 1, move },
        { "��", 1, move },
        { "����", 1, move }, /* leave */
        { "��", 2, look },     /* look */  
        { "����", 2, look },
        { "����", 100, action },
        { "����", 2, look }, /* examine */
        { "��", 3, quit }, /* quit */
        { "��", 4, say },
        { "\"", 4, say },
        { "'", 4, say },
        { "�ֿ�", 5, get }, /* get */
        { "��", 5, get },
        { "����", 5, get }, /* take */
        { "����", 5, get }, /* �߰� */
        { "����ǰ", 6, inventory }, /* inventory */
        { "����", 7, drop }, /* drop */
        { "�־�", 7, drop }, /* put */
        { "����", 8, who }, /* who */
        { "�Ծ�", 9, wear }, /* wear */
        { "����", 10, remove_obj }, /* remove */
        { "���", 11, equipment }, /* equipment */
        { "��", 11, equipment },
        { "���", 12, hold }, /* hold */
        { "���", 12, hold },
        { "����", 13, ready }, /* wield */
        { "����", 14, help }, /* ���� */
        { "?", 14, help },
        { "�ǰ�", 15, health }, /* health */
        { "����", 15, health }, /* score */
        { "����", 16, info }, /* information */
        { "���", 17, send }, /* send */
        { "�̾߱�", 17, send }, /* tell */
        { "����", 18, follow }, /* follow */
        { "������", 19, lose },
        { "�׷�", 20, group }, /* group */
        { "����", 20, group }, /* party */
        { "����", 21, track },
        { "����", 22, peek }, /* peek */
        { "����", 23, attack }, /* attack */
        { "��", 23, attack },
        { "��", 23, attack }, /* kill */
        { "����", 23, attack }, /* k */
        { "�˻�", 24, search }, /* search */
        { "ã��", 24, search },
        { "ǥ��",25, emote},
        { "����", 26, hide },
        { "����", 26, hide },
        { "����", 27, set }, /* set */
        { "����", 28, clear }, /* clear */
        { "����", 29, yell }, /* yell */
        { "��", 30, go }, /* go */
        { "��", 30, go },
        { "����", 31, openexit }, /* open */
        { "�ݾ�", 32, closeexit }, /* close */
        { "Ǯ��", 33, unlock }, /* Ǯ�� */
        { "���", 34, lock }, /* lock */
        { "��", 35, picklock }, /* pick */
        { "����", 36, steal }, /* steal */
        { "����", 37, flee }, /* flee */
        { "��", 37, flee },
        { "�ֹ�", 38, cast }, /* cast */
        { "���", 39, study },
        { "����", 39, study },
        { "�о�", 40, readscroll },
        { "ǰ��", 41, list },
        { "��", 42, buy },
        { "�Ⱦ�", 43, sell },
        { "��ġ", 44, value },
        { "����", 44, value },
        { "���", 45, backstab },
        { "����", 46, train },
        { "��", 47, give },
        { "����", 48, repair },
        { "�ð�", 49, prt_time },
        { "����", 50, circle },
        { "�Ͱ�", 51, bash },
        { "����", 52, savegame }, /* save */
        { "����������", 53, postsend },
        { "��������", 55, postdelete },
        { "�����ޱ�", 54, postread },
        { "��ȭ", 56, talk },
        { "�׷츻",57, gtalk }, /* gtalk */
        { "������",57, gtalk },
        { "=", 57, gtalk },
        { "����", 58, drink }, /* drink */
        { "�Ծ�", 58, drink },
        { "���", 59, broadsend },
        { "��", 59, broadsend },
		{ "ȯȣ", 70, broadsend2 },
        { "zap", 60, zap },
        { "ȯ��", 61, welcome },
        { "��ȥ��", 62, turn },
		{ "������", 63, bank_inv },
		{ "�ܾ�", 63, bank },
		{ "�Ա�", 63, deposit },
		{ "���", 63, withdraw },
		{ "�޾�", 63, output_bank },
        { "Ȱ����", 64, haste },
        { "�ſ���", 65, pray }, /* pray */
        { "���", 66, prepare },
        { "���", 67, use }, /* use */
        { "���ź�", 68, ignore }, /* ignore */
        { "����ڰ˻�", 69, whois },
		{ "����", 73, description },
/*        { "���ż�", 70, sneak },    */
        { "������", 71, teach },
		{ "��������", 72, call_war },
/*
        { "����",72, pledge },
        { "Ż��",73, rescind },
*/
        { "����",74, purchase },
        { "����",75, selection },
        { "��ȯ",76, trade},
        { "��Ŵޱ�", 77, ply_suicide }, /* suicide */
        { "��ȣ", 78, passwd}, /* passwd */
        { "��ǥ",79,vote},
        { "���������", 80, pfinger},
        { "��ȯ", 81, return_square },
        { "��", 81, return_square },
        { "���Ӹ�", 82, ply_aliases },
        { "��", 82, ply_aliases },
        { "�¿�", 83, burn },
        { "�Ұ�", 83, burn },
        { "Īȣ", 84, set_title },
        { "Īȣ����", 84, clear_title },
        { "����", 85, forge },
	{ "���⸸���", 85, newforge }, 
        { "������ȯ", 86, change_class },
        { "�������", 87, power },
        { "�������", 88, accurate },
        { "�����", 89, absorb },
        { "����", 90, meditate },
        { "��������", 91, magic_stop },
        { "��", 92, writeboard },
        { "�ۻ���", 93, del_board },
        { "�Խ���", 94, look_board },
        { "���", 95, chg_name },
        { "����", 96, info_obj },
        { "��", 96, obj_compare },
        { "����", 97, kick },
        { "������", 98, poison_mon },
        { "��°ݹ�", 99, up_dmg },
        { "����ǥ��", 100, action },
        { "�����", 100, action },
        { "����", 100, action },
        { "��", 100, action },
        { "�ƴ�", 100, action },
        { "��", 100, action },
        { "����", 100, action },
        { "�̼�", 100, action },
        { "ûȥ", 100, action },
        { "����", 100, action },
        { "��", 100, action },
        { "��ǰ", 100, action },
        { "����", 100, action },
        { "�̾�", 100, action },
        { "�Ǽ�", 100, action },
        { "�������̺�", 100, action },
        { "�ڼ�", 100, action },
        { "��", 100, action },
        { "���", 100, action },
        { "��", 100, action },
        { "��", 100, action },
        { "��", 100, action },
        { "�뷡", 100, action },
        { "���", 100, action },
        { "�޷�", 100, action },
        { "��Ȳ", 100, action },
        { "����", 100, action },
        { "�β���", 100, action },
        { "���", 100, action },
        { "����", 100, action },
        { "�߰�", 100, action },
        { "����", 100, action },
        { "�ȳ�", 100, action },
        { "�ǻ�", 100, action },
        { "��ũ", 100, action },
        { "����", 100, action },
	{ "����", 100, action },
	{ "�Ⱦ�", 100, action },
	{ "���Ⱦ�", 100, action },
/****************************************************************/
/*  { "����", 149, trans_exp },    */
/****************************************************************/
	{ "���", 149, buy_states },
	{ "��ȥ", 150, marriage },
	{ "�����", 150, m_send },
    { "��ȥ", 150, divorce },   
    { "���", 153, moon_set }, 
	{ "�޸�", 151, memo },
	{ "�ʴ�", 152, invite },
    { "����", 154, enemy_status },
	{ "�аŸ�����", 148, family_who },
	{ "�аŸ�����", 148, family_news },
	{ "�аŸ�����", 148, family }, 
	{ "�����㰡", 148, boss_family },
	{ "�аŸ�Ż��", 148, out_family },
	{ "�аŸ��߹�", 148, fm_out },
	{ "�аŸ���", 148, family_member },
	{ "�аŸ���", 148, family_talk },
	{ "]", 148, family_talk },
	{ "����аŸ�", 148, list_family },
	{ "���", 153, resend },
	{ "/", 153, resend },
        { "*teleport", 101, dm_teleport },
        { "*�����̵�", 101, dm_teleport },
        { "*���ȣ", 102, dm_rmstat },
        { "*rm", 102, dm_rmstat },
        { "*��", 102, dm_rmstat },
        { "*reload", 103, dm_reload_rom },
        { "*�ε�", 103, dm_reload_rom },
        { "*save", 104, dm_resave },
        { "*���̺�", 104, dm_resave },
        { "*create", 105, dm_create_obj },
        { "*�������ٸ����", 105, dm_create_obj},
        { "*c", 105, dm_create_obj },
        { "*perm", 106, dm_perm },
        { "*����", 106, dm_perm },
        { "*invis", 107, dm_invis },
        { "*�ٶ�ó�������", 107, dm_invis },
        { "*i", 107, dm_invis },
        { "*s", 108, dm_send },
        { "*send", 108, dm_send },
        { "*����", 108, dm_send },
        { "*purge", 109, dm_purge },
        { "*û��", 109, dm_purge },
        { "*û", 109, dm_purge },
        { "*ac", 110, dm_ac },
        { "*����", 110, dm_ac },
        { "*users", 111, dm_users },
        { "*��", 111, dm_users },
        { "*����", 111, dm_users },
        { "*echo", 112, dm_echo },
        { "*��", 112, dm_echo },
        { "*flushrooms", 113, dm_flushsave },
        { "*�������", 113, dm_flushsave },
        { "*shutdown", 114, dm_shutdown },
        { "*����", 114, dm_shutdown },
        { "*f", 115, dm_force },
        { "*force", 115, dm_force },
        { "*�������ٽ���", 115, dm_force },
        { "*flushcrtobj", 116, dm_flush_crtobj },
        { "*�缳��", 116, dm_flush_crtobj },
        { "*��", 116, dm_flush_crtobj },
        { "*monster", 117, dm_create_crt },
        { "*����", 117, dm_create_crt },
        { "*��", 117, dm_create_crt },
        { "*status", 118, dm_stat },
        { "*����", 118, dm_stat },
        { "*add", 119, dm_add_rom },
        { "*������", 119, dm_add_rom },
        { "*����������", 120, dm_set },
        { "*log", 121, dm_log },
        { "*����", 121, dm_log },
        { "*spy", 122, dm_spy },
        { "*�������ٿ���", 122, dm_spy },
        { "*lock", 123, dm_loadlockout },
        { "*����", 123, dm_loadlockout },
        { "*finger", 124, dm_finger },
        { "*�ΰ�", 124, dm_finger },
        { "*list", 125, dm_list },
        { "*���������ٺ�", 125, dm_list }, 
        { "*info", 126, dm_info },
        { "*����", 126, dm_info },
        { "*parameter", 127, dm_param },
        { "*��ġ", 127, dm_param },
        { "*silence", 128, dm_silence},
        { "*���", 128, dm_silence},
        { "*broad", 129, dm_broadecho},
        { "*���", 129, dm_broadecho },
        { "*replace",130, dm_replace},
        { "*��ü", 130, dm_replace},
        { "*name",131, dm_nameroom},
        { "*���̸�", 131, dm_nameroom},
        { "*append",132, dm_append},
        { "*�߰�", 132, dm_append},
        { "*prepend",133, dm_prepend},
        { "*����", 133, dm_prepend},     
        { "*gcast",134, dm_cast},
        { "*���ֹ�", 134, dm_cast},
        { "*group",135, dm_group},
        { "*�׷�", 135, dm_group},
        { "*notepad",136, notepad},
        { "*�޸�", 136, notepad},
        { "*delete",137, dm_delete},
        { "*�����", 137, dm_delete},
        { "*oname", 138, dm_obj_name },
        { "*�������ٹٲ�", 138, dm_obj_name },
        { "*cname", 139, dm_crt_name },
        { "*�����̸�", 139, dm_crt_name },
        { "*active", 140, list_act },
        { "*Ȱ��", 140, list_act },
        { "*dust", 141, dm_dust },
        { "*��������ȭ����", 141, dm_dust },
        { "*cfollow", 142, dm_follow },
        { "*������", 142, dm_follow },
        { "*��������", 148, dm_moonstone },
        { "*ħ��", 148, dm_monster },
        { "*dmhelp", 143, dm_help },
        { "*����", 143, dm_help },
        { "*attack", 144, dm_attack },
        { "*����", 144, dm_attack },
        { "*enemy", 145, list_enm },
        { "*��", 145, list_enm },
        { "*charm", 146, list_charm },
        { "*�ָ�", 146, list_charm },
        { "*���������", 147, dm_save_all_ply },
        { "����", -2, 0 },
        { "�о�", -2, 0 },
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
                  { "ȸ��", SVIGOR, vigor, 1 },
                  { "��ǳ", SHURTS, offensive_spell, 2 },
                  { "�߱�", SLIGHT, light, 2 },
                  { "�ص�", SCUREP, curepoison, 1 },
                  { "������", SBLESS, bless, 2 },
                  { "��ȣ��", SPROTE, protection, 2 },
                  { "ȭ��", SFIREB, offensive_spell, 3 },
                  { "���й�", SINVIS, invisibility, 5 },
                  { "���¹�", SRESTO, restore, 4 },
                  { "���а�����", SDINVI, detectinvis, 3 },
                  { "�ֹ�������", SDMAGI, detectmagic, 3 },
                  { "������", STELEP, teleport, 4 },
                  { "ȥ��", SBEFUD, befuddle, 3 },
                  { "����", SLGHTN, offensive_spell, 4 },
                  { "������", SICEBL, offensive_spell, 5 },
                  { "����", SENCHA, enchant, 4 },
                  { "��ȯ", SRECAL, recall, 3 },
                  { "��ȯ", SSUMMO, summon, 3 },
                  { "����ȸ��", SMENDW, mend, 3 },
                  { "��ġ", SFHEAL, heal, 5 },
                  { "����", STRACK, magictrack, 4 },
                  { "�ξ��", SLEVIT, levitate, 3 },
                  { "�濭��", SRFIRE, resist_fire, 3 },
                  { "����", SFLYSP, fly, 4 },
                  { "������", SRMAGI, resist_magic, 3 },
                  { "��ǳ��", SSHOCK, offensive_spell, 5 },
                  { "������", SRUMBL, offensive_spell, 2 },
                  { "ȭ����", SBURNS, offensive_spell, 2 },
                  { "ź����", SBLIST, offensive_spell, 2 },
                  { "ǳ����", SDUSTG, offensive_spell, 3 },
                  { "���ʽ�", SWBOLT, offensive_spell, 3 },
                  { "����", SCRUSH, offensive_spell, 3 },
                  { "����", SENGUL, offensive_spell, 5 },
                  { "ȭǳ��", SBURST, offensive_spell, 3 },
                  { "ȭ���õ", SSTEAM, offensive_spell, 3 },
                  { "���ռ�", SSHATT, offensive_spell, 4 },
                  { "������", SIMMOL, offensive_spell, 4 },
                  { "����õ", SBLOOD, offensive_spell, 4 },
                  { "��õǳ", STHUND, offensive_spell, 5 },
                  { "������", SEQUAK, offensive_spell, 5 },
                  { "�¾��", SFLFIL, offensive_spell, 5 },
                  { "���ǰ���", SKNOWA, know_alignment, 3 },
                  { "�����ؼ�", SREMOV, remove_curse, 4 },
                  { "������", SRCOLD, resist_cold, 3 },
                  { "������", SBRWAT, breathe_water, 3 },
                  { "����ȣ", SSSHLD, earth_shield, 3 },
                  { "õ����", SLOCAT, locate_player, 4 },
                  { "��ġ��", SDREXP, drain_exp, 5 },
                  { "ġ��", SRMDIS, rm_disease, 3 },
                  { "���ȼ�", SRMBLD, rm_blind, 3 },
                  { "����", SFEARS, fear, 4 },
                  { "��ȸ��", SRVIGO, room_vigor, 4 },
                  { "����", STRANO, object_send, 4 },
                  { "�Ǹ�", SBLIND, blind, 5 },
                  { "���ձ�", SSILNC, silence, 5 },
                  { "��ȥ���", SCHARM, charm, 5 },
/*                { "�������ѱ�", SNAHAN, offensive_spell },     */
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
/*                { SNAHAN,  WIND, 10, 2, 5, 13, 2 },  */ /* �������ѱ� */
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


