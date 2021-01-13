
#include "mstruct.h"
#include "editor.h"
#include "mtype.h"
#include "mextern.h"
#include <time.h>
#include "kstbl.h"

#ifdef TC
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#endif

void main()
{
	FILE *fp;
	char file[80];
	char str[256];
	object *bnk_ptr;
	creature *ply_ptr;
	int n;

	clearscreen();
    posit(3,20); printf("Enter player name to edit: ");
    getstr(str, "", 80);
    if(!str[0]) return;

	load_ply_from_file(str, &ply_ptr);
	
	n = load_bank(ply_ptr->name, &bnk_ptr);
	if(n < 0){
	        printf("º¸°üÇÏ°í ÀÖ´Â ¹°ÇÀÌ ¾ø½À´Ï´Ù.");
            bnk_ptr = (object *)malloc(sizeof(object));
            zero(bnk_ptr, sizeof(object));
            bnk_ptr->shotsmax = 200;
            F_ISSET(bnk_ptr, OCONTN);
            n = save_bank(ply_ptr->name, bnk_ptr);
            free_obj(bnk_ptr);
    }
    else {
            printf( "´ç½ÅÀÇ ÀÌ¸§ÀÌ »õ°ÜÁø º¸°üÇÔÀÔ´Ï´Ù.\n");
            strcpy(str, "º¸°üÇ°ÀÇ ¸ñ·Ï : ");
            n = list_obj(&str[16], ply_ptr, bnk_ptr->first_obj);
            if(n)   printf( "%s.\n", str);
            else    printf( "º¸°üÇÏ°í ÀÖ´Â ¹°ÇÀÌ ¾ø½À´Ï´Ù.");
		    printf( "ÀºÇà¿¡ ÀÜ°í´Â %1d³ÉÀÔ´Ï´Ù.", bnk_ptr->value);
    }		
}
