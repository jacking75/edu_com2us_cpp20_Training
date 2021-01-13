
/* 두번째 캐릭터 화일 */
typedef struct creature2 {
    char title[80];
    long bank;
    otag *inventory_box;
    int  alias_num;
    char flags[16];
    int  empty[1024-27];
} creature2;

typedef struct alias_tag {
    struct alias_tag *next;
    char key[16];
    char alias[128];
} atag;

typedef struct {
    atag *first_alias;
    ctag *group;
} extra2;

struct {
    creature2 *ply;
    extra2 *extr;
} Ply2[PMAX];

