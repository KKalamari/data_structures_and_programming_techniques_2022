//STATE.C CAN HANDLE UP TO 80000 BRIDGES
#include <stdlib.h>
#include <stdio.h>
#include "ADTList.h"
#include "state.h"


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	List objects;			// περιέχει στοιχεία Object (Εδαφος / Ελικόπτερα / Πλοία/ Γέφυρες)
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
	int left_width,width;
	for (int i = 0; i < BRIDGE_NUM; i++) {
		left_width = (rand() % (SCREEN_WIDTH/4));
		width = rand() % (SCREEN_WIDTH/2);
		// Δημιουργία γέφυρας
		Object bridge = create_object(
			BRIDGE,
			left_width,								// x στο αριστερό άκρο της οθόνης
			start_y - 4 * (i+1) * SPACING,	// Η γέφυρα i έχει y = 4 * (i+1) * SPACING
			SCREEN_WIDTH - width - left_width,					// Πλάτος ολόκληρη η οθόνη
			20								// Υψος
		);
		
		// Δημιουργία εδάφους
		Object terain_left = create_object(
			TERAIN,
			0,								// Αριστερό έδαφος, x = 0
			bridge->rect.y,					// y ίδιο με την γέφυρα
			left_width,						// Πλάτος επιλεγμένο τυχαία
			4*SPACING						// Υψος καλύπτει το χώρο ανάμεσα σε 2 γέφυρες
		);
		Object terain_right = create_object(
			TERAIN,
			SCREEN_WIDTH- width, 			// Δεξί έδαφος, x = <οθόνη> - <πλάτος εδάφους>
			bridge->rect.y,					// y ίδιο με τη γέφυρα
			width+20,							// Πλάτος, επιλεγμένο τυχαία
			4*SPACING						// Υψος καλύπτει το χώρο ανάμεσα σε 2 γέφυρες
		);
		list_insert_next(state->objects, list_last(state->objects), terain_left);
		list_insert_next(state->objects, list_last(state->objects), terain_right);
		list_insert_next(state->objects, list_last(state->objects), bridge);
		// Προσθήκη 3 εχθρών πριν από τη γέφυρα.
		for (int j = 0; j < 3; j++) {
			// Ο πρώτος εχθρός βρίσκεται SPACING pixels κάτω από τη γέφυρα, ο δεύτερος 2*SPACING pixels κάτω από τη γέφυρα, κλπ.
			float y = bridge->rect.y + (j+1)*SPACING;
			int spawn_point = (SCREEN_WIDTH - width) - left_width - 125;
			while (spawn_point < left_width+90)
				spawn_point += 10;
			while (spawn_point > (SCREEN_WIDTH - width-90)) 
				spawn_point -= 10;
			// printf("LEFT WALL %d RIGHT WALL %d SPAWN POINT %d Y %f\n",left_width,(SCREEN_WIDTH - width),spawn_point,bridge->rect.y);
			Object enemy = rand() % 2 == 0		// Τυχαία επιλογή ανάμεσα σε πλοίο και ελικόπτερο
				? create_object(WARSHIP,    spawn_point, y, 83, 30)		// οριζόντιο κεντράρισμα
				: create_object(HELICOPTER, spawn_point, y, 66, 25);
			enemy->forward = rand() % 2 == 0;	// Τυχαία αρχική κατεύθυνση

			list_insert_next(state->objects, list_last(state->objects), enemy);
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
	state->objects = list_create(free);
	add_objects(state, 0);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	return &state->info;
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη y είναι ανάμεσα στο y_from και y_to.
int last_bridge_y = - 4 * (BRIDGE_NUM) * SPACING;
List state_objects(State state, float y_from, float y_to) {
	List Screen_Objects = list_create(NULL);
	
	for(ListNode node = list_first(state->objects);
	    node != LIST_EOF;
		node = list_next(state->objects,node)) {
			Object item = list_node_value(state->objects,node);
			if (item->rect.y >= y_from+500 && item->rect.y <= y_to && item->type != TERAIN) {
				list_insert_next(Screen_Objects,LIST_BOF,item);
			}
		}
	for(ListNode node = list_first(state->objects);
	    node != LIST_EOF;
		node = list_next(state->objects,node)) {
			Object item = list_node_value(state->objects,node);
			if (item->rect.y >= y_from && item->rect.y <= y_to && item->type == TERAIN) {
				list_insert_next(Screen_Objects,LIST_BOF,item);
			}
		}
	return Screen_Objects;
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


// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {
	if (state->info.game_over)
		state->info.game_over = false;
	if (state->info.missile_collision)
		state->info.missile_collision = false;
	//IS THE GAME SUPPOSED TO CONTINUE? IS IT PAUSED? ARE THOSE DEBUG FRAMES?
	if ((state->info.playing && !state->info.paused) || (state->info.paused && IsKeyDown(KEY_N))) {
		
	// DETECT KEY PRESSES
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
		// GAME PAUSE BASED ON KEY (P)
		if(keys->p && !state->info.paused) {
			state->info.paused = true;
		}
		// JET MOVEMENT BASED ON KEY PRESSES
		state->info.jet->rect.y -= keys->up ? 3 *state->speed_factor: 0;
		state->info.jet->rect.y += keys->down ? 1 *state->speed_factor: 0;
		state->info.jet->rect.x -= keys->left ? 3 *state->speed_factor: 0;
		state->info.jet->rect.x += keys->right ? 3 * state->speed_factor : 0;
		state->info.jet->rect.y -= 3 * state->speed_factor;

		// state->info.jet->rect.y *= state->speed_factor;
		//CHECK MISSILE COLLISIONS - MOVEMENT
		if (keys->space && state->info.missile == NULL) {
			state->info.missile = create_object(MISSILE, state->info.jet->rect.x,  state->info.jet->rect.y - 5, 15, 10);
		}

		
	//CREATE 2 LISTS FOR TERAIN OBJECTS AND ENEMY OBJECTS
		List terains = list_create(NULL);
		List enemies = list_create(NULL);
		List Objects = state_objects(state,state->info.jet->rect.y-4000,state->info.jet->rect.y + 500);
		for(ListNode node = list_first(Objects); node != LIST_EOF; node = list_next(Objects,node)) {
			//CHECK IF ANYTHING COLLIDES WITH THE JET AND IF SO THE GAME ENDS
			Object item = list_node_value(Objects,node);
			bool JetCollision = false;
			JetCollision = CheckCollisionRecs(state->info.jet->rect,item->rect);
			if (JetCollision) {
				state->info.playing = false;
				state->info.game_over = true;
				return;
			}
			switch(item->type) {
				case TERAIN:
					list_insert_next(terains,LIST_EOF,item);
					break;
				case HELICOPTER:
					list_insert_next(enemies,LIST_EOF,item);
					break;
				case WARSHIP:
					list_insert_next(enemies,LIST_EOF,item);
					break;
				case BRIDGE:
					break;
				case JET:
					break;
				case MISSILE:
					break;
			}
		}

		//CHECK FOR COLLISIONS WITH MISSILE AND IF THEY HAPPEN REMOVE THE OBJECT FROM THE LIST
		if(state->info.missile) {
			ListNode prev = list_first(state->objects);
			int count = 0;
			for(ListNode node = list_first(state->objects); node != LIST_EOF; node = list_next(state->objects,node)) {
				Object item = list_node_value(state->objects,node);
				bool Collision = CheckCollisionRecs(state->info.missile->rect,item->rect);
				if (Collision) {
					Object prev_item = list_node_value(state->objects,prev);
					//IF THE OBJECT HIT IS TERAIN, WE DONT WANT TO ADD POINTS OR REMOVE THE OBJECT
					if (item->type == HELICOPTER || item->type == WARSHIP || item->type == BRIDGE) {
						ListNode true_node = list_find_node(state->objects,prev_item,Compare_Objects);
						list_remove_next(state->objects,true_node);
						state->info.score += 10;
					} 
					free(state->info.missile);
					state->info.missile = NULL;
					state->info.missile_collision = true;
					break;
				}
				//GET THE PREVIOUS OBJECT 
				if(count > 0)
					prev = list_next(state->objects,prev);
				count++;

			}
			//IF THE MISSILE HASN'T COLLIDED MOVE THE MISSILE
			if(state->info.missile != NULL)
				state->info.missile->rect.y -= 10;
			
		}

		
		//ENEMY MOVEMENT
		for(ListNode node = list_first(enemies); node != LIST_EOF; node = list_next(enemies,node)) {
			Object enemy = list_node_value(enemies,node);
			if (enemy->type == HELICOPTER) {
				enemy->rect.x += (4 * (enemy->forward ? 1 : -1) * state->speed_factor);
			} else if (enemy->type == WARSHIP) {
				enemy->rect.x += (3 * (enemy->forward ? 1 : -1) * state->speed_factor);
			}
		}

		//CHECK COLLISION BETWEEN TERAIN AND ENEMY TO CHANGE DIRECTION IF SO
		for(ListNode node = list_first(enemies); node != LIST_EOF; node = list_next(enemies,node)) {
			Object enemy = list_node_value(enemies,node);
			bool Collision = false;
			for(ListNode terain_node = list_first(terains); terain_node != LIST_EOF; terain_node = list_next(terains,terain_node)) {
				Object terain = list_node_value(terains,terain_node);
				Collision = CheckCollisionRecs(enemy->rect,terain->rect);
				if (Collision) {
					enemy->forward = enemy->forward ? false : true;
					while (Collision) {
						enemy->rect.x += (3 * (enemy->forward ? 1 : -1));
						Collision = CheckCollisionRecs(enemy->rect,terain->rect);
					}
				}
			}
		}

		//GET LAST BRIDGE COORDINATES TO LOAD MORE OBJECTS
		if (last_bridge_y - state->info.jet->rect.y  > (SCREEN_HEIGHT * 2)*-1) {
			add_objects(state,last_bridge_y);
			last_bridge_y -= 4 * (BRIDGE_NUM) * SPACING;
			state->speed_factor *= 1.3;
		}

		list_destroy(terains);
		list_destroy(Objects);
		list_destroy(enemies);
	}  else { //WE GET HERE IF (1) THE GAME ENDS (2) THE GAME IS PAUSED
		if (!state->info.playing) {
			//IF THE PLAYER LOSES THEY CAN PRESS ENTER TO PLAY AGAIN
			if(IsKeyDown(KEY_ENTER)) 
				keys->enter = true;	
			state->info.playing = keys->enter ? true : false;
			if (state->info.playing) {
				//RESET GAME AFTER ENTER IS PRESSED
				state->info.paused = false;
				state->info.score = 0;
				state->info.jet->rect.y = 0;
				state->info.jet->rect.x =  (SCREEN_WIDTH - 35)/2;
				if(state->info.missile != NULL) {
					free(state->info.missile);
					state->info.missile = NULL;
				}
				state->speed_factor = 1;
				//EMPTY THE LIST WITHOUT DESTROYING IT AND RECREATING IT
				while (list_size(state->objects) > 0) {
					list_remove_next(state->objects,LIST_BOF);
				}

				add_objects(state, 0);
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

	list_destroy(state->objects);
	if(state->info.missile != NULL) {
		free(state->info.missile);
		state->info.missile = NULL;
	}
	free(state->info.jet);
	free(state);
}