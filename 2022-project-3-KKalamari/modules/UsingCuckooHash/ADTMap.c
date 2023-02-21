/////////////////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Map μέσω Cuckoo hashing
//
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "ADTMap.h"


typedef enum {
	EMPTY, OCCUPIED, DELETED
} State;


int prime_sizes[] = {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241,
	786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};


#define MAX_LOAD_FACTOR 0.5

// Δομή του κάθε κόμβου που έχει το hash table (με το οποίο υλοιποιούμε το map)
struct map_node{
	Pointer key;		
	Pointer value;  	
	State state;		
};

// Δομή του Map (περιέχει όλες τις πληροφορίες που χρεαζόμαστε για το HashTable)
struct map {
	MapNode array_number_one;
	MapNode array_number_two;				
	int capacity;				
	int size;					
	CompareFunc compare;		
	HashFunc hash_function;	
	DestroyFunc destroy_key;	
	DestroyFunc destroy_value;
};

Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	Map map = malloc(sizeof(*map));
	map->capacity = prime_sizes[0];
	map->array_number_one = malloc(map->capacity * sizeof(struct map_node));	
	map->array_number_two = malloc(map->capacity * sizeof(struct map_node));	

	map->size = 0;
	map->compare = compare;
	map->destroy_key = destroy_key;
	map->destroy_value = destroy_value;

	for (int i = 0; i < map->capacity; i++) {
		map->array_number_one[i].state = EMPTY;
		map->array_number_two[i].state = EMPTY;
	}
		

	return map;

}

// Επιστρέφει τον αριθμό των entries του map σε μία χρονική στιγμή.
int map_size(Map map) {
	return map->size;
}

static void rehash(Map map) {
	int old_capacity = map->capacity;
	MapNode old_array_number_one = map->array_number_one;
	MapNode old_array_number_two = map->array_number_two;

	int prime_no = sizeof(prime_sizes) / sizeof(int);	
	for (int i = 0; i < prime_no; i++) {					    
		if (prime_sizes[i] > old_capacity) {
			map->capacity = prime_sizes[i]; 
			break;
		}
	}

	if (map->capacity == old_capacity)					
		map->capacity *= 2;							

	map->array_number_one = malloc(map->capacity * sizeof(struct map_node));
	map->array_number_two = malloc(map->capacity * sizeof(struct map_node));

	for (int i = 0; i < map->capacity; i++) {
		map->array_number_one[i].state = EMPTY;
		map->array_number_two[i].state = EMPTY;
	}

	map->size = 0;
	for (int i = 0; i < old_capacity; i++) { 
		if (old_array_number_one[i].state == OCCUPIED)
			map_insert(map, old_array_number_one[i].key, old_array_number_one[i].value);
		if(old_array_number_two[i].state == OCCUPIED)
			map_insert(map, old_array_number_two[i].key, old_array_number_two[i].value);
	}
		
	free(old_array_number_one);
	free(old_array_number_two);
}

// Εισαγωγή στο hash table του ζευγαριού (key, item). Αν το key υπάρχει,
// ανανέωση του με ένα νέο value, και η συνάρτηση επιστρέφει true.
void time_for_cuckoo(Map map, Pointer key, Pointer value, MapNode must_save, bool for_array, int count) {
	if(count > map->capacity) {	// εάν το count είναι μεγαλύτερο απο map->capacity τότε γίνεται rehash
		rehash(map);
		map_insert(map,must_save->key,must_save->value);
		return;
	}
	if(for_array) {  // ψάχνω για διαθέσιμη θέση στο πρώτο array, αν δεν βρω ξανακαλώ την time_for_cuckoo για το array 2
		uint position = ((map->hash_function(key) ^ 2)/ map->capacity) % map->capacity;
		if(map->array_number_two[position].state==EMPTY) { 
			must_save = &map->array_number_two[position];
			must_save->key = key;
			must_save ->value = value;
			must_save->state = OCCUPIED;
		}
		else { 
			count++;
			time_for_cuckoo(map,map->array_number_two[position].key, map->array_number_two[position].value,&map->array_number_two[position] ,false,count);
			must_save = &map->array_number_two[position];
			must_save->key = key;
			must_save ->value = value;
			must_save->state = OCCUPIED;
		}
	}
	else {	// ψάχνω για διαθέσιμη θέση στο δεύτερο array, αν δεν βρω ξανακαλώ την time_for_cuckoo για το array 1
		uint position = map->hash_function(key) % map->capacity;
		if(map->array_number_one[position].state==EMPTY) { 
			must_save = &map->array_number_one[position];
			must_save->key = key;
			must_save ->value = value;
			must_save->state = OCCUPIED;
		}
		else { 
			count++;
			time_for_cuckoo(map,map->array_number_one[position].key, map->array_number_one[position].value,&map->array_number_one[position] ,false,count);
			must_save = &map->array_number_one[position];
			must_save->key = key;
			must_save ->value = value;
			must_save->state = OCCUPIED;
		}
	}
}

void map_insert(Map map, Pointer key, Pointer value) {
	int count =0;
	bool saved = false; // μεταβλητή για την αποθήκευση του ζευγαριού
	bool exists = false; // μεταβλητή για το αν υπαρχει ήδη το ζευγάρι
	MapNode node = NULL;
	MapNode to_be_saved = NULL;
	uint position_one = map->hash_function(key) % map->capacity;
	if(map_find(map,key)) {
		node = map_find_node(map,key);
		exists = true;
	}
	else {		
		if(map->array_number_one[position_one].state==EMPTY)  // Βρήκαμε την θέση που μπαίνει
			saved = true;	
		else {
			to_be_saved = &map->array_number_one[position_one];   // αλλιως αποθηκεύουμε το στοιχείο που υπήρχε ήδη στην θεση σε προσωρινη μεταβλητη
			to_be_saved->key = map->array_number_one[position_one].key;
			to_be_saved->value = map->array_number_one[position_one].value;			
			saved = true;
			time_for_cuckoo(map,to_be_saved->key,to_be_saved->value,to_be_saved, true, count); // αν το bool είναι true τότε ψάχνω στο array 2, αν είναι false ψάχνω στο array 1
		}
	}
		
	if(saved) {
		if(!exists) {
			node = &map->array_number_one[position_one];
			node->key = key;
			node->value = value;
			node->state = OCCUPIED;
		}	
	}

	if (exists) {
		if (node->key != key && map->destroy_key != NULL)
			map->destroy_key(node->key);

		if (node->value != value && map->destroy_value != NULL)
			map->destroy_value(node->value);
	} 
	else 
		map->size++;
	
	float load_factor = (float)map->size / map->capacity;
	if (load_factor > MAX_LOAD_FACTOR)
		rehash(map);
}


// Διαργραφή απο το Hash Table του κλειδιού με τιμή key
bool map_remove(Map map, Pointer key) {
	MapNode for_removal = map_find_node(map,key);  //εαν υπάρχει ο κομβος τον διαγραφω αλλιώς επιστρέφω false
	if(for_removal != MAP_EOF)	{
		if (map->destroy_key != NULL)
			map->destroy_key(for_removal->key);
		if (map->destroy_value != NULL)
			map->destroy_value(for_removal->value);

		for_removal->state = DELETED;
		map->size--;

		return true;
	} 
		return false;
}

// Αναζήτηση στο map, με σκοπό να επιστραφεί το value του κλειδιού που περνάμε σαν όρισμα.

Pointer map_find(Map map, Pointer key) {
	MapNode node = map_find_node(map,key);
	if(node == MAP_EOF)
		return NULL;
	else	
		return node->value;
}


DestroyFunc map_set_destroy_key(Map map, DestroyFunc destroy_key) {
	DestroyFunc old = map->destroy_key;
	map->destroy_key = destroy_key;
	return old;
}

DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value) {
	DestroyFunc old = map->destroy_value;
	map->destroy_value = destroy_value;
	return old;
}

// Απελευθέρωση μνήμης που δεσμεύει το map
void map_destroy(Map map) {
	for (int i = 0; i < map->capacity; i++) {
		if (map->array_number_one[i].state == OCCUPIED) {
			if (map->destroy_key != NULL)
				map->destroy_key(map->array_number_one[i].key);
			if (map->destroy_value != NULL)
				map->destroy_value(map->array_number_one[i].value);
		}
	}

	for (int i = 0; i < map->capacity; i++) {
		if (map->array_number_two[i].state == OCCUPIED) {
			if (map->destroy_key != NULL)
				map->destroy_key(map->array_number_two[i].key);
			if (map->destroy_value != NULL)
				map->destroy_value(map->array_number_two[i].value);
		}
	}

	free(map->array_number_one);
	free(map->array_number_two);
	free(map);
}

/////////////////////// Διάσχιση του map μέσω κόμβων ///////////////////////////

MapNode map_first(Map map) {
	for(int i = 0; i< map->capacity; i++) {
		if(map->array_number_one[i].state==OCCUPIED)
			return &map->array_number_one[i];
	}
	return MAP_EOF;
}

MapNode map_next(Map map, MapNode node) {
	for (int i = 0; i < map->capacity; i++) {
		if (map->array_number_one[i].state == OCCUPIED)
			return &map->array_number_two[i];
		if (map->array_number_two[i].state == OCCUPIED)
			return &map->array_number_two[i];
	}
	return MAP_EOF;
}

Pointer map_node_key(Map map, MapNode node) {
	return node->key;
}

Pointer map_node_value(Map map, MapNode node) {
	return node->value;
}

MapNode map_find_node(Map map, Pointer key) {
	uint position = map->hash_function(key) % map->capacity;
	uint position_2 = ((map->hash_function(key)^2) / map->capacity) % map->capacity;
	
	if(map->array_number_one[position].state==OCCUPIED && map->compare(map->array_number_one[position].key,key)==0)
		return &map->array_number_one[position];

	if(map->array_number_two[position_2].state==OCCUPIED && map->compare(map->array_number_two[position_2].key,key)==0)
		return &map->array_number_two[position];

	return MAP_EOF;
}

// Αρχικοποίηση της συνάρτησης κατακερματισμού του συγκεκριμένου map.
void map_set_hash_function(Map map, HashFunc func) {
	map->hash_function = func;
}

uint hash_string(Pointer value) {
	// djb2 hash function, απλή, γρήγορη, και σε γενικές γραμμές αποδοτική
    uint hash = 5381;
    for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;			// hash = (hash * 33) + *s. Το foo << 5 είναι γρηγορότερη εκδοχή του foo * 32.
    return hash;
}

uint hash_int(Pointer value) {
	return *(int*)value;
}

uint hash_pointer(Pointer value) {
	return (size_t)value;				// cast σε sizt_t, που έχει το ίδιο μήκος με έναν pointer
}