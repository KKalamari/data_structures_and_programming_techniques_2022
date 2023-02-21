/////////////////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Map μέσω Hash Table (με Set αντί για λίστες)
//
/////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include "ADTSet.h"
#include "ADTMap.h"


#define MAX_LOAD_FACTOR 0.5
#define FIXED_SIZE 3
typedef enum {
	EMPTY, OCCUPIED
} State;

int prime_sizes[] = {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241,
	786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};

// Δομή του κάθε κόμβου που έχει το hash table (με το οποίο υλοιποιούμε το map)
struct map_node{
	Pointer key;
	Pointer value;
	State state;
	Set set;
	Pointer elements[FIXED_SIZE];
	bool full; //Είναι full όταν έχουν εισαχθεί >= FIXED_SIZE στοιχεία
};

// Δομή του Map (περιέχει όλες τις πληροφορίες που χρεαζόμαστε για το HashTable)
struct map {
	int size;
	int capacity;
	CompareFunc compare;
	HashFunc hash_function;
	DestroyFunc destroy_key;
	DestroyFunc destroy_value;
	MapNode the_array;
};



Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	Map map = malloc(sizeof(*map));
	map->capacity = prime_sizes[0];
	map->the_array = malloc(map->capacity * sizeof(struct map_node));
	map->size = 0;

	for (int i = 0; i < map->capacity; i++) {       //αρχικοποιώ το map->array.state= empty και το pointer array του map_node = null
		map->the_array[i].state = EMPTY; 	
		map->the_array[i].full = false;
		for(int x = 0; x < FIXED_SIZE; x++) 
			map->the_array[i].elements[x]=NULL;
	}
	
	map->compare = compare;
	map->destroy_key = destroy_key;
	map->destroy_value = destroy_value;

	return map;
}

// Επιστρέφει τον αριθμό των entries του map σε μία χρονική στιγμή.
int map_size(Map map) {
	return map->size;
}

static void rehash(Map map) {
	
	MapNode prev_array = map->the_array;
	int old_capacity = map->capacity;

	int prime_no = sizeof(prime_sizes) / sizeof(int);	
	for (int i = 0; i < prime_no; i++) {					
		if (prime_sizes[i] > old_capacity) {
			map->capacity = prime_sizes[i]; 
			break;
		}
	}

	if (map->capacity == old_capacity)					
		map->capacity *= 2;								

	map->the_array = malloc(map->capacity * sizeof(struct map_node));
	map->size = 0;
	for (int i = 0; i < map->capacity; i++) {       //αρχικοποιώ το map->array.state= empty και το pointer array του map_node = null
		map->the_array[i].state = EMPTY; 	
		map->the_array[i].full = false;
		for(int x = 0; x < FIXED_SIZE; x++) 
			map->the_array[i].elements[x]=NULL;
	}
	for (int i = 0; i < old_capacity; i++) {
	
		if (prev_array[i].state == OCCUPIED) {
			map_insert(map, prev_array[i].key, prev_array[i].value);

			if(prev_array[i].set!=NULL)		
				set_destroy(prev_array[i].set);
		}					
	}
	free(prev_array);
}

// Εισαγωγή στο hash table του ζευγαριού (key, item). Αν το key υπάρχει,
// ανανέωση του με ένα νέο value, και η συνάρτηση επιστρέφει true.

void map_insert(Map map, Pointer key, Pointer value) {
	bool exists = false; //Μεταβλήτη που χρησιμοποιώ σε περίπτωση που δεν χρειάζεται να αλλάξει το map->size
	MapNode node = NULL;
	uint position = map->hash_function(key) % map->capacity ;
	
	if(map->the_array[position].full==false) { // ελέγχω αν του array[position] ειναι full
		for(int i = 0; i<FIXED_SIZE; i++) {

			if(map->the_array[position].elements[i]==NULL) { // ελέγχω εάν υπάρχει κενή θέση στο array του Mapnode
				map->the_array[position].elements[i]=value; //αν υπάρχει αποθηκεύω εκεί το στοιχείο
				node = &map->the_array[position];

				if(i == FIXED_SIZE-1)   //εάν το στοιχείο εισαχθεί στην FIXED_SIZE-1 τότε full=true
					map->the_array[position].full = true;

				break;
			}
			else if (map->compare(map->the_array[position].key, key) == 0) {// ελέγχω εάν το key υπάρχει ήδη 
				exists = true; 
				node = &map->the_array[position];						
				break;											
			}
		}
	}
	else {// αν το array[position] είναι full , δημιουργώ το set (εάν δεν υπάρχει) και περνάω τα στοιχεία του κόμβου αε αυτό
		if(map->the_array[position].set==NULL) {
			map->the_array[position].set = set_create(map->compare,map->destroy_value); 

			for(int i = 0; i<FIXED_SIZE; i++) {
				set_insert(map->the_array[position].set,map->the_array[position].elements[i]);
				map->the_array[position].elements[i]=NULL;
			}		
		}	
		set_insert(map->the_array[position].set,value); //εισάγω το νέο στοιχείο
		node = &map->the_array[position];
	}

	if (exists) { //αντικαθιστώ key/value εαν υπάρχουν ήδη
		if (node->key != key && map->destroy_key != NULL)
			map->destroy_key(node->key);

		if (node->value != value && map->destroy_value != NULL)
			map->destroy_value(node->value);
	} else 
		map->size++;
	
	node->state = OCCUPIED;
	node->key = key;
	node->value = value;
	
	float load_factor = map->size / (float)map->capacity;
	if(load_factor>MAX_LOAD_FACTOR) {
		rehash(map);
	}
}

// Διαργραφή απο το Hash Table του κλειδιού με τιμή key
//Βρήσκω εάν υπάρχει node στη τιμή key, εάν υπάρχει κανω destroy key , value το θέτω state= empty
// και μειώνω το size. Αν δεν υπάρχει επιστρέφω false
bool map_remove(Map map, Pointer key) {
	MapNode for_removal = map_find_node(map,key);

	if(for_removal != MAP_EOF) {
		if(map->destroy_key != NULL)
			map->destroy_key(for_removal->key);
		if(map->destroy_value != NULL)
			map->destroy_value(for_removal->value);

		for_removal->state = EMPTY; 
		map->size--;

		return true;
	}
	else 
		return false;

}

// Αναζήτηση στο map, με σκοπό να επιστραφεί το value του κλειδιού που περνάμε σαν όρισμα.

Pointer map_find(Map map, Pointer key) {
	MapNode node = map_find_node(map,key);
	
	if(node != MAP_EOF)	
		return node->value;
	else 
		return NULL;
}


DestroyFunc map_set_destroy_key(Map map, DestroyFunc destroy_key) {
	DestroyFunc func = map->destroy_key;
	map->destroy_key = destroy_key;
	return func;
}

DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value) {
	DestroyFunc func = map->destroy_key;
	map->destroy_value = destroy_value;
	return func;
}

// Απελευθέρωση μνήμης που δεσμεύει το map

void map_destroy(Map map) {

	for (int i = 0; i < map->capacity; i++) {
		
		if(map->the_array[i].full==true) {
			if(map->the_array[i].set==NULL)	{

			if (map->destroy_key != NULL)
				map->destroy_key(map->the_array[i].key);

			if (map->destroy_value != NULL)
				map->destroy_value(map->the_array[i].value);
			}
			else 
				set_destroy(map->the_array[i].set);
			
		}
		else {
			if(map->the_array[i].state==OCCUPIED) {
				if (map->destroy_key != NULL)
					map->destroy_key(map->the_array[i].key);

				if (map->destroy_value != NULL)
					map->destroy_value(map->the_array[i].value);
			}
		}
	}

	free(map->the_array);
	free(map);
}

/////////////////////// Διάσχιση του map μέσω κόμβων ///////////////////////////

MapNode map_first(Map map) {  //επιστρέφω τον πρώτο occupied κόμβο
	for(int i = 0; i<map->capacity; i++) {

		if(map->the_array[i].state==OCCUPIED)
			return &map->the_array[i];
	}

	return MAP_EOF;
}

MapNode map_next(Map map, MapNode node) { 
	for(int i = 0; i<map->capacity-1; i++) {

			if(map->compare(map->the_array[i].key,node->key)==0)
				return &map->the_array[i+1];
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

	if(map->the_array[position].state==OCCUPIED) 
		return &map->the_array[position];		
		
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