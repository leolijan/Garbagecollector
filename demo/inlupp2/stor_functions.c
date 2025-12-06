#include "stdlib.h"
#include "ctype.h"
#include "stdbool.h"
#include "string.h"

#include "stor_functions.h"
#include "hash_table-dynamic.h"
#include "linked_list.h"
#include "iterator.h"

#include "common.h"
#include "utils.h"
#include "../../src/gc.h"

//Help function
static bool is_shelf(char *shelf) {
	//first should be a char
	if(isalpha(shelf[0])){
		if(strlen(shelf)<3) {
			return false;
		}
		for(int i = 2; i < strlen(shelf); i++) {
			if(!isdigit(shelf[i])) {
				return false;
			}
		}
	}else {
		return false;
	}
	return true;	
}

static char **create_array_from_list(ioopm_list_t *list, heap_t *h) {
	const size_t keys_size = list->size * sizeof(char*); /// Modified
	char **keys = h_alloc_raw(h, keys_size); /// Modified
	ioopm_list_iterator_t *it = ioopm_list_iterator(list, h);

	for(int i = 0; i < list->size; i++) {
		keys[i] = ioopm_iterator_current(it).string;
		ioopm_iterator_next(it);
	}
	return keys;
}

static int cmpstringp(const void *p1, const void *p2) {
	return strcmp(*(char *const *)p1, *(char *const *)p2);
}

static void sort_keys(char *keys[], size_t no_keys) {
	qsort(keys, no_keys, sizeof(char *), cmpstringp);
}

static bool compare_shelfs(ioopm_elem_t s1, ioopm_elem_t s2) {
	shelf_pair_t *shelf1 = s1.any;
	shelf_pair_t *shelf2 = s2.any;
	return strcmp(shelf1->shelf_name,shelf2->shelf_name) == 0;
}

merch_t *make_merch(char *name, char *description, int price, heap_t *h) {
	char *merch_fs = "2ci*"; /// Modified
	merch_t *new_merch = h_alloc_struct(h, merch_fs); /// Modified
	new_merch->name = name;
	new_merch->description = description;
	new_merch->price = price;
	new_merch->shelfs = ioopm_linked_list_create(compare_shelfs, h);
	return new_merch;  
}

merch_t *input_merch(heap_t *h) {
	char *name = ask_question_string("Write the name of your merchandise: ");
	char *description = ask_question_string("Write the description of your merchandise: ");
	int price = ask_question_int("Write the price of your merchandise (SKR): ");
	return make_merch(name,description,price, h);
}

void add_merchandise(char *name, char *desciption,int price, ioopm_hash_table_t *all_merch, heap_t *h) {
	if(ioopm_hash_table_lookup(all_merch,(ioopm_elem_t)name).string == NULL) {

		void *v = make_merch(name,desciption, price, h);
		ioopm_hash_table_insert(all_merch,(ioopm_elem_t)name,(ioopm_elem_t)v, h);
	} else {
		printf("That merchandise does already exist\n");
	}
}

void remove_from_cart(ioopm_hash_table_t *all_carts, int cart_key, char *merch_name, int quantity, heap_t *h);
static int num_for_shelfs(ioopm_list_t *shelfs, heap_t *h);


static void remove_merchandise_all_carts(ioopm_hash_table_t *all_carts, char *merch_name, int quantity, heap_t *h) {
  	ioopm_list_t *all_cart_keys = ioopm_hash_table_keys(all_carts, h);
	ioopm_list_iterator_t *it_carts = ioopm_list_iterator(all_cart_keys, h);
  	for(int i = 0; i < all_cart_keys->size; i++) {
		ioopm_elem_t v = ioopm_iterator_current(it_carts);
		int cart_key = v.number;
		remove_from_cart(all_carts,cart_key,merch_name,quantity, h);
		ioopm_iterator_next(it_carts);
	}

}

void remove_merchandise(ioopm_hash_table_t *all_carts, ioopm_hash_table_t *all_merch, char *name, heap_t *h) {
	ioopm_elem_t lookup = ioopm_hash_table_lookup(all_merch,(ioopm_elem_t)name);
	if(lookup.string != NULL) {
		merch_t *merch = lookup.any;

		//remove merch from carts...
		remove_merchandise_all_carts(all_carts,name,num_for_shelfs(merch->shelfs, h),h);
		
		ioopm_hash_table_remove(all_merch,(ioopm_elem_t)name);
	} else {
		printf("That merchandise does not exist\n");
	}
}

static shelf_pair_t *create_shelf_pair(char *shelf_name, int num_of_merch, heap_t *h);

static ioopm_list_t *copy_shelfs(ioopm_list_t *old_shelfs, heap_t *h) {
	ioopm_list_t *new_shelfs = ioopm_linked_list_create(compare_string, h);
	ioopm_list_iterator_t *it_shelfs = ioopm_list_iterator(old_shelfs, h);

	for(int i = 0; i < old_shelfs->size; i++) {
		ioopm_elem_t current = ioopm_iterator_current(it_shelfs);
		shelf_pair_t *shelf_pair = current.any;

		int num_of_merch = shelf_pair->num_of_merch;
		/// former strdup
		/// char *shelf_name = strdup(shelf_pair->shelf_name);
		/// new stdup function for heap
		char *shelf_name = h_strdup(shelf_pair->shelf_name, h);
		

		shelf_pair_t *new_pair = create_shelf_pair(shelf_name,num_of_merch, h);

		ioopm_linked_list_prepend(new_shelfs,(ioopm_elem_t){.any = new_pair}, h);

		ioopm_iterator_next(it_shelfs);
	}
	return new_shelfs;
}

void edit_merchandise(ioopm_hash_table_t *all_merch, ioopm_hash_table_t *all_carts, char *name, char *new_name, char *description, int price, heap_t *h) {
	
	ioopm_elem_t result = ioopm_hash_table_lookup(all_merch,(ioopm_elem_t)name);
	if(result.string == NULL) {
		printf("That merchandise does not exist\n");
	} else {
		merch_t *merch = (merch_t*)result.any;
		printf("EDIT MERCH:");
		printf("Name: %s\n", merch->name);
		printf("Description: %s\n", merch->description);
		printf("Price: %d\n", merch->price);


		ioopm_list_t *new_shelfs = copy_shelfs(merch->shelfs, h);
		
		remove_merchandise(all_carts,all_merch,name, h);		
		
		add_merchandise(new_name, description, price, all_merch, h);		
		//find the new merchandise
		merch_t *new_merch = ioopm_hash_table_lookup(all_merch,(ioopm_elem_t)new_name).any;

		new_merch->shelfs = new_shelfs;

	}
}


static bool confirmation() {
	char buf[255];
	int buf_siz = 255;

	printf("Do you want to continue? Yes [y], No [n]\n");
	read_string(buf,buf_siz);
	if(strcmp("y",buf) == 0) {
		return true;
	}
	return false;
}

void list_merchandise(ioopm_hash_table_t *all_merch, heap_t *h) {
	ioopm_list_t *merchandise  = ioopm_hash_table_keys(all_merch, h);
	size_t size = ioopm_hash_table_size(all_merch);
	char **merch_array = create_array_from_list(merchandise, h);
	sort_keys(merch_array,size);
	int count_merch = 0;

	for (size_t i = 0; i < size; ++i) {
		printf("%s:\n", merch_array[i]);
		count_merch++;
		if(count_merch == 20) {
			if(!confirmation()) {
				break;
			}
			count_merch = 0;
		}
	}
}


//return -1 if shelf is not found
static int index_for_shelf_pair(ioopm_list_t *shelfs, char *compare_shelf_name, heap_t *h) {
	ioopm_list_iterator_t *it_shelfs = ioopm_list_iterator(shelfs, h);
  
	for(int i = 0; i < shelfs->size; i++) {
		ioopm_elem_t current = ioopm_iterator_current(it_shelfs);
		shelf_pair_t *shelf_pair = current.any;
		if(strcmp(shelf_pair->shelf_name,compare_shelf_name) == 0) {
			return i;
		}
		ioopm_iterator_next(it_shelfs);
	}
	return -1; //Shelf was not found
}

static int num_for_shelfs(ioopm_list_t *shelfs, heap_t *h) {
	ioopm_list_iterator_t *it_shelfs = ioopm_list_iterator(shelfs, h);
	int num = 0;
	for(int i = 0; i < shelfs->size; i++) {
		ioopm_elem_t current = ioopm_iterator_current(it_shelfs);
		shelf_pair_t *shelf_pair = current.any;
		num += shelf_pair->num_of_merch;
		ioopm_iterator_next(it_shelfs);
	}
	return num; 
}

static shelf_pair_t *create_shelf_pair(char *shelf_name, int num_of_merch, heap_t *h) {
	const size_t pair_size = sizeof(char *) * sizeof(int); /// Modified
	shelf_pair_t *new_pair = h_alloc_raw(h, pair_size); /// Modified
	new_pair->shelf_name = shelf_name;
	new_pair->num_of_merch = num_of_merch;
	return new_pair;
}

void replenish(ioopm_hash_table_t *all_merch, char *name, char *shelf, int merch_input, heap_t *h) {
	ioopm_elem_t result = ioopm_hash_table_lookup(all_merch,(ioopm_elem_t)name);
	if(result.string != NULL){
		if(is_shelf(shelf)){
			merch_t *merch = result.any;
			ioopm_list_t *shelfs = merch->shelfs;
			int index = index_for_shelf_pair(shelfs,shelf, h);
			if(index < 0) {
				shelf_pair_t *new_pair = create_shelf_pair(shelf,merch_input, h);
	
				ioopm_linked_list_prepend(merch->shelfs,(ioopm_elem_t){.any = new_pair}, h); 
	
			} else {
				ioopm_elem_t v = ioopm_linked_list_remove(merch->shelfs,index);
				shelf_pair_t *pair = v.any;
				int num = pair->num_of_merch + merch_input;
				ioopm_linked_list_append(merch->shelfs,(ioopm_elem_t){.any = create_shelf_pair(pair->shelf_name,num, h)}, h);
			}			
		} else {
			printf("Not a valid shelf!");
		}
	} else {
		printf("No such merchandise exists\n");
	}
}

void show_stock(ioopm_hash_table_t *all_merch, char *name, heap_t *h) {
	ioopm_elem_t merch_elem = ioopm_hash_table_lookup(all_merch,(ioopm_elem_t)name);
	if(merch_elem.string != NULL) {
		merch_t *merch = merch_elem.any;
		ioopm_list_t *shelfs = merch->shelfs;
		ioopm_list_iterator_t *it = ioopm_list_iterator(shelfs, h);

		for(int i = 0; i < shelfs->size; i++) {
			ioopm_elem_t current = ioopm_iterator_current(it);
			shelf_pair_t *shelf_pair = current.any;
			printf("Shelf: %s. Amount: %d\n", shelf_pair->shelf_name, shelf_pair->num_of_merch);      
			ioopm_iterator_next(it);
		}
	} else {
		printf("That merchandise does not exist!\n");
	}
}

void create_cart(ioopm_hash_table_t *all_carts, int cart_key, heap_t *h) {
	ioopm_hash_table_t *cart = ioopm_hash_table_create(hash_string_sum, compare_string, compare_int, h);
	ioopm_hash_table_insert(all_carts, (ioopm_elem_t)cart_key, (ioopm_elem_t){.any = cart}, h);
	printf("CREATING CART %d\n", cart_key);
}

static cart_pair_t *create_cart_pair(merch_t *merch, int amount_in_cart, heap_t *h) {
	char *cart_fs = "*i"; /// Modified
	cart_pair_t *pair = h_alloc_struct(h, cart_fs); /// Modified
	pair->merch = merch;
	pair->amount_in_cart = amount_in_cart;
	return pair;
}

static int num_of_merch_in_one_cart(ioopm_hash_table_t *cart, heap_t *h) {
	ioopm_list_t *all_cart_pairs = ioopm_hash_table_values(cart, h);
	ioopm_list_iterator_t *it_carts = ioopm_list_iterator(all_cart_pairs, h);

	int counter = 0;

	for(int i = 0; i < all_cart_pairs->size; i++) {
		ioopm_elem_t v = ioopm_iterator_current(it_carts);
		cart_pair_t *current_pair = v.any;
		counter += current_pair->amount_in_cart;
		printf("%d\n",current_pair->amount_in_cart);
		ioopm_iterator_next(it_carts);
	}

	return counter;
	
}

static int num_of_merch_in_carts(ioopm_hash_table_t *all_carts, heap_t *h) {
	ioopm_list_t *carts = ioopm_hash_table_values(all_carts, h);
	ioopm_list_iterator_t *it_carts = ioopm_list_iterator(carts, h);

	int total_num_of_merch = 0;
	
	for(int i = 0; i < carts->size; i++) {
		ioopm_elem_t v = ioopm_iterator_current(it_carts);
		ioopm_hash_table_t *current_cart = v.any;
		total_num_of_merch += num_of_merch_in_one_cart(current_cart, h);
		ioopm_iterator_next(it_carts);
	}
	
	return total_num_of_merch;
}

void add_to_cart(ioopm_hash_table_t *all_merch,ioopm_hash_table_t *all_carts, int cart_key, char *merch_name, int quantity, heap_t *h) {
  
	ioopm_elem_t lookup_merch = ioopm_hash_table_lookup(all_merch,(ioopm_elem_t)merch_name);
	if(lookup_merch.string == NULL) {
		printf("That merchandise does not exist\n");
		return;
	}

	merch_t *merch = lookup_merch.any;
	ioopm_elem_t lookup_cart = ioopm_hash_table_lookup(all_carts, (ioopm_elem_t)cart_key);

  
	if(lookup_cart.string != NULL){      
		ioopm_hash_table_t *cart = lookup_cart.any;
		ioopm_elem_t lookup_cart_pair = ioopm_hash_table_lookup(cart, (ioopm_elem_t)merch_name);
		int merch_already_in_cart = 0;

		if(quantity + num_of_merch_in_carts(all_carts, h) <= num_for_shelfs(merch->shelfs, h)) {
			
			if(lookup_cart_pair.string != NULL) {
				printf("Adding to already existing cart pair\n");
				cart_pair_t *existing_cart_pair = lookup_cart_pair.any;	
				merch_already_in_cart += existing_cart_pair->amount_in_cart;

				//Remove existing
				ioopm_hash_table_remove(cart,(ioopm_elem_t)merch_name);
			}

			int num_for_merch = num_for_shelfs(merch->shelfs, h);
		
			if(quantity+merch_already_in_cart <= num_for_merch) {      
				cart_pair_t *pair = create_cart_pair(merch, quantity + merch_already_in_cart, h); //change
				void *v = pair;
				ioopm_hash_table_insert(cart, (ioopm_elem_t)merch->name, (ioopm_elem_t)v, h);      
			} else {
				printf("Not enough merch in stock! There are only %d amount.\n",num_for_merch);
			}

		} else {
			printf("Not enough merch left in stock!\n");
		}

	} else {
		printf("That cart does not exist\n");
	}
}

void remove_from_cart(ioopm_hash_table_t *all_carts, int cart_key, char *merch_name, int quantity, heap_t *h) {

	ioopm_elem_t lookup_cart = ioopm_hash_table_lookup(all_carts,(ioopm_elem_t)cart_key);
  
	if(lookup_cart.string != NULL) {
		ioopm_hash_table_t *cart = lookup_cart.any;
		//check if the merchandise exists
		ioopm_elem_t lookup_cart_pair = ioopm_hash_table_lookup(cart, (ioopm_elem_t)merch_name);
    
		if(lookup_cart_pair.string != NULL) {
			cart_pair_t *cart_pair = lookup_cart_pair.any;
			int merch_after_remove = cart_pair->amount_in_cart - quantity;

			if(merch_after_remove <= 0) {
				printf("REMOVE THIS\n");
				ioopm_hash_table_remove(cart,(ioopm_elem_t)merch_name);

			} else {
				cart_pair_t *new_cart_pair = create_cart_pair(cart_pair->merch,merch_after_remove, h);
				ioopm_hash_table_insert(cart,(ioopm_elem_t)merch_name,(ioopm_elem_t){.any = new_cart_pair}, h);

			}      
		} else {
			printf("That merchandise does not exist in the cart.\n");
		}    
	} else {
		printf("That cart does not exist.\n");
	}  
}


void remove_cart(int cart_key,ioopm_hash_table_t *all_carts, heap_t *h) {
  
	ioopm_elem_t lookup_cart = ioopm_hash_table_lookup(all_carts, (ioopm_elem_t)cart_key);
  
	if(lookup_cart.string != NULL) {
		ioopm_hash_table_remove(all_carts,(ioopm_elem_t)cart_key);
	} else {
		printf("That cart does not exist!\n");
	}
}

int calculate_cost(ioopm_hash_table_t *all_carts, int cart_key, heap_t *h) {
	ioopm_elem_t lookup_cart = ioopm_hash_table_lookup(all_carts, (ioopm_elem_t)cart_key);

	if(lookup_cart.string != NULL) {
		ioopm_hash_table_t *cart = lookup_cart.any;
		ioopm_list_t *cart_pairs = ioopm_hash_table_values(cart,h);
		ioopm_list_iterator_t *it_cart_pairs = ioopm_list_iterator(cart_pairs,h);

		int total_cost = 0;

		for(int i = 0; i < cart_pairs->size; i++) {
			ioopm_elem_t current = ioopm_iterator_current(it_cart_pairs);
			cart_pair_t *cart_pair = current.any;
			int amount = cart_pair->amount_in_cart;
			int price = cart_pair->merch->price;
			total_cost += amount*price;
		}

		printf("The total price in cart %d is %d.\n",cart_key,total_cost);
		return total_cost;
    
	} else {
		printf("That cart does not exist!\n");
		return -1;
	}
}

static void decrease_stock(merch_t *merch, int amount_to_decrease, heap_t *h) {
	
	ioopm_list_t *shelfs = merch->shelfs;
	ioopm_list_iterator_t *it_shelfs = ioopm_list_iterator(shelfs,h);

	//when there is no more shelfs...
  
	while (amount_to_decrease > 0) {
		ioopm_elem_t current = ioopm_iterator_current(it_shelfs);
		shelf_pair_t *shelf_pair = current.any;

		int shelf_stock = shelf_pair->num_of_merch;
	
		printf("Shelf stock: %d\n",shelf_stock);
		printf("amount to decrease %d\n",amount_to_decrease);

		//does not check if num of merch
		//0 < 10 exempel

		if (shelf_stock < amount_to_decrease) {
			amount_to_decrease -= shelf_stock;
			shelf_pair->num_of_merch = 0;
		} else {
			shelf_pair->num_of_merch -= amount_to_decrease;
			amount_to_decrease = 0;     
		}
		ioopm_iterator_next(it_shelfs);
	}
}

void checkout(ioopm_hash_table_t *all_carts, int cart_key, heap_t *h) {

	ioopm_elem_t lookup_cart = ioopm_hash_table_lookup(all_carts,(ioopm_elem_t)cart_key);
	if(lookup_cart.string != NULL) {
		ioopm_hash_table_t *cart = lookup_cart.any;
		ioopm_list_t *cart_pairs = ioopm_hash_table_values(cart,h);
		ioopm_list_iterator_t *it_cart_pairs = ioopm_list_iterator(cart_pairs,h);
		//Decrease stock!
		for(int i = 0; i < cart_pairs->size; i++) {
			ioopm_elem_t current = ioopm_iterator_current(it_cart_pairs);
			cart_pair_t *cart_pair = current.any;
			decrease_stock(cart_pair->merch,cart_pair->amount_in_cart, h);
			ioopm_iterator_next(it_cart_pairs);      
		}
    
		//remove shooping cart
		remove_cart(cart_key,all_carts, h);
    
	} else {
		printf("That cart does not exist!\n");
	}
}

database_t *create_database(heap_t *h) {
	char *database_fs = "2*i";
	database_t *new_database = h_alloc_struct(h, database_fs);
	new_database->all_merch = ioopm_hash_table_create(hash_string_sum,compare_string,compare_int,h);
	new_database->all_carts = ioopm_hash_table_create(hash_int,compare_int,compare_int,h);
	new_database->created_carts = 0;
	return new_database;
}


void destroy_database(database_t *db) {
	db = NULL;
}
