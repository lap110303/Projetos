/*#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

// Quantidade de frames do jogo
const float FPS = 100;

// variáveis globais
int i = 0;
int x = 0;
int playing = 1;
int opcao = 1;
int opcaoataque = 0;
int temp = 10;
int tela = -200;
int tela2 = 0;
int tela3 = 0;
int menu = 1;
int tpokedex, mapa, tcard, bag, tequipe = 0;
int introbatalha = 0;
int batalha = 0;

bool escolhido, selecionado, embatalha = false;
bool som = true;

int maxFrame = 5;
int curFrame = 0;
int frameCount = 0;
int frameDelay = 10;
int frameWidth = 192;
int frameHeight = 192;

// parametros da tela
const int SCREEN_W = 1280;
const int SCREEN_H = 720;

void graficoMenu(ALLEGRO_BITMAP *fundomenu, ALLEGRO_FONT *font, ALLEGRO_FONT *font64, ALLEGRO_SAMPLE_INSTANCE *instancemusicamenu){
	x = 0;
	al_draw_scaled_bitmap(fundomenu, 0, 0, al_get_bitmap_width(fundomenu),
								al_get_bitmap_height(fundomenu),
								0, 0, SCREEN_W, SCREEN_H, 0);
	if(opcao != 1){
		al_draw_textf(font, al_map_rgb(0, 0, 0), 
				SCREEN_W/2, SCREEN_H/2, ALLEGRO_ALIGN_CENTRE, 
				"Pokedex");
	}
	if(opcao != 2){
		al_draw_textf(font, al_map_rgb(0, 0, 0), 
				SCREEN_W/2, SCREEN_H/2 + 70, ALLEGRO_ALIGN_CENTRE, 
				"Equipe");
	}
	if(opcao != 3){
		al_draw_textf(font, al_map_rgb(0, 0, 0), 
				SCREEN_W/2, SCREEN_H/2 + 140, ALLEGRO_ALIGN_CENTRE, 
				"Historico");
	}
	if(opcao != 4){
		al_draw_textf(font, al_map_rgb(0, 0, 0), 
				SCREEN_W/2, SCREEN_H/2 + 210, ALLEGRO_ALIGN_CENTRE, 
				"Mapa");
	}
	if(opcao == 1){
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				SCREEN_W/2 - 10, SCREEN_H/2 - 8, ALLEGRO_ALIGN_CENTRE, 
				"-> Pokedex");
	}
	if(opcao == 2){
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				SCREEN_W/2 - 10, SCREEN_H/2 + 62, ALLEGRO_ALIGN_CENTRE, 
				"-> Equipe");
	}
	if(opcao == 3){
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				SCREEN_W/2 - 10, SCREEN_H/2 + 132, ALLEGRO_ALIGN_CENTRE, 
				"-> Historico");
	}
	if(opcao == 4){
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				SCREEN_W/2 - 10, SCREEN_H/2 + 202, ALLEGRO_ALIGN_CENTRE, 
				"-> Mapa");
	}

	al_set_sample_instance_gain(instancemusicamenu, 0.6);
	al_set_sample_instance_speed(instancemusicamenu, 1);
	al_play_sample_instance(instancemusicamenu);
}

void menuTeclas(ALLEGRO_EVENT ev, ALLEGRO_SAMPLE *escolha){
	if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		opcao++;
		if(opcao > 4){
			opcao = 1;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_W) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		opcao--;
		if(opcao < 1){
			opcao = 4;
		}
	}
	if(ev.keyboard.keycode == ALLEGRO_KEY_ENTER){
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		if(opcao == 1){
			menu = 0;
			tpokedex = 1;
		}
		if(opcao == 2){
			menu = 0;
			tequipe = 1;
		}
		if(opcao == 3){
			menu = 0;
			tcard = 1;
		}
		if(opcao == 4){
			menu = 0;
			mapa = 1;
		}
	}
	if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
		menu = 1;
		playing = 0;
	}
}

void graficoTpokedex (ALLEGRO_BITMAP *pokedex, ALLEGRO_BITMAP *pokemons[], ALLEGRO_SAMPLE *sompokemon[], ALLEGRO_BITMAP *tipos[], ALLEGRO_FONT *font80, ALLEGRO_FONT *font64, ALLEGRO_FONT *font40){
	al_draw_scaled_bitmap(pokedex, 0, 0, al_get_bitmap_width(pokedex),
									 al_get_bitmap_height(pokedex),
									 0, 0, SCREEN_W, SCREEN_H, 0);
        	al_draw_scaled_bitmap(pokemons[x], 0, 0, al_get_bitmap_width(pokemons[x]),
                              	al_get_bitmap_height(pokemons[x]),
                              	100, 90, 400, 400, 0);
			
			if(som == true){
				al_play_sample(sompokemon[x], 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				som = false;
			}

			if(x%2 == 0){
				al_draw_scaled_bitmap(tipos[x], 0, 0, al_get_bitmap_width(tipos[x]),
                              	al_get_bitmap_height(tipos[x]),
                              	513, 417, 96, 43, 0);
			}
			else{
				al_draw_scaled_bitmap(tipos[x], 0, 0, al_get_bitmap_width(tipos[x]),
                              	al_get_bitmap_height(tipos[x]),
                              	513, 397, 96, 43, 0);
				al_draw_scaled_bitmap(tipos[x], 0, 0, al_get_bitmap_width(tipos[x]),
                              	al_get_bitmap_height(tipos[x]),
                              	513, 447, 96, 43, 0);
			}
			al_draw_textf(font64, al_map_rgb(0, 0, 0), 
						830, 87, ALLEGRO_ALIGN_CENTRE, 
						"Numero:   %d", x+1);
			al_draw_textf(font80, al_map_rgb(0, 0, 0), 
						860, 150, ALLEGRO_ALIGN_CENTRE, 
						"NOME:   %d", x);
			al_draw_textf(font40, al_map_rgb(0, 0, 0), 
						1010, 410, ALLEGRO_ALIGN_CENTRE, 
						"HP: %d / Att: %d / Def: %d / Vel: %d", x, x, x, x);
			al_draw_textf(font40, al_map_rgb(0, 0, 0), 
						1010, 345, ALLEGRO_ALIGN_CENTRE, 
						"Pokemon %d", x);
			al_draw_textf(font64, al_map_rgb(0, 0, 0), 
						180, 520, ALLEGRO_ALIGN_LEFT, 
						"Flamethrower");
			al_draw_textf(font64, al_map_rgb(0, 0, 0), 
						180, 610, ALLEGRO_ALIGN_LEFT, 
						"Giga Drain");
			al_draw_textf(font64, al_map_rgb(0, 0, 0), 
						550, 520, ALLEGRO_ALIGN_LEFT, 
						"Surf");
			al_draw_textf(font64, al_map_rgb(0, 0, 0), 
						550, 610, ALLEGRO_ALIGN_LEFT, 
						"Thunder");
			al_draw_textf(font80, al_map_rgb(0, 0, 0), 
						950, 550, ALLEGRO_ALIGN_CENTRE, 
						"Tipo: Inseto");
			if(opcao == 1){
				al_draw_textf(font80, al_map_rgb(0, 0, 0), 
						130, 520, ALLEGRO_ALIGN_CENTRE, 
						"->");
			}
			if(opcao == 2){
				al_draw_textf(font80, al_map_rgb(0, 0, 0), 
						130, 610, ALLEGRO_ALIGN_CENTRE, 
						"->");
			}
			if(opcao == 3){
				al_draw_textf(font80, al_map_rgb(0, 0, 0), 
						500, 520, ALLEGRO_ALIGN_CENTRE, 
						"->");
			}
			if(opcao == 4){
				al_draw_textf(font80, al_map_rgb(0, 0, 0), 
						500, 610, ALLEGRO_ALIGN_CENTRE, 
						"->");
			}
}

void tpokedexTeclas (ALLEGRO_EVENT ev, ALLEGRO_SAMPLE *escolha){
	if (ev.keyboard.keycode == ALLEGRO_KEY_D) {
		x++;
		som = true;
		opcao = 0;
		if(x > 150){
			x = 0;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_A) {
		x--;
		som = true;
		opcao = 0;
		if(x < 0){
			x = 150;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_W) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		opcao--;
		if(opcao < 1){
			opcao = 4;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		opcao++;
		if(opcao > 4){
			opcao = 1;
		}
	}
	if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE){
		if(embatalha == true){
			al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
			tpokedex = 0;
			tequipe = 1;
			opcao = 1;
		}
		else{
			al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
			menu = 1;
			tpokedex = 0;
			opcao = 1;
			x = 0;
		}
	}
}

void graficoMapa (ALLEGRO_BITMAP *fundomapa[], ALLEGRO_BITMAP *caixa, ALLEGRO_FONT *font80, ALLEGRO_FONT *font){
	al_draw_scaled_bitmap(fundomapa[x], 0, 0, al_get_bitmap_width(fundomapa[x]),
									 al_get_bitmap_height(fundomapa[x]),
									 0, 0, SCREEN_W, SCREEN_H, 0);
	al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				180, 20, ALLEGRO_ALIGN_CENTRE, 
				"Ginasio: %d", x);
	al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				180, 120, ALLEGRO_ALIGN_CENTRE, 
				"Tipo: %d", x);
	al_draw_scaled_bitmap(caixa, 0, 0, al_get_bitmap_width(caixa),
								al_get_bitmap_height(caixa),
								0, 470, 400, 200, 0);
	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				200, 480, ALLEGRO_ALIGN_CENTRE, 
				"Ginasio desbloqueado", x);
	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				200, 530, ALLEGRO_ALIGN_CENTRE, 
				"Iniciar batalha?", x);
	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				100, 600, ALLEGRO_ALIGN_CENTRE, 
				"Sim", x);
	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				300, 600, ALLEGRO_ALIGN_CENTRE, 
				"Nao", x);
	if(opcao == 1){
		al_draw_textf(font, al_map_rgb(0, 0, 0), 
				50, 600, ALLEGRO_ALIGN_CENTRE, 
				"->", x);
	}
	if(opcao == 2){
		al_draw_textf(font, al_map_rgb(0, 0, 0), 
				250, 600, ALLEGRO_ALIGN_CENTRE, 
				"->", x);
	}
}

void mapaTeclas(ALLEGRO_EVENT ev, ALLEGRO_SAMPLE *escolha, ALLEGRO_SAMPLE_INSTANCE *instancemusicamenu){
	if (ev.keyboard.keycode == ALLEGRO_KEY_W) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		x++;
		if(x > 4){
			x = 4;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		x--;
		if(x < 0){
			x = 0;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_D) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		opcao++;
		if(opcao > 2){
			opcao = 1;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_A) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		opcao--;
		if(opcao < 1){
			opcao = 2;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		if(opcao == 1){
			mapa = 0;
			introbatalha = 1;
			al_stop_sample_instance(instancemusicamenu);
		}
		else {
			opcao = 0;
		}
	}
	if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE){
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		menu = 1;
		mapa = 0;
		x = 0;
	}
}

void graficoTcard (ALLEGRO_BITMAP *cards[], ALLEGRO_FONT *font80, ALLEGRO_FONT *font64, ALLEGRO_FONT *font){
	al_draw_scaled_bitmap(cards[x], 0, 0, al_get_bitmap_width(cards[x]),
								al_get_bitmap_height(cards[x]),
								0, 0, SCREEN_W, SCREEN_H, 0);
	al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				980, 60, ALLEGRO_ALIGN_CENTRE, 
				"Nome");
	al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				120, 150, ALLEGRO_ALIGN_LEFT, 
				"Status: Desafiante");
	al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				360, 230, ALLEGRO_ALIGN_CENTRE, 
				"Historico das 3 ultimas batalhas:");
	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				120, 300, ALLEGRO_ALIGN_LEFT, 
				"Vitoria vs Treinador Agua");
	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				120, 375, ALLEGRO_ALIGN_LEFT, 
				"Derrota vs Campeao");
	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				120, 454, ALLEGRO_ALIGN_LEFT, 
				"Vitoria vs Treinador Terra");
}

void tcardTeclas (ALLEGRO_EVENT ev, ALLEGRO_SAMPLE *escolha){
	if (ev.keyboard.keycode == ALLEGRO_KEY_D) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		x++;
		if(x > 4){
			x = 4;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_A) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		x--;
		if(x < 0){
			x = 0;
		}
	}
	if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE){
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		menu = 1;
		tcard = 0;
		x = 0;
	}
}

void graficoBag (ALLEGRO_BITMAP *itens[], ALLEGRO_FONT *font80, ALLEGRO_FONT *font64){
	al_draw_scaled_bitmap(itens[x], 0, 0, al_get_bitmap_width(itens[x]),
								al_get_bitmap_height(itens[x]),
								0, 0, SCREEN_W, SCREEN_H, 0);
	al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				230, 40, ALLEGRO_ALIGN_CENTRE, 
				"Itens");
	al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				550, 55, ALLEGRO_ALIGN_LEFT, 
				"Potion");
	al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				550, 128, ALLEGRO_ALIGN_LEFT, 
				"Superpotion");
	al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				550, 199, ALLEGRO_ALIGN_LEFT, 
				"Hiperpotion");
	al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				550, 272, ALLEGRO_ALIGN_LEFT, 
				"Full Restore");
	al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				550, 344, ALLEGRO_ALIGN_LEFT, 
				"Maxpotion");
	al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				950, 55, ALLEGRO_ALIGN_LEFT, 
				"Revive");
	al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				950, 128, ALLEGRO_ALIGN_LEFT, 
				"Full Heal");
	al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				950, 199, ALLEGRO_ALIGN_LEFT, 
				"X-Attack");
	al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				950, 272, ALLEGRO_ALIGN_LEFT, 
				"X-Defense");
	al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				950, 344, ALLEGRO_ALIGN_LEFT, 
				"X-Speed");
	if(opcao == 1){
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				500, 55, ALLEGRO_ALIGN_LEFT, 
				"->");
		al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				210, 570, ALLEGRO_ALIGN_LEFT, 
				"Restaura a vida do Pokemon em 20.");
	}
	if(opcao == 2){
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				500, 128, ALLEGRO_ALIGN_LEFT, 
				"->");
		al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				210, 570, ALLEGRO_ALIGN_LEFT, 
				"Restaura a vida do Pokemon em 50.");
	}
	if(opcao == 3){
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				500, 199, ALLEGRO_ALIGN_LEFT, 
				"->");
		al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				210, 570, ALLEGRO_ALIGN_LEFT, 
				"Restaura a vida do Pokemon em 200.");
	}
	if(opcao == 4){
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				500, 272, ALLEGRO_ALIGN_LEFT, 
				"->");
		al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				210, 570, ALLEGRO_ALIGN_LEFT, 
				"Restaura toda a vida do Pokemon.");
	}
	if(opcao == 5){
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				500, 344, ALLEGRO_ALIGN_LEFT, 
				"->");
		al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				210, 570, ALLEGRO_ALIGN_LEFT, 
				"Restaura a vida e remove efeitos.");
	}
	if(opcao == 6){
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				900, 55, ALLEGRO_ALIGN_LEFT, 
				"->");
		al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				210, 570, ALLEGRO_ALIGN_LEFT, 
				"Revive o Pokemon.");
	}
	if(opcao == 7){
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				900, 128, ALLEGRO_ALIGN_LEFT, 
				"->");
		al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				210, 570, ALLEGRO_ALIGN_LEFT, 
				"Remove efeitos.");
	}
	if(opcao == 8){
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				900, 199, ALLEGRO_ALIGN_LEFT, 
				"->");
		al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				210, 570, ALLEGRO_ALIGN_LEFT, 
				"Aumenta o ataque em 40.");
	}
	if(opcao == 9){
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				900, 272, ALLEGRO_ALIGN_LEFT, 
				"->");
		al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				210, 570, ALLEGRO_ALIGN_LEFT, 
				"Aumenta a defesa em 40.");
	}
	if(opcao == 10){
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				900, 344, ALLEGRO_ALIGN_LEFT, 
				"->");
		al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				210, 570, ALLEGRO_ALIGN_LEFT, 
				"Aumenta a velocidade em 40.");
	}
}

void teclasBag (ALLEGRO_EVENT ev, ALLEGRO_SAMPLE *escolha){
	if (ev.keyboard.keycode == ALLEGRO_KEY_W) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		x--;
		opcao--;
		if(x < 0){
			x = 9;
			opcao = 10;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		x++;
		opcao++;
		if(x > 9){
			x = 0;
			opcao = 1;
		}
	}
	if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE){
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		bag = 0;
		batalha = 1;
		opcao = 0;
	}
}

void graficoTequipe (ALLEGRO_BITMAP *equipe[], ALLEGRO_BITMAP *pokemonsi[], ALLEGRO_FONT *font80, ALLEGRO_FONT *font){
	al_draw_scaled_bitmap(equipe[x], 0, 0, al_get_bitmap_width(equipe[x]),
								al_get_bitmap_height(equipe[x]),
								0, 0, SCREEN_W, SCREEN_H, 0);
	al_draw_scaled_bitmap(pokemonsi[0], 0, 0, al_get_bitmap_width(pokemonsi[0]),
						al_get_bitmap_height(pokemonsi[0]),
						15, 85, 100, 80, 0);
	al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				140, 155, ALLEGRO_ALIGN_LEFT, 
				"Nome");
	al_draw_scaled_bitmap(pokemonsi[1], 0, 0, al_get_bitmap_width(pokemonsi[1]),
						al_get_bitmap_height(pokemonsi[1]),
						475, 47, 100, 80, 0);
	al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				600, 47, ALLEGRO_ALIGN_LEFT, 
				"Nome");
	al_draw_scaled_bitmap(pokemonsi[2], 0, 0, al_get_bitmap_width(pokemonsi[2]),
						al_get_bitmap_height(pokemonsi[2]),
						475, 155, 100, 80, 0);
	al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				600, 155, ALLEGRO_ALIGN_LEFT, 
				"Nome");
	al_draw_scaled_bitmap(pokemonsi[3], 0, 0, al_get_bitmap_width(pokemonsi[3]),
						al_get_bitmap_height(pokemonsi[3]),
						475, 263, 100, 80, 0);
	al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				600, 263, ALLEGRO_ALIGN_LEFT, 
				"Nome");
	al_draw_scaled_bitmap(pokemonsi[4], 0, 0, al_get_bitmap_width(pokemonsi[4]),
						al_get_bitmap_height(pokemonsi[4]),
						475, 372, 100, 80, 0);
	al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				600, 372, ALLEGRO_ALIGN_LEFT, 
				"Nome");
	al_draw_scaled_bitmap(pokemonsi[5], 0, 0, al_get_bitmap_width(pokemonsi[5]),
						al_get_bitmap_height(pokemonsi[5]),
						475, 480, 100, 80, 0);
	al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				600, 480, ALLEGRO_ALIGN_LEFT, 
				"Nome");


	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				200, 620, ALLEGRO_ALIGN_CENTER, 
				"Ver pokemon");
	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				500, 620, ALLEGRO_ALIGN_CENTER, 
				"Trocar posicao");
	if(opcao == 1){
		al_draw_textf(font, al_map_rgb(0, 0, 0), 
				85, 620, ALLEGRO_ALIGN_CENTER, 
				"->");
	}
	if(opcao == 2){
		al_draw_textf(font, al_map_rgb(0, 0, 0), 
				365, 620, ALLEGRO_ALIGN_CENTER, 
				"->");
	}


	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				420, 288, ALLEGRO_ALIGN_RIGHT, 
				"0/100");
	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				1235, 100, ALLEGRO_ALIGN_RIGHT, 
				"0/100");
	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				1235, 208, ALLEGRO_ALIGN_RIGHT, 
				"0/100");
	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				1235, 316, ALLEGRO_ALIGN_RIGHT, 
				"0/100");
	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				1235, 424, ALLEGRO_ALIGN_RIGHT, 
				"0/100");
	al_draw_textf(font, al_map_rgb(0, 0, 0), 
				1235, 532, ALLEGRO_ALIGN_RIGHT, 
				"0/100");

	double porcentagemvida = 100;
	int barra;
	barra = 256 * porcentagemvida / 100;

	if(porcentagemvida > 50){
		al_draw_filled_rectangle(171, 266, 171 + barra, 279, al_map_rgb(0, 255, 0));
		al_draw_filled_rectangle(171, 266, 171 + barra, 270, al_map_rgb(0, 150, 0));

		al_draw_filled_rectangle(981, 81, 981 + barra, 95, al_map_rgb(0, 255, 0));
		al_draw_filled_rectangle(981, 81, 981 + barra, 85, al_map_rgb(0, 150, 0));

		al_draw_filled_rectangle(981, 189, 981 + barra, 203, al_map_rgb(0, 255, 0));
		al_draw_filled_rectangle(981, 189, 981 + barra, 193, al_map_rgb(0, 150, 0));

		al_draw_filled_rectangle(981, 297, 981 + barra, 311, al_map_rgb(0, 255, 0));
		al_draw_filled_rectangle(981, 297, 981 + barra, 301, al_map_rgb(0, 150, 0));

		al_draw_filled_rectangle(981, 405, 981 + barra, 419, al_map_rgb(0, 255, 0));
		al_draw_filled_rectangle(981, 405, 981 + barra, 409, al_map_rgb(0, 150, 0));

		al_draw_filled_rectangle(981, 513, 981 + barra, 527, al_map_rgb(0, 255, 0));
		al_draw_filled_rectangle(981, 513, 981 + barra, 517, al_map_rgb(0, 150, 0));
	}

	if(porcentagemvida <= 50 && porcentagemvida > 20){
		al_draw_filled_rectangle(171, 266, 171 + barra, 279, al_map_rgb(255, 255, 0));
		al_draw_filled_rectangle(171, 266, 171 + barra, 270, al_map_rgb(204, 204, 0));
		
		al_draw_filled_rectangle(981, 81, 981 + barra, 95, al_map_rgb(255, 255, 0));
		al_draw_filled_rectangle(981, 81, 981 + barra, 85, al_map_rgb(204, 204, 0));

		al_draw_filled_rectangle(981, 189, 981 + barra, 203, al_map_rgb(255, 255, 0));
		al_draw_filled_rectangle(981, 189, 981 + barra, 193, al_map_rgb(204, 204, 0));

		al_draw_filled_rectangle(981, 297, 981 + barra, 311, al_map_rgb(255, 255, 0));
		al_draw_filled_rectangle(981, 297, 981 + barra, 301, al_map_rgb(204, 204, 0));

		al_draw_filled_rectangle(981, 405, 981 + barra, 419, al_map_rgb(255, 255, 0));
		al_draw_filled_rectangle(981, 405, 981 + barra, 409, al_map_rgb(204, 204, 0));

		al_draw_filled_rectangle(981, 513, 981 + barra, 527, al_map_rgb(255, 255, 0));
		al_draw_filled_rectangle(981, 513, 981 + barra, 517, al_map_rgb(204, 204, 0));
	}

	if(porcentagemvida <= 20){
		al_draw_filled_rectangle(171, 266, 171 + barra, 279, al_map_rgb(255, 0, 0));
		al_draw_filled_rectangle(171, 266, 171 + barra, 270, al_map_rgb(100, 0, 0));
		
		al_draw_filled_rectangle(981, 81, 981 + barra, 95, al_map_rgb(255, 0, 0));
		al_draw_filled_rectangle(981, 81, 981 + barra, 85, al_map_rgb(100, 0, 0));

		al_draw_filled_rectangle(981, 189, 981 + barra, 203, al_map_rgb(255, 0, 0));
		al_draw_filled_rectangle(981, 189, 981 + barra, 193, al_map_rgb(150, 0, 0));

		al_draw_filled_rectangle(981, 297, 981 + barra, 311, al_map_rgb(255, 0, 0));
		al_draw_filled_rectangle(981, 297, 981 + barra, 301, al_map_rgb(100, 0, 0));

		al_draw_filled_rectangle(981, 405, 981 + barra, 419, al_map_rgb(255, 0, 0));
		al_draw_filled_rectangle(981, 405, 981 + barra, 409, al_map_rgb(100, 0, 0));

		al_draw_filled_rectangle(981, 513, 981 + barra, 527, al_map_rgb(255, 0, 0));
		al_draw_filled_rectangle(981, 513, 981 + barra, 517, al_map_rgb(100, 0, 0));
	}
}

void teclasTequipe (ALLEGRO_EVENT ev, ALLEGRO_SAMPLE *escolha){
	if (ev.keyboard.keycode == ALLEGRO_KEY_W) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		x--;
		if(x < 0){
			x = 0;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		x++;
		if(x > 5){
			x = 5;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_D) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		opcao++;
		if(opcao > 2){
			opcao = 1;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_A) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		opcao--;
		if(opcao < 1){
			opcao = 2;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		if(opcao == 1){
			tequipe = 0;
			tpokedex = 1;
		}
		if(opcao == 2){
			//trocar as posiçoes do vector
		}
	}
	if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE){
		if(embatalha == false){
			al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
			menu = 1;
			tequipe = 0;
			x = 0;
		}
		else{
			al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
			batalha = 1;
			tequipe = 0;
		}
	}
}

void graficoIntroBatalha (ALLEGRO_BITMAP *fundobatalha, ALLEGRO_BITMAP *barra, ALLEGRO_BITMAP *barraop, ALLEGRO_BITMAP *vs, ALLEGRO_BITMAP *barraeu, ALLEGRO_BITMAP *treinador, ALLEGRO_BITMAP *rivais[], ALLEGRO_FONT *font80, ALLEGRO_SAMPLE_INSTANCE *instancemusicabatalha){
	al_set_sample_instance_gain(instancemusicabatalha, 0.4);
	al_set_sample_instance_speed(instancemusicabatalha, 1);
	al_play_sample_instance(instancemusicabatalha);
	tela++;
	if(tela == 360){
		introbatalha = 0;
		batalha = 1;
		embatalha = true;
		tela = 0;
	}
	
	al_draw_scaled_bitmap(fundobatalha, 0, 0, al_get_bitmap_width(fundobatalha),
								al_get_bitmap_height(fundobatalha),
								0, 0, SCREEN_W, SCREEN_H, 0);
	al_draw_scaled_bitmap(barra, 0, 0, al_get_bitmap_width(barra),
								al_get_bitmap_height(barra),
								0, 0, SCREEN_W, SCREEN_H/2 - tela, 0);
	al_draw_scaled_bitmap(barra, 0, 0, al_get_bitmap_width(barra),
								al_get_bitmap_height(barra),
								0, SCREEN_H/2 + tela, SCREEN_W, SCREEN_H, 0);
	al_draw_scaled_bitmap(barraop, 0, 0, al_get_bitmap_width(barraop),
								al_get_bitmap_height(barraop),
								0, 100, SCREEN_W, 200, 0);
	al_draw_scaled_bitmap(vs, 0, 0, al_get_bitmap_width(vs),
								al_get_bitmap_height(vs),
								600, 320, 80, 80, 0);
	al_draw_scaled_bitmap(barraeu, 0, 0, al_get_bitmap_width(barraeu),
								al_get_bitmap_height(barraeu),
								0, 420, SCREEN_W, 200, 0);
	al_draw_scaled_bitmap(treinador, 0, 0, al_get_bitmap_width(treinador),
						al_get_bitmap_height(treinador),
						220, 420, 200, 200, 0);
	al_draw_scaled_bitmap(rivais[x], 0, 0, al_get_bitmap_width(rivais[x]),
						al_get_bitmap_height(rivais[x]),
						860, 100, 200, 200, 0);
	al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				SCREEN_W/2, 160, ALLEGRO_ALIGN_CENTER, 
				"Cinthya");
	al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				SCREEN_W/2, 480, ALLEGRO_ALIGN_CENTER, 
				"Lucas");
}

void graficoBatalha (ALLEGRO_EVENT ev, ALLEGRO_BITMAP *fundobatalha, ALLEGRO_BITMAP *barraopcao, ALLEGRO_BITMAP *hp, ALLEGRO_BITMAP *hp2, ALLEGRO_BITMAP *sem, ALLEGRO_FONT *font, ALLEGRO_BITMAP *opcoes, ALLEGRO_BITMAP *treinadorcosta, ALLEGRO_BITMAP *pokebola, ALLEGRO_BITMAP *pokebolaa, ALLEGRO_BITMAP *pokemons[], ALLEGRO_BITMAP *pokemonsb[], ALLEGRO_BITMAP *animacaoataques[], ALLEGRO_BITMAP *barra, ALLEGRO_BITMAP *barraop, ALLEGRO_BITMAP *vs, ALLEGRO_BITMAP *barraeu, ALLEGRO_BITMAP *treinador, ALLEGRO_BITMAP *rivais[], ALLEGRO_FONT *font80, ALLEGRO_FONT *font64, ALLEGRO_SAMPLE_INSTANCE *instancemusicabatalha, ALLEGRO_SAMPLE *sompokemon[]){
	tela += 2;
	al_draw_scaled_bitmap(fundobatalha, 0, 0, al_get_bitmap_width(fundobatalha),
								al_get_bitmap_height(fundobatalha),
								0, 0, SCREEN_W, SCREEN_H, 0);
	al_draw_scaled_bitmap(treinadorcosta, 0, 0, al_get_bitmap_width(treinadorcosta),
						al_get_bitmap_height(treinadorcosta),
						220 - tela2, 305, 200, 200, 0);
	al_draw_scaled_bitmap(rivais[x], 0, 0, al_get_bitmap_width(rivais[x]),
						al_get_bitmap_height(rivais[x]),
						850 + tela, 100, 200, 200, 0);
	al_draw_scaled_bitmap(pokebola, 0, 0, al_get_bitmap_width(pokebola),
						al_get_bitmap_height(pokebola),
						920, 250, 45, 90, 0);
	al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				105, 569, ALLEGRO_ALIGN_LEFT, 
				"Cynthia enviou Poliwrath!!");
	al_draw_textf(font80, al_map_rgb(255, 255, 255), 
				100, 564, ALLEGRO_ALIGN_LEFT, 
				"Cynthia enviou Poliwrath!!");

	if(tela > 400){
		al_draw_scaled_bitmap(pokebolaa, 0, 0, al_get_bitmap_width(pokebolaa),
						al_get_bitmap_height(pokebolaa),
						920, 250, 45, 90, 0);
	}
	if(tela == 420){
		som = true;
		if(som == true){
			al_play_sample(sompokemon[60], 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
			som = false;
		}
	}
	if(tela > 420){
		tela2 += 2;
		al_draw_scaled_bitmap(fundobatalha, 0, 0, al_get_bitmap_width(fundobatalha),
								al_get_bitmap_height(fundobatalha),
								0, 0, SCREEN_W, SCREEN_H, 0);
		al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				105, 569, ALLEGRO_ALIGN_LEFT, 
				"Sua vez Poliwag!!");
		al_draw_textf(font80, al_map_rgb(255, 255, 255), 
				100, 564, ALLEGRO_ALIGN_LEFT, 
				"Sua vez Poliwag!!");
		al_draw_scaled_bitmap(treinadorcosta, 0, 0, al_get_bitmap_width(treinadorcosta),
							al_get_bitmap_height(treinadorcosta),
							220 - tela2, 305, 200, 200, 0);
		al_draw_scaled_bitmap(pokebola, 0, 0, al_get_bitmap_width(pokebola),
						al_get_bitmap_height(pokebola),
						300, 440, 45, 90, 0);
		al_draw_scaled_bitmap(pokemons[60], 0, 0, al_get_bitmap_width(pokemons[60]),
						al_get_bitmap_height(pokemons[60]),
						805, 105, 250, 250, 0);
	}
	if(tela2 > 400){
		al_draw_scaled_bitmap(pokebolaa, 0, 0, al_get_bitmap_width(pokebolaa),
						al_get_bitmap_height(pokebolaa),
						300, 440, 45, 90, 0);
	}
	if(tela2 == 420){
		som = true;
		if(som == true){
			al_play_sample(sompokemon[59], 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
			som = false;
		}
	}
	if(tela2 > 420){
		tela3 += 3;
		al_draw_scaled_bitmap(fundobatalha, 0, 0, al_get_bitmap_width(fundobatalha),
								al_get_bitmap_height(fundobatalha),
								0, 0, SCREEN_W, SCREEN_H, 0);
		al_draw_scaled_bitmap(pokemons[60], 0, 0, al_get_bitmap_width(pokemons[60]),
						al_get_bitmap_height(pokemons[60]),
						805, 105, 250, 250, 0);
		al_draw_scaled_bitmap(pokemonsb[59], 0, 0, al_get_bitmap_width(pokemonsb[59]),
						al_get_bitmap_height(pokemonsb[59]),
						205, 320, 250, 250, 0);
		al_draw_scaled_bitmap(barraopcao, 0, 0, al_get_bitmap_width(barraopcao),
								al_get_bitmap_height(barraopcao),
								0, 504, SCREEN_W, 214, 0);
		al_draw_scaled_bitmap(opcoes, 0, 0, al_get_bitmap_width(opcoes),
								al_get_bitmap_height(opcoes),
								780, 504, 500, 214, 0);
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				925, 544, ALLEGRO_ALIGN_CENTER, 
				"Lutar");
		if(opcao == 1){
			al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				835, 549, ALLEGRO_ALIGN_CENTER, 
				"->");
		}
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				1125, 544, ALLEGRO_ALIGN_CENTER, 
				"Mochila");
		if(opcao == 2){
			al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				1025, 549, ALLEGRO_ALIGN_CENTER, 
				"->");
		}
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				1025, 604, ALLEGRO_ALIGN_CENTER, 
				"Equipe");
		if(opcao == 3){
			al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				925, 609, ALLEGRO_ALIGN_CENTER, 
				"->");
		}
		al_draw_textf(font80, al_map_rgb(0, 0, 0), 
				105, 569, ALLEGRO_ALIGN_LEFT, 
				"Seu turno, o que fazer??");
		al_draw_textf(font80, al_map_rgb(255, 255, 255), 
				100, 564, ALLEGRO_ALIGN_LEFT, 
				"Seu turno, o que fazer??");
		al_draw_scaled_bitmap(hp, 0, 0, al_get_bitmap_width(hp),
								al_get_bitmap_height(hp),
								1280 - tela3, 384, 500, 120, 0);
		al_draw_scaled_bitmap(hp2, 0, 0, al_get_bitmap_width(hp2),
								al_get_bitmap_height(hp2),
								-500 + tela3, 80, 500, 120, 0);
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				1640 - tela3, 394, ALLEGRO_ALIGN_CENTER, 
				"Poliwrath");
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
				-180 + tela3, 90, ALLEGRO_ALIGN_CENTER, 
				"Poliwag");
		al_draw_textf(font, al_map_rgb(0, 0, 0), 
				1405 - tela3, 444, ALLEGRO_ALIGN_CENTER, 
				"100/100");
		al_draw_textf(font, al_map_rgb(0, 0, 0), 
				-410 + tela3, 140, ALLEGRO_ALIGN_CENTER, 
				"100/100");
		for(i = 0; i < 6; i++){
			al_draw_scaled_bitmap(sem, 0, 0, al_get_bitmap_width(sem),
								al_get_bitmap_height(sem),
								-480 + tela3 + i*25, 110, 15, 15, 0);
		}
		for(i = 0; i < 6; i++){
			al_draw_scaled_bitmap(sem, 0, 0, al_get_bitmap_width(sem),
								al_get_bitmap_height(sem),
								1350 - tela3 + i*25, 414, 15, 15, 0);
		}

		double porcentagemvida = 100;
		int barra;
		barra = 185 * porcentagemvida / 100;

		if(porcentagemvida > 50){
			al_draw_filled_rectangle(-273 + tela3, 157, -273 + tela3 + barra, 169, al_map_rgb(0, 255, 0));
			al_draw_filled_rectangle(-273 + tela3, 157, -273 + tela3 + barra, 161, al_map_rgb(0, 150, 0));
		}

		if(porcentagemvida <= 50 && porcentagemvida > 20){
			al_draw_filled_rectangle(-273 + tela3, 157, -273 + tela3 + barra, 169, al_map_rgb(255, 255, 0));
			al_draw_filled_rectangle(-273 + tela3, 157, -273 + tela3 + barra, 161, al_map_rgb(204, 204, 0));
		}

		if(porcentagemvida <= 20){
			al_draw_filled_rectangle(-273 + tela3, 157, -273 + tela3 + barra, 169, al_map_rgb(255, 0, 0));
			al_draw_filled_rectangle(-273 + tela3, 157, -273 + tela3 + barra, 161, al_map_rgb(100, 0, 0));
		}


		if(porcentagemvida > 50){
			al_draw_filled_rectangle(1541 - tela3, 461, 1541 - tela3 + barra, 473, al_map_rgb(0, 255, 0));
			al_draw_filled_rectangle(1541 - tela3, 461, 1541 - tela3 + barra, 465, al_map_rgb(0, 150, 0));
		}

		if(porcentagemvida <= 50 && porcentagemvida > 20){
			al_draw_filled_rectangle(1541 - tela3, 461, 1541 - tela3 + barra, 473, al_map_rgb(255, 255, 0));
			al_draw_filled_rectangle(1541 - tela3, 461, 1541 - tela3 + barra, 465, al_map_rgb(204, 204, 0));
		}

		if(porcentagemvida <= 20){
			al_draw_filled_rectangle(1541 - tela3, 461, 1541 - tela3 + barra, 473, al_map_rgb(255, 0, 0));
			al_draw_filled_rectangle(1541 - tela3, 461, 1541 - tela3 + barra, 465, al_map_rgb(100, 0, 0));
		}
	}

	if(opcao == 1 && escolhido == true){
		al_draw_scaled_bitmap(opcoes, 0, 0, al_get_bitmap_width(opcoes),
					al_get_bitmap_height(opcoes),
					0, 504, 780, 214, 0);
		al_draw_scaled_bitmap(opcoes, 0, 0, al_get_bitmap_width(opcoes),
					al_get_bitmap_height(opcoes),
					780, 504, 500, 214, 0);
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
					110, 540, ALLEGRO_ALIGN_LEFT, 
					"Flamethrower");
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
					110, 610, ALLEGRO_ALIGN_LEFT, 
					"Giga Drain");
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
					550, 540, ALLEGRO_ALIGN_LEFT, 
					"Surf");
		al_draw_textf(font64, al_map_rgb(0, 0, 0), 
					550, 610, ALLEGRO_ALIGN_LEFT, 
					"Thunder");
		if(opcaoataque == 1){
			al_draw_textf(font64, al_map_rgb(0, 0, 0), 
					70, 545, ALLEGRO_ALIGN_CENTRE, 
					"->");
		}
		if(opcaoataque == 2){
			al_draw_textf(font64, al_map_rgb(0, 0, 0), 
					510, 545, ALLEGRO_ALIGN_CENTRE, 
					"->");
		}
		if(opcaoataque == 3){
			al_draw_textf(font64, al_map_rgb(0, 0, 0), 
					70, 615, ALLEGRO_ALIGN_CENTRE, 
					"->");
		}
		if(opcaoataque == 4){
			al_draw_textf(font64, al_map_rgb(0, 0, 0), 
					510, 615, ALLEGRO_ALIGN_CENTRE, 
					"->");
		}
		al_draw_textf(font80, al_map_rgb(0, 0, 0), 
					1030, 530, ALLEGRO_ALIGN_CENTRE, 
					"Tipo: Inseto");
		al_draw_textf(font80, al_map_rgb(0, 0, 0), 
					1030, 615, ALLEGRO_ALIGN_CENTRE, 
					"Dano: 90");
		if(opcaoataque == 1 && selecionado == true){
			som = true;
			if(ev.type == ALLEGRO_EVENT_TIMER){
				if(++frameCount >= frameDelay){
					if(++curFrame >= maxFrame){
						escolhido = false;
						selecionado = false;
						curFrame = 0;
					}
					frameCount = 0;
				}
				
				al_draw_bitmap_region(animacaoataques[x], curFrame * frameWidth, 0, frameWidth,
					frameHeight, 835, 145, 0);
			}
		}
		if(opcaoataque == 2 && selecionado == true){
			som = true;
			if(ev.type == ALLEGRO_EVENT_TIMER){
				if(++frameCount >= frameDelay){
					if(++curFrame >= maxFrame){
						escolhido = false;
						selecionado = false;
						curFrame = 0;
					}
					frameCount = 0;
				}
				
				al_draw_bitmap_region(animacaoataques[x], curFrame * frameWidth, 0, frameWidth,
					frameHeight, 835, 145, 0);
			}
		}
		if(opcaoataque == 3 && selecionado == true){
			som = true;
			if(ev.type == ALLEGRO_EVENT_TIMER){
				if(++frameCount >= frameDelay){
					if(++curFrame >= maxFrame){
						escolhido = false;
						selecionado = false;
						curFrame = 0;
					}
					frameCount = 0;
				}

				al_draw_bitmap_region(animacaoataques[x], curFrame * frameWidth, 0, frameWidth,
					frameHeight, 835, 145, 0);
			}
		}
		if(opcaoataque == 4 && selecionado == true){
			if(ev.type == ALLEGRO_EVENT_TIMER){
				if(++frameCount >= frameDelay){
					if(++curFrame >= maxFrame){
						escolhido = false;
						selecionado = false;
						curFrame = 0;
					}
					frameCount = 0;
				}

				al_draw_bitmap_region(animacaoataques[x], curFrame * frameWidth, 0, frameWidth,
					frameHeight, 835, 145, 0);
			}
		}
	}

	if(tela3 > 495){
		tela3 = 495;
	}
}

void teclasBatalha (ALLEGRO_EVENT ev, ALLEGRO_SAMPLE *escolha, ALLEGRO_SAMPLE *somataques[]){
	if (ev.keyboard.keycode == ALLEGRO_KEY_W) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		x--;
		if(x < 0){
			x = 0;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		x++;
		if(x > 6){
			x = 6;
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_D) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		if(escolhido == false){
			opcao++;
			if(opcao > 3){
				opcao = 1;
			}
		}
		if(escolhido == true){
			opcaoataque++;
			if(opcaoataque > 4){
				opcaoataque = 1;
			}
		}
	}
	if (ev.keyboard.keycode == ALLEGRO_KEY_A) {
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		if(escolhido == false){
			opcao--;
			if(opcao < 1){
				opcao = 3;
			}
		}
		if(escolhido == true){
			opcaoataque--;
			if(opcaoataque < 1){
				opcaoataque = 4;
			}
		}
	}
	if(ev.keyboard.keycode == ALLEGRO_KEY_ENTER){
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		if(opcao == 1 && escolhido != true){
			escolhido = true;
			opcaoataque = 0;
		}
		if(opcao == 2){
			al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
			batalha = 0;
			bag = 1;
			opcao = 1;
			x = 0;
		}
		if(opcao == 3){
			al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
			batalha = 0;
			tequipe = 1;
			opcao = 1;
			x = 0;
		}
		if (escolhido == true && opcaoataque == 1){
			selecionado = true;
			x = 0;
			al_play_sample(somataques[x + 1], 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		}
		if (escolhido == true && opcaoataque == 2){
			selecionado = true;
			x = 1;
			al_play_sample(somataques[x + 1], 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		}
		if (escolhido == true && opcaoataque == 3){
			selecionado = true;
			x = 2;
			al_play_sample(somataques[x + 1], 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		}
		if (escolhido == true && opcaoataque == 4){
			selecionado = true;
			x = 3;
			al_play_sample(somataques[x + 1], 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		}
	}
	if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE){
		al_play_sample(escolha, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		escolhido = false;
	}
}


int main(int argc, char **argv){
	
	// inicializações padrão
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	// inicializações de diferentes tamanhos de fonte
	ALLEGRO_FONT *font = NULL;
	ALLEGRO_FONT *font40 = NULL;
	ALLEGRO_FONT *font64 = NULL;
	ALLEGRO_FONT *font80 = NULL;

	// imagem de imagens
	ALLEGRO_BITMAP *fundomenu = NULL;
	ALLEGRO_BITMAP *caixa = NULL;
	ALLEGRO_BITMAP *pokedex = NULL;
	ALLEGRO_BITMAP *barra = NULL;
	ALLEGRO_BITMAP *barraeu = NULL;
	ALLEGRO_BITMAP *barraop = NULL;
	ALLEGRO_BITMAP *barraopcao = NULL;
	ALLEGRO_BITMAP *vs = NULL;
	ALLEGRO_BITMAP *fundobatalha = NULL;
	ALLEGRO_BITMAP *treinador = NULL;
	ALLEGRO_BITMAP *treinadorcosta = NULL;
	ALLEGRO_BITMAP *pokebola = NULL;
	ALLEGRO_BITMAP *pokebolaa = NULL;
	ALLEGRO_BITMAP *hp = NULL;
	ALLEGRO_BITMAP *hp2 = NULL;
	ALLEGRO_BITMAP *vivo = NULL;
	ALLEGRO_BITMAP *desmaiado = NULL;
	ALLEGRO_BITMAP *sem = NULL;
	ALLEGRO_BITMAP *opcoes = NULL;

	// inicialização de arrays de imagens
	ALLEGRO_BITMAP *animacaoataques[17]; // sprites dos ataques
	ALLEGRO_BITMAP *pokemons[151];	// sprites dos pokemons
	ALLEGRO_BITMAP *pokemonsi[151]; // icones dos pokemons
	ALLEGRO_BITMAP *pokemonsb[151];	// costas dos pokemons
	ALLEGRO_BITMAP *tipos[18];	// tipos de pokemon
	ALLEGRO_BITMAP *efeitos[6];	// efeitos de batalha
	ALLEGRO_BITMAP *fundomapa[5]; // diferentes telas de mapa
	ALLEGRO_BITMAP *cards[5];	// diferentes telas de insignia
	ALLEGRO_BITMAP *itens[10];	// diferentes telas de itens
	ALLEGRO_BITMAP *equipe[6]; // diferentes telas de equipe
	ALLEGRO_BITMAP *rivais[7];	// telas de batalha contra todos os oponentes

	
	// sons do jogo
	ALLEGRO_SAMPLE *escolha = NULL;
	ALLEGRO_SAMPLE *comecajogo = NULL;
	ALLEGRO_SAMPLE *sompokemon[151];
	ALLEGRO_SAMPLE *somataques[17];
	ALLEGRO_SAMPLE *musicamenu = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instancemusicamenu = NULL;
	ALLEGRO_SAMPLE *musicabatalha = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instancemusicabatalha = NULL;

   
	//----------------------- rotinas de inicializacao ---------------------------------------
    
	// inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
    // inicializa o modulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	// inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}

	// inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	al_init_image_addon();

	al_install_audio();

	al_init_acodec_addon();

	// inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	// cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
 
	// cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	// carrega o arquivo fonte padrão do pokemon e inicializa com difentes tipos de tamanho
    font = al_load_font("pokemon_fire_red.ttf", 48, 1);   
	if(font == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
	font40 = al_load_font("pokemon_fire_red.ttf", 40, 1);   
	if(font == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
	font64 = al_load_font("pokemon_fire_red.ttf", 64, 1);   
	if(font == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
	font80 = al_load_font("pokemon_fire_red.ttf", 80, 1);   
	if(font == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}

 	// cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}
   
	// instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	

	// registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	// registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	// registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());


	// inicia o temporizador
	al_start_timer(timer);
	srand(time(NULL));

	// espaço de audio
	al_reserve_samples(20);

	// carregamento dos sons do jogo
	escolha = al_load_sample("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Sons/escolha.ogg");
	comecajogo = al_load_sample("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Sons/comecajogo.ogg");

	// musica geral
	musicamenu = al_load_sample("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Sons/musicamenu.ogg");
	instancemusicamenu = al_create_sample_instance(musicamenu);
	al_attach_sample_instance_to_mixer(instancemusicamenu, al_get_default_mixer());

	// musica usada na batalha
	musicabatalha = al_load_sample("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Sons/musicabatalha.ogg");
	instancemusicabatalha = al_create_sample_instance(musicabatalha);
	al_attach_sample_instance_to_mixer(instancemusicabatalha, al_get_default_mixer());

	// som de cada pokemon
	for (i = 1; i <= 151; ++i) {
        char caminho[200];
        sprintf(caminho, "C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Sons/Pokemon/%d.ogg", i);
        sompokemon[i - 1] = al_load_sample(caminho);
    }

	// som de cada ataque
	for (i = 1; i <= 17; ++i) {
        char caminho[200];
        sprintf(caminho, "C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Sons/Ataques/%d.ogg", i);
        somataques[i - 1] = al_load_sample(caminho);
    }
	
	// carregamento das imagens do jogo
	fundomenu = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/menu.png");
	pokedex = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/pokedexfundo.png");
	caixa = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/menu2.png");
	barra = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha/telapreta.png");
	barraeu = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha/barraeu.png");
	barraop = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha/barraop.png");
	barraopcao = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/barraopcao.png");
	vs = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha/vs.png");
	fundobatalha = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha/fundobatalha.png");
	treinador = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha/eu.png");
	treinadorcosta = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/eu.png");
	pokebola = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/pokebola.png");
	pokebolaa = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/pokebolaa.png");
	hp = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/hp.png");
	hp2 = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/hp2.png");
	vivo = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/vivo.png");
	desmaiado = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/desmaiado.png");
	sem = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/sem.png");
	opcoes = al_load_bitmap("C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/opcoes.png");
	
	// carregamento de cada ataque
	for (i = 1; i <= 18; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Ataques/%d.png", i);
        animacaoataques[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento de cada pokemon
	for (i = 1; i <= 151; ++i) {
        char caminho[200];
        sprintf(caminho, "C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/firered-leafgreen/%d.png", i);
        pokemons[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento das costas dos pokemons
	for (i = 1; i <= 151; ++i) {
        char caminho[200];
        sprintf(caminho, "C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/firered-leafgreen/back/%d.png", i);
        pokemonsb[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento dos icones dos pokemons
	for (i = 1; i <= 151; ++i) {
        char caminho[200];
        sprintf(caminho, "C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/pokemon/icons/%d.png", i);
        pokemonsi[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento dos tipos dos pokemons
	for (i = 1; i <= 17; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Tipos/%d.png", i);
        tipos[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento dos efeitos
	for (i = 1; i <= 6; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Efeitos/%d.png", i);
        efeitos[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento das telas dos mapas
	for (i = 1; i <= 5; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Mapas/%d.png", i);
        fundomapa[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento das telas do historico
	for (i = 1; i <= 5; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Cards/%d.png", i);
        cards[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento das telas da mochila
	for (i = 1; i <= 10; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Mochila/%d.png", i);
        itens[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento das telas de equipe
	for (i = 1; i <= 6; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Equipe/%d.png", i);
        equipe[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento dos treinadores rivais
	for (i = 1; i <= 7; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Users/lap11/OneDrive/Área de Trabalho/TP-PDS1/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha/%d.png", i);
        rivais[i - 1] = al_load_bitmap(caminho);
    }


	al_play_sample(comecajogo, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		
	while(playing) 
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if(menu == 1){
			graficoMenu(fundomenu, font, font64, instancemusicamenu);

			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            	menuTeclas(ev, escolha);
        	}
		}

		if(tpokedex == 1){
			graficoTpokedex(pokedex, pokemons, sompokemon, tipos, font80, font64, font40);

			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            	tpokedexTeclas(ev, escolha);
        	}
		}

		if(mapa == 1){
			graficoMapa(fundomapa, caixa, font80, font);
			
			
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            	mapaTeclas(ev, escolha, instancemusicamenu);
        	}
		}

		if(tcard == 1){
			graficoTcard (cards, font80,font64, font);
			
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            	tcardTeclas (ev, escolha);
        	}
		}
		
		if(bag == 1){
			graficoBag (itens, font80, font64);

			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
				teclasBag (ev, escolha);
        	}
		}

		if(tequipe == 1){
			graficoTequipe (equipe, pokemonsi, font80, font);
			
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
				teclasTequipe (ev, escolha);
        	}
		}

		if(introbatalha == 1){
			graficoIntroBatalha (fundobatalha, barra, barraop, vs, barraeu, treinador, rivais, font80, instancemusicabatalha);
		}

		if(batalha == 1){
			graficoBatalha (ev, fundobatalha, barraopcao, hp, hp2, sem, font, opcoes, treinadorcosta, pokebola, pokebolaa, pokemons, pokemonsb, animacaoataques, barra, barraop, vs, barraeu, treinador, rivais, font80, font64, instancemusicabatalha, sompokemon);
			
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
				teclasBatalha (ev, escolha, somataques);
				// quando a batalha acabar int tela = -200; int tela2 = 0; int tela3 = 0;
        	}
		}
    		
		al_flip_display();
	} // fim do while
     


	// procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
	al_destroy_timer(timer);


	// destroi o audio
	al_destroy_sample(escolha);
	al_destroy_sample(comecajogo);
	for(i = 0; i < 151; i++){
		al_destroy_sample(sompokemon[i]);
	}
	for(i = 0; i < 17; i++){
		al_destroy_sample(somataques[i]);
	}
	al_destroy_sample(musicamenu);
	al_destroy_sample_instance(instancemusicamenu);
	al_destroy_sample(musicabatalha);
	al_destroy_sample_instance(instancemusicabatalha);


	// destroi grafico
	al_destroy_bitmap(fundomenu);
	al_destroy_bitmap(caixa);
	al_destroy_bitmap(barra);
	al_destroy_bitmap(barraeu);
	al_destroy_bitmap(barraop);
	al_destroy_bitmap(barraopcao);
	al_destroy_bitmap(vs);
	al_destroy_bitmap(fundobatalha);
	al_destroy_bitmap(treinador);
	al_destroy_bitmap(treinadorcosta);
	al_destroy_bitmap(pokebola);
	al_destroy_bitmap(pokebolaa);
	al_destroy_bitmap(hp);
	al_destroy_bitmap(hp2);
	al_destroy_bitmap(vivo);
	al_destroy_bitmap(desmaiado);
	al_destroy_bitmap(sem);
	al_destroy_bitmap(opcoes);
	al_destroy_bitmap(pokedex);
	for (i = 0; i < 17; ++i) {
        al_destroy_bitmap(animacaoataques[i]);
    }
	for (i = 0; i < 151; ++i) {
        al_destroy_bitmap(pokemons[i]);
    }
	for (i = 0; i < 151; ++i) {
        al_destroy_bitmap(pokemonsi[i]);
    }
	for (i = 0; i < 151; ++i) {
        al_destroy_bitmap(pokemonsb[i]);
    }
	for (i = 0; i < 17; ++i) {
        al_destroy_bitmap(tipos[i]);
    }
	for (i = 0; i < 6; ++i) {
        al_destroy_bitmap(efeitos[i]);
    }
	for (i = 0; i < 5; ++i) {
        al_destroy_bitmap(fundomapa[i]);
    }
	for (i = 0; i < 5; ++i) {
        al_destroy_bitmap(cards[i]);
    }
	for (i = 0; i < 10; ++i) {
        al_destroy_bitmap(itens[i]);
    }
	for (i = 0; i < 6; ++i) {
        al_destroy_bitmap(equipe[i]);
    }
	for (i = 0; i < 7; ++i) {
        al_destroy_bitmap(rivais[i]);
    }

	// destruções padrão e fonte
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_font(font);
	al_destroy_font(font40);
	al_destroy_font(font64);
	al_destroy_font(font80);
   
 
	return 0;
}*/