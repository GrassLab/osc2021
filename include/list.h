typedef struct list_head {
	struct list_head *next, *prev;
}list_head_t;

static inline void INIT_list_head_t(list_head_t *list){
	list->prev = list->next = list;
}

static inline void __list_add(list_head_t *new_, list_head_t *prev, list_head_t *next){

	next->prev = new_;
	new_->next = next;
	new_->prev = prev;
	prev->next = new_;

}

static inline void list_add(list_head_t *new_, list_head_t *head){
	__list_add(new_, head, head->next);
}

static inline void list_add_tail(list_head_t *new_, list_head_t *head){
	__list_add(new_, head->prev, head);
}

static inline void list_remove(list_head_t *list_a, list_head_t *list_b){
	list_a->prev->next=list_b->next;
    list_b->next->prev=list_a->prev;
}

static inline int list_empty(list_head_t *head){
	return head->next == head;
}