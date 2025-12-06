#pragma once

#ifndef STOR_FUNCTIONS_H
#define STOR_FUNCTIONS_H

#include "stdlib.h"
#include "ctype.h"
#include "stdbool.h"
#include "string.h"

#include "hash_table-dynamic.h"
#include "linked_list.h"
#include "iterator.h"
#include "../../src/gc.h"

typedef struct merch merch_t;
typedef struct shelf_pair shelf_pair_t;
typedef struct cart_pair cart_pair_t;
typedef struct database database_t;

struct shelf_pair {
	char *shelf_name;
	int num_of_merch;
};

struct merch {
	char *name;
	char *description;
	int price;
	ioopm_list_t *shelfs;
};


//Value pairs inside every cart holding the merchandise
struct cart_pair {
	merch_t *merch;
	int amount_in_cart;
};

//all_merch uses the merch name as the key (string)
//all_carts uses the the cart key from created_carts variable (integer)
struct database {
	ioopm_hash_table_t *all_merch;
	ioopm_hash_table_t *all_carts;
	int created_carts;
};

// @brief Function to create a new database structure
// @return the new database that can hold merch and carts
database_t *create_database(heap_t *h);

// @brief prints questions to the user to form a new merch struct
// @return returns a merch based on the user input 
merch_t *input_merch();

// @brief Create a new merch struct from the given parameters
// @param name the name for the merchandise
// @param description the description of the merchandise
// @param price the price of the merchandise
// @param extra pointer for other appropriate values to be used
merch_t *make_merch(char *name, char *description, int price, heap_t *h);

// @brief Add a merchandise to the database
// @param name the name for the merchandise
// @param description the description of the merchandise
// @param price the price of the merchandise
// @param all_merch hash table containing all merchandise
void add_merchandise(char *name, char *description, int price, ioopm_hash_table_t *all_merch, heap_t *h);

// @brief Completely remove a merchandise from the store, including all stock
// @param all_merch hash table containing all merchandise
// @param name the name of the merchandise to be removed
void remove_merchandise(ioopm_hash_table_t *all_carts, ioopm_hash_table_t *all_merch, char *name, heap_t *h);

// @brief Change name/Description/price of a merch
// @param all_merch hash table containing all merchandise
// @param all_carts hash table containing all carts
// @param new_name the edited name for the merch
// @param description edited decription for the merch
// @param price the edited price for the merch
void edit_merchandise(ioopm_hash_table_t *all_merch, ioopm_hash_table_t *all_carts, char *name, char *new_name, char *description, int price, heap_t *h);

// @brief List all items in the store printed in alphabetical order, maximum 20 items at a time
// @param all_merch hash table containing all merchandise
void list_merchandise(ioopm_hash_table_t *all_merch, heap_t *h);


// @brief Show the stock of a merchandise and the quantity at each shelf
// @param all_merch hash table containing all merchandise
// @param name the name of the merchandise
void show_stock(ioopm_hash_table_t *all_merch, char *name, heap_t *h);

//increase stock

// @brief Replenish stock of a merchandise at a specific shelf
// @param db the database
// @param name the name of the merchandise
// @param num_of_merch the number of merch to add to the shelf
void replenish(ioopm_hash_table_t *db, char *name, char *shelf, int num_of_merch, heap_t *h);

// @brief Create a new cart 
// @param all_carts a hash table containing all carts
// @param cart_key the key in the hash table to the new cart
void create_cart(ioopm_hash_table_t *all_carts, int cart_key, heap_t *h);

// @brief Remove a cart and free its memory
// @param cart_key the key in the hash table to the cart
// @param all_carts a hash table containing all carts
void remove_cart(int cart_key, ioopm_hash_table_t *all_carts, heap_t *h);

// @brief Add merchandise to the cart
// @param all_merch a hash_table containign all merchandise
// @param all_carts a hash table containing all carts
// @param cart_key the key in the hash table to the cart
// @param merch_name the name of the merch to be added to the cart
// @param quantity the number of merch to be added
void add_to_cart(ioopm_hash_table_t *all_merch, ioopm_hash_table_t *all_cart, int cart_key, char *merch_name, int quantity, heap_t *h);

// @brief Remove merchandise from the cart
// @param all_carts a hash table containing all carts
// @param merch_name the name of the merch to be added to the cart
// @param quantity the number of merch to be added
void remove_from_cart(ioopm_hash_table_t *all_carts, int cart_key, char *merch_name, int quantity, heap_t *h);

// @brief Calculate the total cost for the cart
// @param all_carts a hash table containing all carts
// @param cart_key the key in the hash table to the cart
int calculate_cost(ioopm_hash_table_t *all_carts, int cart_key, heap_t *h);

// @brief Checkout the cart and remove the merchandise from the stock then remove the cart
// @param all_carts a hash table containing all carts
// @param cart_key the key in the hash table to the cart
void checkout(ioopm_hash_table_t *all_carts, int cart_key, heap_t *h);

//TODO
int undo();

#endif
