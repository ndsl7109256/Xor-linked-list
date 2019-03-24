#include <stdio.h>                                                                                                                      
#include <stdlib.h>

typedef struct {
    unsigned long lxr;
    int data1, data2;
} item;
item *lh = NULL;

typedef struct { item *left, *right; } lxr_info;

void print_at(lxr_info *p_lxri, item *pli) {
    if (pli)
        printf("data1 = %d, data2 = %d\n", pli->data1, pli->data2);
}

static inline item *extract_address(item *lxr, item *other) {
    return (item *) (lxr->lxr ^ (unsigned long) other);
}

static inline void shift_lxr(item *lxr, item *known, item *update) {
    lxr->lxr = (lxr->lxr ^ (unsigned long) known) ^ (unsigned long) update;
}

static inline item *lxr_move_right(lxr_info *lxr,item *pcur){
    item *p_this, *p_left, *p_right,*c;

    p_right = (*lxr).right;
    p_left = (*lxr).left;

    p_this = p_right;
    lxr->left = pcur;
    if(lxr->right){
        c = extract_address(pcur,p_left);
        lxr->right = extract_address(c,pcur);
    }

    return p_this;
}



/* Walk the list starting at the header */
void list_walk(void) {
    item *pcur = lh;
    lxr_info lxri = {.left = NULL, .right = NULL};
    lxri.right = extract_address(pcur, lxri.left);
    while (pcur) {
        print_at(&lxri, pcur);
        pcur = lxr_move_right(&lxri, pcur);
    }
}

/* Append an item where we are */
item *append_item(lxr_info *p_lxri, item *p_current, int d1, int d2) {
    item *p_this, *p_left, *p_right;
    if ((p_this = calloc(1, sizeof(item)))) {
        p_left = p_lxri->left = p_current;
        p_right = p_lxri->right;
        if (p_left) shift_lxr(p_left, p_right, p_this);
        if (p_right) shift_lxr(p_right, p_left, p_this);
        p_this->lxr = (unsigned long) p_current ^ (unsigned long) p_lxri->right;
        p_this->data1 = d1;
        p_this->data2 = d2;
    }
    return p_this;
}

int main(int argc, char **argv) {
    int index = 0;
    int maxrun = 20;
    item *pcur = lh, *pnli;
    lxr_info lxri = {.left = NULL, .right = NULL};

    while (index < maxrun) {
        if ((pnli = append_item(&lxri, pcur, 3 * index, 3 * index + 1)) ==
            NULL) {
            exit(2);
        }
        pcur = pnli;
        if (!lh)
            lh = pcur;
        index++;
    }

    list_walk();
    return 0;
}
