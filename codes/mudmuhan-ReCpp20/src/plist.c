#include <stdio.h>
#include <ctype.h>
#include "mstruct.h"
#include "mextern.h"

#define set_flag(f,n) ((f) |= 1 << (n))
#define clr_flag(f,n) ((f) &= ~(1 << (n)))
#define is_fset(f,n) ((f) & 1 << (n))

main(int argc, char *argv[])
/* plist coomand provides a quick way to check a given players *
 * level, class, password and inventory without having to log  *
 * the player in or use the editor */

{
creature	*player;
int		i,adj,lvl=0;
char		flags = 0;

	adj = 1;

 
    if (argc < 2) {
        printf("syntax: plist [[-pni] [-l #]] <players>\n");
        return;
    }
 
    for (i=1;i<argc;i++){
 
        if (argv[i][0] == '-'){
            switch(argv[i][1]){
                case 'i':
		    set_flag(flags,0);
                    continue;
                break;
                case 'n':
		    set_flag(flags,1);
                    continue;
                case 'p':
		    set_flag(flags,2);
                    continue;
                break;  
            }
 
            if(i+1 < argc && isdigit(argv[i+1][0])){
                switch(argv[i][1]){
                    case 'l':
                        lvl = atoi(argv[i+1]);
                    break;
                    default:
                       set_flag(flags,3); 
                    break;
                }
                i++;
            }
            else
                set_flag(flags,3); 
        }
	else
	    break;
 
        if (is_fset(flags,3)){
            printf("syntax: plist [[-pni] [-l #]] <players>\n");
            return;
        }
            
    } 
	adj = i;

	if (adj >= argc){
        	printf("syntax: plist [[-pni] [-l #]] <players>\n");
		return;
	}

	for(i= adj;i < argc; i++){

		if(!isupper(argv[i][0]))
			continue;	
		if(load_ply(argv[i], &player) < 0){
			printf("Player Load Error (%s).\n", argv[i]);
			continue;
		}

		/* not lvl check or player = lvl */
		if( !lvl || player->level == lvl){
		   printf("%s%s",(!is_fset(flags,1)) ? "Name: " : "",
			player->name);

		/*print  password */
		if (is_fset(flags,2))
			printf(" (%s)\n",player->password);
		else
			printf("\n");

		/* if not name only display */
		if(!is_fset(flags,1))
		    printf("Class: %s  (%d) %s\n",class_str[player->class],
		       player->level,race_str[player->race]);

		/* print players inventory */
		if(is_fset(flags,0)){
		    otag *obj, *cnt;
		    int len = 0;

		    printf("Inventory:\n");
		    obj = player->first_obj; 
		    if(!obj) printf("none");

		    while(obj){
			printf("%s%s",obj->obj->name, (obj->next_tag) ?
			    (F_ISSET(obj->obj,OCONTN)) ? ":" :", " : ".");
			len += strlen(obj->obj->name);

			if(F_ISSET(obj->obj,OCONTN)){
			    cnt = obj->obj->first_obj;
			    printf("(");
			    if (!cnt) printf("empty");
			    while(cnt){
				printf("%s%s",cnt->obj->name, 
				  (cnt->next_tag) ?  ", " : "");
				len += strlen(cnt->obj->name);
				if (len > 50){
					printf("\n");
					len = 0; }
				cnt = cnt->next_tag;
			    }
			    printf(")%s",(obj->next_tag) ? ", " : ".");
			}
			if (len > 50){
				printf("\n");
				len = 0;
			}
                       obj = obj->next_tag;    
		    }
		       printf("\n");
		}
                }

		free_crt(player);
	}

	return;
}
