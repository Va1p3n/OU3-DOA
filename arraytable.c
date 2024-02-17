#include <stdlib.h>
#include <stdio.h>

#include <array_1d.h>
#include <table.h>

#define MAXSIZE 80000

/**
 * Implementation of the datatype table using an 1d array.
 * Duplicates is handled by overwriting
 * 
 * Author: Felix Vallström (c23fvm)
 * 
 * Hand in date: 2024-02-16
 * 
 * Version:
 *   2024-02-XX: v1.0. First hand in
*/

// Table type.
typedef struct table {
	array_1d *entries;
	compare_function *key_cmp_func;
	free_function key_free_func;
	free_function value_free_func;
	int amount_entries_set;
} table;

// Table entry type.
typedef struct table_entry
{
	void *key;
	void *value;
} table_entry;

/**
 * search_for_key() - Searches for the index where a key is located in the array_1d.
 * @t: A pointer to the table to search
 * @key: The key to search for
 * 
 * Returns: A index (integer) to where the key is located in the array_1d.
 * 			NULL if the key isn't in the array.
*/
int search_for_key(const table *t, const void *key)
{
	for (int index = array_1d_low(t->entries); index < (array_1d_low(t->entries) + t->amount_entries_set); index++)
	{
		table_entry *entry = array_1d_inspect_value(t->entries, index);

		if (t->key_cmp_func(entry->key, key) == 0)
		{
			return index;
		}
	}

	return -1;
}

/**
 * table_empty() - Create an empty table.
 * @key_cmp_func: A pointer to a function to be used to compare keys. See
 *                util.h for the definition of compare_function.
 * @key_free_func: A pointer to a function (or NULL) to be called to
 *		   de-allocate memory for keys on remove/kill.
 * @value_free_func: A pointer to a function (or NULL) to be called to
 *		     de-allocate memory for values on remove/kill.
 *
 * Returns: Pointer to a new table.
 */
table *table_empty(compare_function key_cmp_func, free_function key_free_func, free_function value_free_func) 
{
	// Allocating the head of the table
	table *t = malloc(sizeof(table));

	// Create the array to store elements
	t->entries = array_1d_create(0, MAXSIZE, NULL);

	// Store free functions
	t->key_cmp_func = key_cmp_func;
	t->key_free_func = key_free_func;
	t->value_free_func = value_free_func;

	// Sets the amount of set values for the array to 0
	t->amount_entries_set = array_1d_low(t->entries);

	return t;
}

/**
 * table_is_empty() - Check if a table is empty.
 * @t: Table to check.
 *
 * Returns: True if table contains no key/value pairs, false otherwise.
 */
bool table_is_empty(const table *t)
{
	// Entries set is 0 if the table does not have any entries
	return t->amount_entries_set == 0;	
}

/**
 * table_insert() - Add a key/value pair to a table.
 * @t: Table to manipulate.
 * @key: A pointer to the key value.
 * @value: A pointer to the value value.
 *
 * Insert the key/value pair into the table. No test is performed to
 * check if key is a duplicate. table_lookup() will return the latest
 * added value for a duplicate key. table_remove() will remove all
 * duplicates for a given key.
 *
 * Returns: Nothing.
 */
void table_insert(table *t, void *key, void *value)
{
	// Look and store if the key is in the table and if so, where
	int search_result = search_for_key(t, key);

	// If not -1, the key is already in the table and we overwrite it
	if (search_result != -1)
	{
		// Extract the entry from the array 
		table_entry *found_entry = array_1d_inspect_value(t->entries, search_result);

		// Free key/value
		t->key_free_func(found_entry->key);
		t->value_free_func(found_entry->value);

		// Update key/value
		found_entry->key = key;
		found_entry->value = value;

		// Insert the key/value into the table
		array_1d_set_value(t->entries, found_entry, search_result);

		// Exits the function
		return;
	}

	// Allocates memory for the new entry
	table_entry *entry = malloc(sizeof(table_entry));

	// Store the key/value pointers in the entry
	entry->key = key;
	entry->value = value;

	// Insert the new entry to the table
	array_1d_set_value(t->entries, entry, t->amount_entries_set);

	// Increase the amount of entries is stored in the array
	t->amount_entries_set += 1;
}

/**
 * table_lookup() - Look up a given key in a table.
 * @t: Table to inspect.
 * @key: Key to look up.
 *
 * Returns: The value corresponding to a given key, or NULL if the key
 * is not found in the table.
 */
void *table_lookup(const table *t, const void *key)
{
	// Store the location/index of the table_entry in the array
	int search_result = search_for_key(t, key);

	// Checks what index was stored, if NULL the key isn't in the table
	if (search_result != -1)
	{
		table_entry *looked_entry = array_1d_inspect_value(t->entries, search_result);

		// Return the entry searched for
		return looked_entry->value;
	}
	
	// The element wasn't in the table
	return NULL;
}

/**
 * table_choose_key() - Return an arbitrary key.
 * @t: Table to inspect.
 *
 * Return an arbitrary key stored in the table. Can be used together
 * with table_remove() to deconstruct the table if no duplicates have
 * been stored in the table. Undefined for an empty table.
 *
 * Returns: An arbitrary key stored in the table.
 */
void *table_choose_key(const table *t)
{
	// Returns the first key in the table
	table_entry *entry = array_1d_inspect_value(t->entries, array_1d_low(t->entries));

	return entry->key;
}

/**
 * table_remove() - Remove a key/value pair in the table.
 * @t: Table to manipulate.
 * @key: Key for which to remove pair.
 *
 * Any matching duplicates will be removed. Will call any free
 * functions set for keys/values. Does nothing if key is not found in
 * the table.
 *
 * Returns: Nothing.
 */
void table_remove(table *t, const void *key)
{
	// Store the location/index of the table_entry in the array
	int search_result = search_for_key(t, key);

	// The key wasn't in the list
	if (search_result == -1)
		return;

	// Get the table_entry to remove from the table
	table_entry *remove_entry = array_1d_inspect_value(t->entries, search_result); 

	// Free key, value and element head.
	t->key_free_func(remove_entry->key);
	t->value_free_func(remove_entry->value);
	free(remove_entry);

	// We will move the last table_entry in the array to the spot where we removed the array
	table_entry *last_entry = array_1d_inspect_value(t->entries, (t->amount_entries_set - 1));
	array_1d_set_value(t->entries, last_entry, search_result);

	// Decrease amount of table_entries stored
	t->amount_entries_set = t->amount_entries_set - 1;
}

/**
 * table_kill() - Destroy a table.
 * @t: Table to destroy.
 *
 * Return all dynamic memory used by the table and its elements. If a
 * free_func was registered for keys and/or values at table creation,
 * it is called each element to free any user-allocated memory
 * occupied by the element values.
 *
 * Returns: Nothing.
 */
void table_kill(table *t)
{
	// Removes all entries
	while (!table_is_empty(t))
	{
		table_remove(t, table_choose_key(t));	
	}
	// kill the array
	array_1d_kill(t->entries);
	// Free the array...
	//free(t->entries);
	// ...and the table
	free(t);
}

/**
 * table_print() - Print the given table.
 * @t: Table to print.
 * @print_func: Function called for each key/value pair in the table.
 *
 * Iterates over the key/value pairs in the table and prints them.
 * Will print all stored elements, including duplicates.
 *
 * Returns: Nothing.
 */
void table_print(const table *t, inspect_callback_pair print_func)
{
	// Iterates over all the entries and calls print_func on keys/values
	for (int index = 0; index < t->amount_entries_set; index++)
	{
		table_entry *entry = array_1d_inspect_value(t->entries, index);

		// Call print_func
		print_func(entry->key, entry->value);
	}
	
}
