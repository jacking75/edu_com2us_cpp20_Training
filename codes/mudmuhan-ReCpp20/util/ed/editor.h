#include <stdio.h>
#ifndef TC
#include <sys/file.h>
#endif

extern int	count_obj(), write_obj(), count_inv(), write_crt(),
		count_mon(), count_ite(), count_ext(), count_ply(),
		write_rom(), read_obj(), read_crt(), read_rom();
extern void	free_obj(), free_crt(), free_rom();

extern void     edit_object(), edit_monster(), edit_room(), edit_exits(),
		clearscreen(), posit(), getnum(), getnums(), getstr(), 
		savepos(), restorepos(), zero(), merror();
extern int	load_obj_from_file(), save_obj_to_file(), 
		load_crt_from_file(), save_crt_to_file(),
		load_rom_from_file(), save_rom_to_file();

extern void	check_ply(), edit_player(), edit_items();
extern int	load_ply_from_file(), save_ply_to_file();
extern int ishan(), is_hangul();
extern char *first_han();
