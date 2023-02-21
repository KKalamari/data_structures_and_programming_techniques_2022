#include "raylib.h"

#include "state.h"
#include "interface.h"
#include <stdio.h>
Texture2D faceless_man,haku,helpers,lanters,terain_left,terain_right,ubaba,ubabaleft,faceless_manleft;
Texture Background;
Sound hakuu;
Music my_music;

void interface_init() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "videogame");
    SetTargetFPS(60);
    InitAudioDevice();
	

	faceless_manleft = LoadTextureFromImage(LoadImage("assets/faceless_manleft.png"));
	ubabaleft = LoadTextureFromImage(LoadImage("assets/ubaba_going_left.png"));
	helpers = LoadTextureFromImage(LoadImage("assets/helpers.png"));
	ubaba = LoadTextureFromImage(LoadImage("assets/ubaba.png"));
	terain_right = LoadTextureFromImage(LoadImage("assets/terain_right.png"));
	terain_left = LoadTextureFromImage(LoadImage("assets/terain_left.png"));
	lanters = LoadTextureFromImage(LoadImage("assets/lanterns.png"));
	haku = LoadTextureFromImage(LoadImage("assets/haku.png"));
	faceless_man = LoadTextureFromImage(LoadImage("assets/faceless_man.png"));
	Background =LoadTextureFromImage(LoadImage("assets/backround.png"));
	my_music = LoadMusicStream("assets/spirited_away_theme.mp3");

	hakuu = LoadSound("assets/haku.mp3");
	PlayMusicStream(my_music);
}

void interface_close() {
    CloseAudioDevice();
    CloseWindow();
}

int frames = 0;
void interface_draw_frame(State state) {
    BeginDrawing();
    ClearBackground(DARKBLUE);
	DrawTexture(Background,0,0,	WHITE);

    List items = state_objects(state, state_info(state)->jet->rect.y-4000,state_info(state)->jet->rect.y+2000);
	for(ListNode node = list_first(items); node != LIST_EOF; node = list_next(items,node)) {
		Object item = list_node_value(items,node);

		if(item->type== BRIDGE)
			DrawTexture(lanters,0,item->rect.y+((state_info(state)->jet->rect.y) * -1) +245,WHITE);
		
		if(item->type==WARSHIP) {
			if(item->forward)
			DrawTexture(ubaba,item->rect.x-13,item->rect.y+((state_info(state)->jet->rect.y) * -1) +275,WHITE);
			else 
			DrawTexture(ubabaleft,item->rect.x-13,item->rect.y+((state_info(state)->jet->rect.y) * -1) +275,WHITE);
		}
		
		if(item->type==HELICOPTER) {
			if(item->forward)
			DrawTexture(faceless_manleft,item->rect.x-13,item->rect.y+((state_info(state)->jet->rect.y) * -1) +275,WHITE);
			else
			DrawTexture(faceless_man,item->rect.x-13,item->rect.y+((state_info(state)->jet->rect.y) * -1) +275,WHITE);
		}
		if(item->type==TERAIN) {	
			if(item->rect.x > SCREEN_WIDTH/2)			
				DrawTexture(terain_right,item->rect.x-45,item->rect.y+((state_info(state)->jet->rect.y) * -1) +275,WHITE);
			else {
				DrawTexture(terain_left,item->rect.width-120,item->rect.y+((state_info(state)->jet->rect.y) * -1) +275,WHITE);
			}
		}
	}
	if(state_info(state)->missile != NULL) 
		DrawTexture(helpers,state_info(state)->missile->rect.x-13,300 - (state_info(state)->jet->rect.y + state_info(state)->missile->rect.y * -1) - 35,WHITE);
	
	DrawTexture(haku,state_info(state)->jet->rect.x-30,300-36,WHITE);

	// Σχεδιάζουμε το σκορ και το FPS counter
	DrawText(TextFormat("%04i", state_info(state)->score), 20, 20, 40, RED);
	DrawFPS(SCREEN_WIDTH - 80, 0);

	// Αν το παιχνίδι έχει τελειώσει, σχεδιάζομαι το μήνυμα για να ξαναρχίσει
	if (!state_info(state)->playing) {
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, WHITE
		);
	}
	if (state_info(state)->paused) {
		DrawText(	
			"GAME PAUSED, PRESS [P] TO KEEP PLAYING",
			 GetScreenWidth() / 2 - MeasureText("GAME PAUSED,PRESS [P] TO KEEP PLAYING",15 ) / 2,
			 GetScreenHeight() / 2 - 50, 15, BLACK
		);
	}
	if(frames<=120 && state_info(state)->playing) {
		DrawText(	
			"Will we meet again?",
			 GetScreenWidth() / 2 - MeasureText("Will we meet again?", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, WHITE
		);
	}
	if(frames<=200 && frames >=120 && frames!=0) {
		DrawText(	
			"I'm sure we will",
			 GetScreenWidth() / 2 - MeasureText("Sure we will!", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, WHITE
		);
	}


	UpdateMusicStream(my_music);

	if(state_info(state)->game_over) {
		StopMusicStream(my_music);
		PlaySound(hakuu);

		frames = 0;		
		PlayMusicStream(my_music);
	}
	
	
	if(state_info(state)->paused)
		PauseMusicStream(my_music);
	else
		ResumeMusicStream(my_music);
	

	frames++;
	EndDrawing();
}

