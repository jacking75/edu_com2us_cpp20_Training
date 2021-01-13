#include "mstruct.h"
#include "mextern.h"

int init_ply2(ply_ptr)
creature *ply_ptr;
{
    int handle,fd;
    creature2 *new_ptr;

    fd=ply_ptr->fd;

    new_ptr=(creature2 *)malloc(sizeof(creature2));
    if(!new_ptr) merror("init_ply2",FATAL);

    strcpy(new_ptr->title,
    if((handle=open(name,O_RDONLY))==-1) {
        
        save_ply2(fd);
        return;
    }

}

