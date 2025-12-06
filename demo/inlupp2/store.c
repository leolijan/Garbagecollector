#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include <ctype.h>
#include <stdbool.h>

#include "hash_table-dynamic.h"
#include "linked_list.h"
#include "stor_functions.h"
#include "../../src/gc.h"

void print_menu() {
	printf("[A] ADD A MERCHANDISE\n"
		   "[D] DELETE A MERCHANDISE\n"
		   "[L] LIST ALL MERCHANDISE\n"
		   "[E] EDIT A MERCHANDISE\n"
		   "[S] SHOW STOCK\n"
		   "[P] REPLENISH\n"
		   "[C] CREATE CART\n"
		   "[Y] REMOVE CART\n"
		   "[+] ADD TO CART\n"
		   "[-] REMOVE FROM CART\n"
		   "[=] CALCULATE COST\n"
		   "[O] CHECKOUT\n"
		   "[U] UNDO\n"
		   "[Q] QUIT\n");
}

bool confirmation() {
	char buf[255];
	int buf_siz = 255;

	printf("Are you sure? Yes [y], No [n]\n");
	read_string(buf,buf_siz);
	if(strcmp("y",buf) == 0) {
		return true;
	}
	return false;
}


bool is_choice(char *buf) {
	char *choices = "adelspcy+-=ouq";
	if(strlen(buf) > 1){
		return false;
	}else{
		for(size_t i = 0; i < strlen(choices); i++){
			if(choices[i] == *buf){
				return true;
			}
		}
	}
	return false;
}

char *ask_question_menu(heap_t *heap) {
	char buf[255];
	int buf_siz = 255;
	do {
		print_menu();
		read_string(buf,buf_siz);
		if (buf[0] == '\0') {
			return NULL;
		}
		*buf = tolower(*buf);
	}while(!is_choice(buf));
	return h_strdup(buf, heap);
}

void event_loop(database_t *db, heap_t *heap) {
	char *choice;

	ioopm_hash_table_t *all_merch = db->all_merch;
	ioopm_hash_table_t *all_carts = db->all_carts;
	bool continue_loop = true;

	do {
    
		choice = ask_question_menu(heap);
		if(choice == NULL) {
			break;
		}
		if(*choice == 'a') {
			char *name = ask_question_string("Write the name of your merchandise: ");
			char *description = ask_question_string("Write the description of your merchandise: ");
			int price = ask_question_int("Write the price of your merchandise (SKR): ");
			add_merchandise(name, description, price,all_merch, heap);
		}
		else if(*choice == 'd') {
			char *merch_name = ask_question_string("Write the name of the merchandise you want to remove: ");
			if(confirmation()) {
				remove_merchandise(all_carts,all_merch,merch_name, heap);
			}
		}
		else if(*choice == 'e') {
				char *name = ask_question_string("What merchandise do you want to edit?");
			char *new_name = ask_question_string("Write the name of your merchandise: ");
			char *description = ask_question_string("Write the description of your merchandise: ");
			int price = ask_question_int("Write the price of your merchandise (SKR): ");
			
			edit_merchandise(all_merch, all_carts, name, new_name,description,price, heap);
		}
		else if(*choice == 'l') {
			list_merchandise(all_merch, heap);
		}
		else if(*choice == 's') {
			char *merch_name = ask_question_string("Write the name of the merchandise you want to show: ");
			show_stock(all_merch,merch_name, heap);
		}
		else if(*choice == 'p') {
			char *merch_name = ask_question_string("Write the name of the merchandise you want to replenish: ");
			char *shelf_name = ask_question_string("Write what shelf you want to put it on: ");
			int num_of_merch = ask_question_int("Write how many of the merchandise should be replenished: ");
			replenish(all_merch,merch_name,shelf_name,num_of_merch, heap);
		}
		else if(*choice == 'c') {
			create_cart(all_carts, ++db->created_carts, heap);
		}
		else if(*choice == 'y') {

			int cart_key = ask_question_int("Write what cart you want to remove: ");
			if(confirmation()) {
				remove_cart(cart_key,all_carts, heap);
			}
		}
		else if(*choice == '+') {
			char *merch_name = ask_question_string("Write what merch should be added: ");
			int cart_key = ask_question_int("Write what cart you are using: ");
			int quantity = ask_question_int("Write how many of the merch you want to take:");
			add_to_cart(all_merch,all_carts,cart_key,merch_name,quantity, heap);
		}
		else if(*choice == '-') {
			char *merch_name = ask_question_string("Write what merch should be removed: ");
			int cart_key = ask_question_int("Write what cart you are using: ");
			int quantity = ask_question_int("Write how many of the merch you want to remove:");
			remove_from_cart(all_carts,cart_key,merch_name,quantity, heap);
		}
		else if(*choice == '=') {
			int cart_key = ask_question_int("Write what cart you are using: ");
			calculate_cost(all_carts,cart_key, heap);
		}
		else if(*choice == 'o') {
			int cart_key = ask_question_int("Write what cart you are checking out: ");
			checkout(all_carts,cart_key, heap);
		}
		else if(*choice == 'u') {
			//undo();
			printf("not yet implemented\n");
		}
		else if(*choice == 'q') {
			continue_loop = !confirmation();
		}

	}while(continue_loop);
  
}


int main(int argc, char *argv[]) {
	size_t bytes = 2048 * 4;
	bool unsafe_stack = false;
	float gc_threshold = 0.8;
	heap_t *heap = h_init(bytes, unsafe_stack, gc_threshold);
	database_t *db = create_database(heap);
	event_loop(db, heap);
}
