#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "linked_list.h"
#include "iterator.h"
#include "../../src/gc.h"

struct link {
	ioopm_elem_t value;
	ioopm_link_t *next;
};

struct iter {
	ioopm_link_t *current;
	ioopm_list_t *list;	
};



ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list, heap_t *h) {
	char *iter_fs = "2*"; /// Modified
	ioopm_list_iterator_t *result = h_alloc_struct(h, iter_fs); /// Modified
	result->current = list->first;
	result->list = list;
	return result;
}

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter) {
  return iter->current->next != NULL; 
}

ioopm_elem_t ioopm_iterator_next(ioopm_list_iterator_t *iter) {
	if(ioopm_iterator_has_next(iter)) {
		iter->current = iter->current->next;	
	}
	return iter->current->value;
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter) {
	iter->current = iter->list->first;
}

ioopm_elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter) {
	return iter->current->value;	
}

ioopm_elem_t ioopm_iterator_remove(ioopm_list_iterator_t *iter) {
	ioopm_link_t *to_remove = iter->current->next;
	ioopm_elem_t value = to_remove->value;
	iter->current->next = to_remove->next;
	iter->list->size-=1;
	return value;
}

void ioopm_iterator_insert(ioopm_list_iterator_t *iter, ioopm_elem_t element, heap_t *h) {
	/// The elem_t size is 8 bytes, the largest type in the union
	char *link_fs = "2*"; /// Modified
    ioopm_link_t *new_link = h_alloc_struct(h, link_fs); /// Modified
	new_link->value = element;
	
    if(iter->list->size == 0) {
        iter->list->first = new_link;
		iter->list->last = new_link;
    }else if(iter->current == iter->list->last){
		iter->current->next = new_link;
		iter->list->last = new_link;
	}else {
		ioopm_link_t *tmp = iter->current->next;
		iter->current->next = new_link;
		new_link->next = tmp;
    }
	iter->current = new_link;
    iter->list->size += 1;
}

bool element_comparison(ioopm_elem_t *element){
	
	return true;
}


ioopm_list_t *ioopm_linked_list_create(ioopm_cmp_t *cmp, heap_t *h) {
	char *list_fs = "3*i"; /// Modified 
	ioopm_list_t *list = h_alloc_struct(h, list_fs); /// Modified
	list->cmp = cmp;
	list->first = NULL;
	list->last = NULL;
	list->size = 0;
	return list;
}

//find a link in a linked list at a certain index!
static ioopm_link_t *find_link(ioopm_list_t *list, int index) {
	ioopm_link_t *current_link = list->first;
	for(int i = 0; i < index; i++) {
		current_link = current_link->next;
	}
	return current_link;
}

void ioopm_linked_list_append(ioopm_list_t *list, ioopm_elem_t value, heap_t *h) {
	char *link_fs = "2*"; /// Modified
	ioopm_link_t *new_link = h_alloc_struct(h, link_fs); /// Modified
	new_link->value = value;
	//check if the list is empty!
	if(list->size == 0) {
		list->first = new_link;
		list->last = new_link;
	} else {
		list->last->next = new_link;
		list->last = new_link;
	}
	list->size++;
}

void ioopm_linked_list_prepend(ioopm_list_t *list, ioopm_elem_t value, heap_t *h) {
	char *link_fs = "2*"; /// Modified
	ioopm_link_t *new_link = h_alloc_struct(h, link_fs); /// Modified
	new_link->value = value;
	//check if the list is empty!
	if(list->size == 0) {
		list->first = new_link;
		list->last = new_link;
	} else {
		new_link->next = list->first;
		list->first = new_link;
	}
	list->size++;
}

ioopm_elem_t ioopm_linked_list_remove(ioopm_list_t *list, int index) {
	//not valid index?
	if(index < 0) {
		return list->first->value;
	} else if(index > list->size-1) {
		return list->last->value;
	}

	ioopm_link_t *current_link = find_link(list,index);
	ioopm_elem_t value = current_link->value;

	if(index == 0) {
		list->first = list->first->next;
	} else if(current_link->next != NULL) {
		ioopm_link_t *prev_link = find_link(list,index-1);
		prev_link->next = current_link->next;
	}

	list->size -= 1;
	return value;	
}

ioopm_elem_t ioopm_linked_list_get(ioopm_list_t *list, int index) {
	//not valid index?
	if(index < 0) {
		return list->first->value;
	} else if(index > list->size-1) {
		return list->last->value;
	}
	
	ioopm_link_t *current_link = find_link(list,index);
	return current_link->value;
}

bool ioopm_linked_list_contains(ioopm_list_t *list, ioopm_elem_t element) {
	ioopm_link_t *current_link = list->first;
	for(int i = 0; i < list->size; i++) {
		if(list->cmp(current_link->value,element)){
			return true;
		}
		current_link = current_link->next;
	}
	return false;
}

int ioopm_linked_list_size(ioopm_list_t *list) {
	return list->size;
}

bool ioopm_linked_list_is_empty(ioopm_list_t *list) {
	return list->size == 0;
}


void ioopm_linked_list_clear(ioopm_list_t *list) {
	list->first = NULL;
	list->last = NULL;
	list->size = 0;
}

bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_list_predicate *prop, void *extra) {
	ioopm_link_t *current_link = list->first;
	for(int i = 0; i < list->size; i++) {
		if(!prop(list->cmp,current_link->value,extra)) {
			return false;
		}
		current_link = current_link->next;
	}
	return true;
}

bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_list_predicate *prop, void *extra) {
	ioopm_link_t *current_link = list->first;
	for(int i = 0; i < list->size; i++) {
		if(prop(list->cmp,(ioopm_elem_t)current_link->value,extra)) {
			return true;
		}
		current_link = current_link->next;
	}
	return false;
}

void ioopm_linked_list_apply_to_all(ioopm_list_t *list, ioopm_list_apply_function *fun, void *extra) {
	ioopm_link_t *current_link = list->first;
	for(int i = 0; i < list->size; i++) {
		fun(&current_link->value,extra);
		current_link = current_link->next;
	}
}
