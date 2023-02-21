//IT CAN HANDLE AT LEAST 200000 BRIDGES WITHOUT TAKING SOME TIME TO LOAD
//IT WILL NEVER GO BELOW 60 FPS BUT FOR TOO MANY BRIDGES (EXAMPLE 2MILLION) IT CAN TAKE SOME SECONDS TO LOAD. ONCE IT LOADS THOUGH THE FPS ARE CONSTANTLY 60
#include <stdlib.h>
#include "ADTSet.h"
#include "ADTList.h"
#include "state.h"
#include <stdio.h>
#include "set_utils.h"
#include "ADTMap.h"

// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Set objects;			// περιέχει στοιχεία Object (Εδαφος / Ελικόπτερα / Πλοία/ Γέφυρες)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};


// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, float x, float y, float width, float height) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->rect.x = x;
	obj->rect.y = y;
	obj->rect.width = width;
	obj->rect.height = height;
	return obj;
}

int Compare_Objects(Pointer a, Pointer b) {
    Object obj_a = a;
    Object obj_b = b;

    if (obj_a->rect.y < obj_b->rect.y) {
        return -1;
    } else if (obj_a->rect.y > obj_b->rect.y) {
        return 1;
    } else if (obj_a->rect.x < obj_b->rect.x){
        return -1;
    } else if (obj_a->rect.x > obj_b->rect.x) {
		return 1;
	} else if (obj_a->type > obj_b->type) {
		return 1;
	} else if (obj_a->type < obj_b->type) {
		return -1;
	} else {
		return 0;
	}
}	

// Προσθέτει αντικείμενα στην πίστα (η οποία μπορεί να περιέχει ήδη αντικείμενα).
// Τα αντικείμενα ξεκινάνε από y = start_y, και επεκτείνονται προς τα πάνω.
//
// ΠΡΟΣΟΧΗ: όλα τα αντικείμενα έχουν συντεταγμένες x,y σε ένα
// καρτεσιανό επίπεδο.
// - Στο άξονα x το 0 είναι το αριστερό μέρος της πίστας και οι συντεταγμένες
//   μεγαλώνουν προς τα δεξιά. Η πίστα έχει σταθερό πλάτος, οπότε όλες οι
//   συντεταγμένες x είναι ανάμεσα στο 0 και το SCREEN_WIDTH.
//
// - Στον άξονα y το 0 είναι η αρχή της πίστας, και οι συντεταγμένες μεγαλώνουν
//   προς τα _κάτω_. Η πίστα αντιθέτως επεκτείνεται προς τα πάνω, οπότε όλες οι
//   συντεταγμένες των αντικειμένων είναι _αρνητικές_.
//
// Πέρα από τις συντεταγμένες, αποθηκεύουμε και τις διαστάσεις width,height
// κάθε αντικειμένου. Τα x,y,width,height ορίζουν ένα παραλληλόγραμμο, οπότε
// μπορούν να αποθηκευτούν όλα μαζί στο obj->rect τύπου Rectangle (ορίζεται
// στο include/raylib.h). Τα x,y αναφέρονται στην πάνω-αριστερά γωνία του Rectangle.

static void add_objects(State state, float start_y) {
	// Προσθέτουμε BRIDGE_NUM γέφυρες.
	// Στο διάστημα ανάμεσα σε δύο διαδοχικές γέφυρες προσθέτουμε:
	// - Εδαφος, αριστερά και δεξιά της οθόνης (με μεταβαλλόμενο πλάτος).
	// - 3 εχθρούς (ελικόπτερα και πλοία)
	// Τα αντικείμενα έχουν SPACING pixels απόσταση μεταξύ τους.
	int width,left_width;
	for (int i = 0; i < BRIDGE_NUM; i++) {
		left_width = rand() % (SCREEN_WIDTH/3);
		width = rand() % (SCREEN_WIDTH/2);
		// Δημιουργία γέφυρας
		Object bridge = create_object(
			BRIDGE,
			left_width,								// x στο αριστερό άκρο της οθόνης
			start_y + 4 * (i+1) * SPACING,	// Η γέφυρα i έχει y = 4 * (i+1) * SPACING
			SCREEN_WIDTH - width - left_width,	// Πλάτος ανάμεσα στα 2 terain
			20								// Υψος
		);
		// Δημιουργία εδάφους
		Object terain_left = create_object(
			TERAIN,
			0,								// Αριστερό έδαφος, x = 0
			bridge->rect.y,					// y ίδιο με την γέφυρα
			left_width,		// Πλάτος επιλεγμένο τυχαία
			4*SPACING						// Υψος καλύπτει το χώρο ανάμεσα σε 2 γέφυρες
		);
		Object terain_right = create_object(
			TERAIN,
			SCREEN_WIDTH - width,			// Δεξί έδαφος, x = <οθόνη> - <πλάτος εδάφους>
			bridge->rect.y,					// y ίδιο με τη γέφυρα
			width+20,							// Πλάτος, επιλεγμένο τυχαία
			4*SPACING						// Υψος καλύπτει το χώρο ανάμεσα σε 2 γέφυρες
		);
		set_insert(state->objects, bridge);
		set_insert(state->objects, terain_left);
		set_insert(state->objects, terain_right);
		// Προσθήκη 3 εχθρών πριν από τη γέφυρα.
		for (int j = 0; j < 3; j++) {
			// Ο πρώτος εχθρός βρίσκεται SPACING pixels κάτω από τη γέφυρα, ο δεύτερος 2*SPACING pixels κάτω από τη γέφυρα, κλπ.
			//Sometimes enemies spawned inside the walls, so I code a different spawn point to make sure they always spawn in the middle of the terains they spawn in
			float y = bridge->rect.y + (j+1)*SPACING;
			int spawn_point = (SCREEN_WIDTH - width) - left_width - 125;
			while (spawn_point < left_width+90)
				spawn_point += 10;
			while (spawn_point > (SCREEN_WIDTH - width-90)) 
				spawn_point -= 10;

			Object enemy = rand() % 2 == 0		// Τυχαία επιλογή ανάμεσα σε πλοίο και ελικόπτερο
				? create_object(WARSHIP,    spawn_point, y, 60, 81)		// οριζόντιο κεντράρισμα
				: create_object(HELICOPTER, spawn_point, y, 50, 111);
			enemy->forward = rand() % 2 == 0;	// Τυχαία αρχική κατεύθυνση
			set_insert(state->objects, enemy);
		}
	}
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));
	// Γενικές πληροφορίες
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.
	state->info.score = 0;					// Αρχικό σκορ 0
	state->info.missile = NULL;				// Αρχικά δεν υπάρχει πύραυλος
	state->speed_factor = 1;				// Κανονική ταχύτητα

	// Δημιουργία του αεροσκάφους, κεντραρισμένο οριζόντια και με y = 0
	state->info.jet = create_object(JET, (SCREEN_WIDTH - 35)/2, 0, 35, 40);

	// Δημιουργούμε τη λίστα των αντικειμένων, και προσθέτουμε αντικείμενα
	// ξεκινώντας από start_y = 0.
	state->objects = set_create(Compare_Objects,free);
	add_objects(state, 0);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	return &state->info;
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη y είναι ανάμεσα στο y_from και y_to.

List state_objects(State state, float y_from, float y_to) {
    Object dummy = malloc(sizeof(Object));
    dummy->rect.y = y_from;
    Object dummy2 = malloc(sizeof(Object));
    dummy2->rect.y = y_to;
    List in_screen = list_create(NULL);
    Object value = set_find_eq_or_greater(state->objects,dummy);
    Object value2 = set_find_eq_or_smaller(state->objects,dummy2);
	//we get the closest objects in the set that match the coordinates given, in the loop we find the coresponding nodes and add the items to the list accordingly
    for(SetNode node = set_find_node(state->objects,value); node != set_find_node(state->objects,value2); node = set_next(state->objects,node)) {
        Object item = set_node_value(state->objects,node);
        list_insert_next(in_screen, LIST_BOF, item);
    }
	return in_screen;
}

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.
int last_bridge_y = - 4 * (BRIDGE_NUM) * SPACING;
void state_update(State state, KeyState keys) {
	//Those are here for the sole purpose of playing sounds while things happen
	if (state->info.game_over)
		state->info.game_over = false;
	if (state->info.missile_collision)
		state->info.missile_collision = false;
    if ((state->info.playing && !state->info.paused) || (state->info.paused && IsKeyDown(KEY_N))) {
        if (IsKeyDown(KEY_UP))
            keys->up = true;
        if (IsKeyDown(KEY_LEFT))
            keys->left = true;
        if (IsKeyDown(KEY_RIGHT))
            keys->right= true;
        if (IsKeyDown(KEY_DOWN))
            keys->down = true;
        if (IsKeyDown(KEY_P))
            keys->p = true;
        if (IsKeyDown(KEY_SPACE))
            keys->space = true;


		//CREATE A MISSILE IF THERE IS NOT ONE ALREADY AND (SPACE) BUTTON IS PRESSED
		if (keys->space && state->info.missile == NULL) {
            state->info.missile = create_object(MISSILE, state->info.jet->rect.x,  state->info.jet->rect.y+state->info.jet->rect.height/2 , 15, 10);
        }

        // GAME PAUSE BASED ON KEY (P)
        if(keys->p && !state->info.paused) {
			state->info.paused = true;
		}
        // JET MOVEMENT BASED ON KEY PRESSES
        state->info.jet->rect.y += keys->down ? 3 *state->speed_factor: 0;
        state->info.jet->rect.y -= keys->up ? 1 *state->speed_factor: 0;
        state->info.jet->rect.x -= keys->left ? 3 *state->speed_factor: 0;
        state->info.jet->rect.x += keys->right ? 3 * state->speed_factor : 0;
        state->info.jet->rect.y += 3 * state->speed_factor;

		

		//MAKE 2 SETS 1 FOR ENEMIES AND 1 FOR TERAIN
		List Drawn_Objects = state_objects(state,state->info.jet->rect.y-4000,state->info.jet->rect.y + 2000);
		Set enemies = set_create(Compare_Objects,NULL);
		Set terains = set_create(Compare_Objects,NULL);
		Map bounds = map_create(Compare_Objects,NULL,NULL);
		for(ListNode node = list_first(Drawn_Objects); node != LIST_EOF; node = list_next(Drawn_Objects,node)) {
			Object item = list_node_value(Drawn_Objects,node);
			if (item->type == HELICOPTER || item->type == WARSHIP || item->type == BRIDGE) {
				set_insert(enemies,item);
			} else {
				set_insert(terains,item);
			}
		}

		SetNode next_node = set_first(terains);
		next_node = set_next(terains,next_node);
		//MATCH TERAINS THAT ARE ON THE SAME LEVEL WITH THE MAP
		for(SetNode node = set_first(terains); node != SET_EOF; node = set_next(terains,node)) {
			Object item = set_node_value(terains,node);
			Object next_item = NULL;
			if(next_node != SET_EOF)
				next_item = set_node_value(terains,next_node);
			if(next_item!= NULL) {
				if (item->rect.x == 0) {
					map_insert(bounds,item,next_item);
				}
			}
			if(next_node != SET_EOF)
				next_node = set_next(terains,next_node);
		}
	
		//ENEMY MOVEMENT. ENEMY MOVEMENT IS NOT UPDATED IF THEY ARE FURTHER THAN 2 SCREENS!
		Object dummy_max = malloc(sizeof(*dummy_max));
		Object dummy_min = malloc(sizeof(*dummy_min));
		dummy_max->rect.y = state->info.jet->rect.y - (SCREEN_HEIGHT * 2);
		dummy_min->rect.y = state->info.jet->rect.y + (SCREEN_HEIGHT * 2);
		
		Object render_max; 
		render_max = set_find_eq_or_greater(enemies,dummy_max);
		Object render_min;
		render_min = set_find_eq_or_smaller(enemies,dummy_min);
		free(dummy_max);
		// free(dummy_min);
		SetNode node_next =	set_find_node(enemies,render_min);
		node_next = set_next(enemies,node_next);

		set_insert(enemies,dummy_min);
		node_next = set_find_node(enemies,dummy_min);

		for(SetNode node = set_find_node(enemies,render_max); node != node_next; node = set_next(enemies,node)) {
			Object enemy = set_node_value(enemies,node);
			Object left = NULL;
			Object right = NULL;
			//WE FIND THE TERAIN THAT CORRESPONDS THE ENEMY'S Y AXIS
			for(MapNode node = map_first(bounds); node != MAP_EOF; node = map_next(bounds,node)) {
				left = map_node_key(bounds,node);
				if(CheckCollisionRecs(left->rect,state->info.jet->rect)) {
					state->info.playing = false;
					return;
				}
				if (left->rect.y*-1 - enemy->rect.y*-1 < 800 && left->rect.y*-1 - enemy->rect.y*-1 > 0) {
					right = map_find(bounds,left);
					if(CheckCollisionRecs(right->rect,state->info.jet->rect)) {
						state->info.playing = false;
						return;
					}
					break;
				}
			}

			//GET THE MAX AND MIN VALUES THAT THE ENEMIES SHOULD MOVE WITHIN
			float min_x;
			float max_x;
			if (left != NULL && right != NULL) {
				min_x = left->rect.width+1;
				max_x = right->rect.x;
			}
			if (enemy->type == HELICOPTER)
				max_x -= 70;
			else if (enemy->type == WARSHIP)
				max_x -= 85;
			if (enemy->type == WARSHIP || enemy->type == HELICOPTER) {
				if(enemy->rect.x > max_x ||enemy->rect.x < min_x) {
					enemy->forward = enemy->forward ? false : true;
				}
			}
			if (enemy->type == HELICOPTER) {
				enemy->rect.x += (4 * (enemy->forward ? 1 : -1) * state->speed_factor);
			} else if (enemy->type == WARSHIP) {
				enemy->rect.x += (3 * (enemy->forward ? 1 : -1) * state->speed_factor);
			}
			
			
		}
		node_next =	set_find_node(state->objects,render_min);
		node_next = set_next(state->objects,node_next);
		for(SetNode node = set_find_node(state->objects,render_max); node != node_next; node = set_next(state->objects, node)) {
			Object item = set_node_value(state->objects,node);
			bool Collision = CheckCollisionRecs(item->rect,state->info.jet->rect); 
			if (Collision) {
				state->info.playing = false;
				return;
			}
		}

		if(state->info.missile) {
			if(state->info.missile->rect.y > state->info.jet->rect.y +SCREEN_HEIGHT/2+60) {
				state->info.missile = NULL;
				return;
			}
		}

		//CHECK FOR COLLISIONS WITH MISSILE WITHIN THE MAX AND MIN RENDER DISTANCE
        if (state->info.missile) {
			node_next =	set_find_node(state->objects,render_min);
			node_next = set_next(state->objects,node_next);
            for(SetNode node = set_find_node(state->objects,render_max); node != node_next; node = set_next(state->objects, node)) {
				Object item = set_node_value(state->objects,node);
				bool Collision = CheckCollisionRecs(item->rect,state->info.missile->rect);
				if (Collision) {
					if (item->type != TERAIN) {
						state->info.score += 10;
						set_remove(state->objects,item);
					}
					state->info.missile = NULL;
					state->info.missile_collision = true;
					break;
				}
			}
            if(state->info.missile)
                state->info.missile->rect.y += 10* state->speed_factor;

        }
		if (last_bridge_y - state->info.jet->rect.y  > (SCREEN_HEIGHT * 2)*-1) {
			add_objects(state,last_bridge_y);
			last_bridge_y -= 4 * (BRIDGE_NUM) * SPACING;
			state->speed_factor *= 1.3;
		};
		free(dummy_min);
		set_destroy(enemies);
		set_destroy(terains);
		list_destroy(Drawn_Objects);
		map_destroy(bounds);

    }  else { //WE GET HERE IF (1) THE GAME ENDS (2) THE GAME IS PAUSED
		if (!state->info.playing) {
			//IF THE PLAYER LOSES THEY CAN PRESS ENTER TO PLAY AGAIN
			state->info.game_over = true;
			if(IsKeyDown(KEY_ENTER)) 
				keys->enter = true;	
			state->info.playing = keys->enter ? true : false;
			if (state->info.playing) {
				//RESET GAME AFTER ENTER IS PRESSED
				state->info.paused = false;
				state->info.score = 0;
				state->info.jet->rect.y = 0;
				state->info.jet->rect.x =  (SCREEN_WIDTH - 35)/2;
				state->speed_factor = 1;
				if(state->info.missile != NULL)
					free(state->info.missile);
				state->info.missile = NULL;
				set_destroy(state->objects);
				state->objects = set_create(Compare_Objects,free);
				add_objects(state,0);
				state->info.game_over = false;
			}
			
		} else if (state->info.paused) {
			//IF GAME IS PAUSED CHECK IF THEY PRESS THE PAUSE BUTTON AGAIN THE GAME GOES ON
			if(IsKeyDown(KEY_P)) {
				keys->p = true;
			}
			if (keys->p) {
				state->info.paused = false;
			}
		} else {
			//IF NONE OF THOSE HAPPEN DESTROY THE STATE
			state_destroy(state);
		}
	}

}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	if(state->info.missile != NULL)
		free(state->info.missile);
	set_destroy(state->objects);
	free(state);
}