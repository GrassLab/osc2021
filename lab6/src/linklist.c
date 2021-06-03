# include "linklist.h"

template <typename T> void ll_push_front(T **head, T *elm){
  T *head_t = *head;
  if(!head_t){
    elm->next = 0;
    elm->pre = 0;
    *head = elm;
    return ;
  }
  elm->next = head_t;
  elm->pre = 0;
  head_t->pre = elm;
  *head = elm;
  return ;
}


template <typename T> void ll_push_back(T **head, T *elm){
  T *head_t = *head;
  if(!head_t){
    elm->next = 0;
    elm->pre = 0;
    *head = elm;
    return ;
  }
  while(head_t->next){
    head_t = head_t->next;
  }
  head_t->next = elm;
  elm->next = 0;
  elm->pre = head_t;
}

template <typename T> void ll_push_elm(T **head, T *elm, T *target){
  T *head_t = *head;
  if(!head_t && !target){
    elm->next = 0;
    elm->pre = 0;
    *head = elm;
    return ;
  }
  if(!target){
    elm->next = *head;
    elm->pre = 0;
    elm->next->pre = elm;
    *head = elm;
    return ;
  }
  elm->next = target->next;
  elm->pre = target;
  if(target->next) target->next->pre = elm;
  target->next = elm;
}

template <typename T> T* ll_pop_front(T **head){
  T *head_t = *head;
  if(!head_t){
    return 0;
  }
  T* r = head_t;
  *head = r->next;
  r->next = 0;
  r->pre = 0;
  return r;
}

template <typename T> T* ll_pop_back(T **head){
  T *head_t = *head;
  if(!head_t){
    return 0;
  }
  while(head_t->next){
    head_t = head_t->next;
  }
  T* r = head_t;
  *head = r->next;
  r->next = 0;
  r->pre = 0;
  return r;
}

template <typename T> void ll_rm_elm(T **head, T *target){
  T *head_t = *head;
  if(!head_t){
    return ;
  }
  if(!target->pre && !target->next){
    *head = 0;
    return ;
  }
  if(!target->pre){
    *head = target->next;
    target->next->pre = 0;
    return ;
  }
  if(!target->next){
    target->pre->next = 0;
    return ;
  }
  target->pre->next = target->next;
  target->next->pre = target->pre;
}
