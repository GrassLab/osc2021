#include "../include/list.h"

void list_push(RUN_Q* q,RUN_Q_NODE *tmp){
    if(q->beg == 0){
        q->beg = q->end = tmp;
    }else{
        tmp->prev = q->end;
        tmp->next = q->beg;
        q->end->next = tmp;
        q->beg->prev = tmp;
        q->end = tmp;
    }

}

RUN_Q_NODE* list_pop(RUN_Q *q){
    if((q->beg) == 0){
        return 0;
    }else{
        if((q->beg->next) == q){
            RUN_Q_NODE *tmp = q->beg;
            q->beg = q->end = 0;
            tmp->next = tmp->prev = 0;
            return tmp;
        }else{
            RUN_Q_NODE *tmp = q->beg;
            q->beg = q->beg->next;
            q->beg->prev = q->end;
            q->end->next = q->beg;
            tmp->next= 0;
            tmp->prev = 0;
            return tmp;
        }
    }


