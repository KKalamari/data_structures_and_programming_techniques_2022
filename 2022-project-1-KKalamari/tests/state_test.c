//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include "ADTSet.h"
#include "state.h"

void test_state_create() {

	State state = state_create();
	TEST_ASSERT(state != NULL);

	StateInfo info = state_info(state);
	TEST_ASSERT(info != NULL);
	
	TEST_ASSERT(info->playing);
	TEST_ASSERT(!info->paused);
	TEST_ASSERT(info->score == 0);
	List Screen_Objects = state_objects(state,-5000,5000);
	bool in_coords = true;
	int size =  list_size(Screen_Objects);
	TEST_ASSERT(size > 0);
	for(ListNode node = list_first(Screen_Objects); node != LIST_EOF; node = list_next(Screen_Objects,node)) {
		Object item = list_node_value(Screen_Objects,node);
		if (item->rect.y > 5000 || item->rect.y < -5000) {
			in_coords = false;
		}
	}
	TEST_ASSERT(in_coords);
	list_destroy(Screen_Objects);	
	state_destroy(state);
}

void test_state_update() {
	State state = state_create();
	List Screen_Objects = state_objects(state,-4000,0);
	TEST_ASSERT(state != NULL && state_info(state) != NULL);

	// Πληροφορίες για τα πλήκτρα (αρχικά κανένα δεν είναι πατημένο)
	struct key_state keys = { false, false, false, false, false, false, false, false };
	
	// Χωρίς κανένα πλήκτρο, το αεροσκάφος μετακινείται 3 pixels πάνω
	Rectangle old_rect = state_info(state)->jet->rect;
	state_update(state, &keys);
	Rectangle new_rect = state_info(state)->jet->rect;
	//WHILE NO BUTTON IS PRESSED, MOVE 3 PIXELS
	TEST_ASSERT( new_rect.x == old_rect.x && new_rect.y == old_rect.y - 3 );
	state_update(state, &keys);
	// WHILE UP IS PRESSED, MOVE 6 PIXELS
	keys.up = true;
	old_rect = state_info(state)->jet->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->jet->rect;
	TEST_CHECK( new_rect.x == old_rect.x && new_rect.y == old_rect.y - 6 );
	keys.up = false;

	// WHILE LEFT BUTTON IS PRESSED, MOVE 3 TO THE RIGHT AND 3 UP
	keys.left = true;
	old_rect = state_info(state)->jet->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->jet->rect;
	TEST_CHECK( new_rect.x == old_rect.x - 3 && new_rect.y == old_rect.y - 3);
	keys.left = false;


	// CHECK IF GAMEOVER WORKS
	state_info(state)->playing = false;
	old_rect = state_info(state)->jet->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->jet->rect;

	//CHECK IF PRESSING ENTER RESTARTS THE GAME
	keys.enter = true;
	old_rect = state_info(state)->jet->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->jet->rect;
	TEST_ASSERT(state_info(state)->playing);
	// TEST_ASSERT( new_rect.x == (SCREEN_WIDTH - 35)/2 && new_rect.y == 0);
	// CHECK IF GAME ACTUALLY RESTARTS WHEN ENTER IS PRESSED BY CHECKING JET MOVEMENT
	keys.enter = false;
	old_rect = state_info(state)->jet->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->jet->rect;
	TEST_ASSERT( new_rect.x == old_rect.x && new_rect.y == old_rect.y - 3);


	//CHECK GAME PAUSE FUNCTION
	keys.p = true;
	state_update(state, &keys);
	old_rect = state_info(state)->jet->rect;
	new_rect = state_info(state)->jet->rect;
	TEST_ASSERT( new_rect.x == old_rect.x && new_rect.y == old_rect.y);
	state_update(state, &keys);
	keys.p = false;	

	// CHECK IF THE GAME GOES ON AS IT SHOULD BE IF WE UNPAUSE
	old_rect = state_info(state)->jet->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->jet->rect;
	TEST_ASSERT( new_rect.x == old_rect.x && new_rect.y == old_rect.y - 3);

	state_update(state,&keys);

	// CHECK THAT THE DEBUG BUTTON (N) PLAYS THE GAME FOR 1 FRAME WHILE PAUSED AS IT SHOULD BE
	keys.p = true;
	state_update(state, &keys);
	old_rect = state_info(state)->jet->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->jet->rect;
	TEST_ASSERT( new_rect.x == old_rect.x && new_rect.y == old_rect.y);
	keys.n = true;
	old_rect = state_info(state)->jet->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->jet->rect;

	TEST_ASSERT( new_rect.x == old_rect.x && new_rect.y == old_rect.y - 3);
	keys.n = false;
	old_rect = state_info(state)->jet->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->jet->rect;
	TEST_ASSERT( new_rect.x == old_rect.x && new_rect.y == old_rect.y);
	keys.p = false;

	// !!!MISSILE FUNCTIONALITY!!!
	// CHECK THAT THERE IS NO MISSILE BEFORE WE CREATE IT
	TEST_ASSERT(state_info(state)->missile == NULL);
	keys.space = true;
	state_update(state, &keys);
	// CHECK THAT MISSILE IS CREATED
	state_update(state, &keys);
	state_update(state, &keys);
	TEST_ASSERT(state_info(state)->missile != NULL);
	old_rect = state_info(state)->missile->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->missile->rect;
	//CHECK THAT MISSILE IS MOVING
	TEST_ASSERT( new_rect.x == old_rect.x && new_rect.y == old_rect.y - 10);
	keys.space = false;
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);	
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);
	state_update(state, &keys);

	TEST_ASSERT(state_info(state)->missile == NULL);

	list_destroy(Screen_Objects);
	state_destroy(state);


	// Προσθέστε επιπλέον ελέγχους
}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_state_create", test_state_create },
	{ "test_state_update", test_state_update },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};