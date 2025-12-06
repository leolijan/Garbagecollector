This is a program that simulates a simple store where you can add/remove merchandise, replenish its stock and purchase merchandise by adding it to different carts.

The program uses the stor_functions.h functions were the main functionality of the store lies.

Each merchandise has a name, a description and a price.
Each merchandise is also places on a number of "shelfes".
A shelf is a string that starts with a letter and then ends with more than two numbers together.

A cart holds different merchandise and different amounts of these merchandises.

We store all merchandise and carts inside our database structure. All merchandise and all carts are implemented as hashtables where the name of the merchandise is the key to the hashtable and the number of the cart is the key for the hashtable that holds all carts.

The functions inside utils.h is used to ask questions and take user input.

Compile the program by writing make or make store with the makefile
Compile the tests by writing make store_tests
