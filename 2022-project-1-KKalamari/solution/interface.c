#include "raylib.h"

#include "state.h"
#include "interface.h"
#include <stdio.h>
Texture Backround,faceless_man,haku,helpers,lanters,terain_left,terain_right,ubaba;
Texture2D Kennen;
Sound game_over;
Sound parry;
Sound jazzynyc;
void interface_init() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "videogame");
    SetTargetFPS(60);
    InitAudioDevice();

	helpers = LoadTextureFromImage(LoadImage("assets!/helpers.png"));
	ubaba = LoadTextureFromImage(LoadImage("assets!/ubaba.png"));
	terain_right = LoadTextureFromImage(LoadImage("assets!/terain_right.png"));
	terain_left = LoadTextureFromImage(LoadImage("assets!/terain_left.png"));
	lanters = LoadTextureFromImage(LoadImage("assets!/lanters.png"));
	haku = LoadTextureFromImage(LoadImage("assets!/haku.png"));
	faceless_man = LoadTextureFromImage(LoadImage("assets!/faceless_man.png"));
	Backround = LoadTextureFromImage(LoadImage("assets!/backround.png"));

    game_over = LoadSound("assets/game_over.mp3");
	parry = LoadSound("assets/sf3_parry.mp3");
	jazzynyc = LoadSound("assets/jazzynycsf3.mp3");
}

void interface_close() {
	UnloadTexture(Backround);
	UnloadTexture(haku);
	UnloadTexture(terain_left);
	UnloadTexture(terain_right);
	UnloadTexture(faceless_man);
	UnloadTexture(ubaba);
	UnloadTexture(lanters);
	UnloadTexture(helpers);

    CloseAudioDevice();
    CloseWindow();
}

int frames = 0;
void interface_draw_frame(State state) {
    BeginDrawing();
    ClearBackground(BLUE);
	//DrawTexture(Backround,0,0,WHITE);
    List items = state_objects(state, state_info(state)->jet->rect.y-4000,state_info(state)->jet->rect.y+2000);
	for(ListNode node = list_first(items); node != LIST_EOF; node = list_next(items,node)) {
			Object item = list_node_value(items,node);
			if (item->type != TERAIN) {
				DrawRectangle(item->rect.x-13,
				    item->rect.y+((state_info(state)->jet->rect.y) * -1) +275,
					item->rect.width,
					item->rect.height,
					BLACK);
			} else {
				DrawRectangle(item->rect.x-13,
					item->rect.y+((state_info(state)->jet->rect.y) * -1) +275,
					item->rect.width,
					item->rect.height,
					GREEN);
			}
		}
    // printf("%d\n",list_size(items));
	if(state_info(state)->missile != NULL) {
		DrawRectangle(state_info(state)->missile->rect.x,
					  300 - (state_info(state)->jet->rect.y + state_info(state)->missile->rect.y * -1) - 20,
					  state_info(state)->missile->rect.width,
					  state_info(state)->missile->rect.height,
					  RED);
	}
	DrawTexture(haku,state_info(state)->jet->rect.x-30,300-36,WHITE);
	DrawCircle(state_info(state)->jet->rect.x, 300,25, RED);
	// DrawTexture(Kennen, state_info(state)->jet->rect.x, 700, WHITE);

    // printf("%f|%f\n",state_info(state)->jet->rect.x,state_info(state)->jet->rect.y);
	if(state_info(state)->missile != NULL) {
		// printf("%f|%f\n",state_info(state)->jet->rect.y,700 - (state_info(state)->jet->rect.y + state_info(state)->missile->rect.y * -1));
	}

	// Σχεδιάζουμε το σκορ και το FPS counter
	DrawText(TextFormat("%04i", state_info(state)->score), 20, 20, 40, RED);
	DrawFPS(SCREEN_WIDTH - 80, 0);

	// Αν το παιχνίδι έχει τελειώσει, σχεδιάζομαι το μήνυμα για να ξαναρχίσει
	if (!state_info(state)->playing) {
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
	}
	if(frames == 1)
		PlaySound(jazzynyc);

	if(state_info(state)->game_over) {
		StopSound(jazzynyc);
		frames = 0;
	}
		// PlaySound(game_over);
	if(state_info(state)->missile_collision)
		PlaySound(parry);

	if(state_info(state)->paused)
		PauseSound(jazzynyc);
	else
		ResumeSound(jazzynyc);
	frames++;
	EndDrawing();
}

