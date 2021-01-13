/*
 * MEXTERN.H:
 *
 *	This file contains the external function and variable 
 *	declarations required by the rest of the program.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include <stdio.h>
#include <sys/file.h>

#ifndef MIGNORE

/* 전쟁 플레그 외부변수 선언 */
extern int            AT_WAR;
extern int            CALLWAR1;
extern int            CALLWAR2;

extern int		Tablesize;
extern int		Cmdnum;
extern long		Time;
extern struct lasttime	Shutdown;
extern int		Spy[PMAX];
extern int		Numlockedout;
extern lockout		*Lockout;

extern struct {
	creature	*ply;
	iobuf		*io;
	extra		*extr;
} Ply[PMAX];

extern struct {
	short		hpstart;
	short		mpstart;
	short		hp;
	short		mp;
	short		ndice;
	short		sdice;
	short		pdice;
} class_stats[12];

extern struct cmdstruct {
	char	*cmdstr;
	int	cmdno;
	int	(*cmdfn)();
} cmdlist[];

extern struct {
	char	*splstr;
	int	splno;
	int	(*splfn)();
	int	spllv;
} spllist[];

extern struct {
	int	splno;
	char	realm;
	int	mp;
	int	ndice;
	int	sdice;
	int	pdice;
	char	bonus_type;
} ospell[];

extern short	level_cycle[][10];
extern short	thaco_list[][20];
extern long	quest_exp[];
extern int	bonus[35];
extern char	class_str[][15];
extern char	race_str[][15];
extern char	race_adj[][15];
extern char	lev_title[][8][20];
extern char 	article[][10];
extern char 	number[][10];
extern long	needed_exp[25];
extern char   family_str[16][15];
extern char   fmboss_str[16][15];
extern int	family_num[16];
extern int	family_gold[16];

#endif

/* alias.c */
#define MAX_ALIAS_BUF 64 /* 한 줄임말내의 명령어 최대 갯수 */

extern int ply_aliases();
extern int alias_cmd();
extern void init_alias();
extern void close_alias();
extern int set_title();
extern int clear_title();
extern char *ansi_title();

/*
extern nstrcat();
*/
/* FILES1.C */

extern int	count_obj(), write_obj(), count_inv(), write_crt(),
		count_mon(), count_ite(), count_ext(), count_ply(),
		write_rom(), read_obj(), read_crt(), read_rom();
extern void	free_obj(), free_crt(), free_rom();

/* FILES2.C */

extern int	load_rom(), load_mon(), load_obj(), load_ply(), save_ply(),
		is_rom_loaded(), reload_rom(), resave_rom();
extern void	put_queue(), pull_queue(), front_queue(), flush_rom(),
		flush_crt(), flush_obj(), resave_all_rom(), save_all_ply();

/* FILES3.C */

extern int	write_obj_to_mem(), write_crt_to_mem(), read_obj_from_mem(),
		read_crt_from_mem(), load_crt_tlk(), talk_crt_act();

/* BANK.C */
extern int load_bank(), save_bank();
extern int input_bank(), output_bank(), bank_inv(), bank();
extern int deposit(), withdraw();
extern void drop_all_bank(), get_all_bank();

/* IO.C */

extern int	io_check(), accept_input(), locked_out(), addr_equal(),
		remove_wait();
extern void	sock_init(), sock_loop(), accept_connect(), output_buf(), 
		print(), handle_commands(), disconnect(), broadcast(), broadcast2(), 
		broadcast_all(), broadcast_wiz(), broadcast_rom(), broadcast_rom2(),
		broadcast_robot_rom(), add_wait(), init_connect(),
		waiting(), child_died(), reap_children();

/* COMMAND1.C */

extern void	login(), create_ply(), command(), parse();
extern int	process_cmd();
extern int    checkdouble();

extern int 	ishan();
extern int	is_hangul();
extern char *first_han();
extern int	under_han();
extern int 	is_number();
extern int	cut_command();
extern void	paste_command();
extern int	return_square();
extern int	ply_is_attacking();
extern char  *cut_space();
extern int    str_compare();

/* COMMAND2.C */

extern int	look(), move(), say(), get(), inventory(), drop(), burn(),
                auto_search();
extern void	get_all_rom(), get_all_obj(), drop_all_rom(), drop_all_obj(),
		get_perm_obj();

/* COMMAND3.C */

extern int	wear(), remove_obj(), equipment(), ready(), hold(),
              info_obj();
extern void	wear_all(), remove_all(), equip_list();

/* COMMAND4.C */

extern int	health(), info(), follow(), lose(), group(), track(), peek(),
		broadsend2(), help(), welcome(), send(), broadsend();
extern void	info_2();

/* COMMAND5.C */

extern int	attack(), who(), search(), hide(), set(), clear(), 
		quit(), ply_suicide(), attack_crt(), whois();
extern void	suicide();

/* COMMAND6.C */

extern int	yell(), go(), openexit(), closeexit(), unlock(), lock(),
		picklock(), steal();

/* COMMAND7.C */

extern int	flee(), list(), buy(), sell(), value(), backstab(), train(),
                newforge(), forge(), change_class(), magic_stop(), poison_mon();

/* COMMAND8.C */

extern int	give(), repair(), prt_time(), circle(), bash(), savegame(), 
		talk(), chg_name(), moon_set(), kick();
extern int savegame_nomsg();
extern void	give_money(), talk_action(), enemy_status();

/* COMMAND9.C */

extern int	bribe(), haste(), pray(), power(), accurate(), meditate(),
                up_dmg(), prepare(), use(), ignore(), pfinger();

/* COMMAND10.C */

extern int	sneak(), gtalk(), pledge(), rescind(), purchase(),
		selection(), trade(), room_obj_count();

extern void 	lower_prof(), add_prof(), lose_all(), dissolve_item();

/* COMMAND11.C */ 

extern int	emote();

extern int	passwd(), vote();

extern int	pfinger();

extern int	load_family();
extern int 	family();
extern int 	boss_family();
extern int	out_family();
extern int	family_who();
extern int 	family_talk();
extern int	list_family();
extern int	trans_exp();
extern int 	buy_states();
extern int	marriage(), divorce();
extern int	m_send();

/* COMMAND12.C */

extern int 	obj_compare();
extern int 	memo();
extern int 	fm_out();
extern int family_member();
extern void edit_member();
extern int	invite();
extern int  check_war(), check_war_one();
extern int 	description();
extern int	resend();

/* MAGIC1.C */

extern int	cast(), teach(), study(), readscroll(), drink(), zap(),
		zap_obj(), offensive_spell();

/* MAGIC2.C */

extern int	vigor(), curepoison(), light(), protection(), mend();

/* MAGIC3.C */

extern int	bless(), invisibility(), restore(), turn(), absorb(), befuddle();

/* MAGIC4.C */

extern int	detectinvis(), detectmagic(), teleport(), enchant();

/* MAGIC5.C */

extern int	recall(), summon(), heal(), magictrack(), levitate(),
		resist_fire(), fly();

/* MAGIC6.C */

extern int	resist_magic(), know_alignment(), remove_curse();

/* MAGIC7.C */

extern int	resist_cold(), breathe_water(), earth_shield(),
		locate_player(), drain_exp (), rm_disease(), object_send();

/* MAGIC8.C */

extern int	room_vigor(), fear(), rm_blind(), silence(), blind(), charm(), spell_fail(); 

/* DM1.C */

extern int	dm_teleport(), dm_send(), dm_purge(), dm_users(),
		dm_echo(), dm_flushsave(), dm_rmstat(), dm_reload_rom(),
		dm_resave(), dm_create_obj(), dm_perm(), dm_invis(),
		dm_ac(), dm_shutdown(), dm_force(), dm_flush_crtobj(),
		dm_create_crt(),dm_save_all_ply();

/* DM2.C */

extern int	stat_rom(), stat_crt(), stat_obj(), dm_stat(),
		dm_add(), dm_add_rom(), dm_add_ext(), dm_add_crt(),
		dm_add_obj(), dm_spy();

/* DM3.C */

extern void	link_rom(), expand_exit_name();
extern int	dm_set(), dm_set_rom(), dm_set_ext(), dm_set_crt(),
		dm_set_obj(), del_exit(), dm_log(), dm_loadlockout(),
		dm_finger(), dm_list(), dm_info(), dm_set_xflg();

/* DM4.C */

extern int	dm_param(), dm_silence(), dm_broadecho(), 
		dm_cast(), dm_group(), dm_view(), dm_obj_name(),
		dm_crt_name();

/* DM5.C */
extern int     dm_replace(), desc_search(), dm_nameroom(), dm_append(), 
               dm_prepend(), dm_delete(), dm_moonstone(), dm_help(),
               dm_monster(); 
               

extern void 	txt_parse();

/* DM6.C */
extern int	dm_dust(), dm_follow(), dm_attack(), list_enm(), list_charm();

/* PLAYER.C */

extern void	init_ply(), uninit_ply(), update_ply(), up_level(),
		down_level(), add_obj_crt(), del_obj_crt(), compute_ac(), 
		compute_thaco();
extern char	*title_ply(), *ply_prompt();
extern int	weight_ply(), max_weight(), profic(), mprofic(),
		mod_profic(), has_light(), fall_ply();
extern creature	*find_who(), *lowest_piety(), *low_piety_alg(), *enemy_ply();

/* CREATURE.C */

extern creature	*find_crt();
extern int 	add_enm_crt(), del_enm_crt(), is_enm_crt(), add_charm_crt(), 
		del_charm_crt(), is_charm_crt();
extern void	end_enm_crt(), die(), temp_perm(), die_perm_crt(), 
		check_for_flee(), consider(), add_enm_dmg(), display_status();

/* ROOM.C */

extern void	add_ply_rom(), del_ply_rom(), add_obj_rom(),
		del_obj_rom(), add_crt_rom(), del_crt_rom(), 
		add_permcrt_rom(), add_permobj_rom(), 
		check_exits(), display_rom(), check_traps();
extern exit_	*find_ext();
extern int	count_vis_ply();

/* OBJECT.C */

extern void	add_obj_obj(), del_obj_obj(), rand_enchant();
extern int	list_obj(), weight_obj(), check_event_obj();
extern object	*find_obj();

/* UPDATE.C */

extern void	update_game(), update_users(), update_random(),
        clear_enm_crt(), update_active(), update_time(), update_moonstone(),
        update_exit(), update_shutdown(), add_active(), del_active(),
        update_monster(), update_monster_two();
extern int	crt_spell(), choose_item();

/* POST.C */

extern int	postsend(), postread(), postdelete(), notepad();
extern int  family_news();
extern void	postedit(), noteedit(), newsedit();

/* ACTION.C */

extern int	action();

/* MISC.C */

extern void	merror(), lowercize(), zero(), delimit(), view_file(), logf(),
		sort_cmds(), load_lockouts(), please_wait(), logn();
extern int	low(), up(), dice(), exp_to_lev(), dec_daily(), sort_cmp(),
		file_exists(), isnumber();
extern char	*crt_str(), *obj_str();

/* SPECIAL1.C */

extern int  call_war(), special_obj(), special_read(), is_bad_item();

extern void check_item(), check_contain(), free_obj2();
#ifdef COMPRESS

/* files2.c */
extern int load_crt();

/* COMPRESS.C */

extern int	compress(), uncompress();


#endif
extern int nirvana(),list_act();
