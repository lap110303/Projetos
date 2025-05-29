/*#include "Grafico.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

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

void Grafico::graficoMenu(ALLEGRO_BITMAP *fundomenu, ALLEGRO_FONT *font, ALLEGRO_FONT *font64, ALLEGRO_SAMPLE_INSTANCE *instancemusicamenu, int x, int opcao){
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

void Grafico::graficoTpokedex (ALLEGRO_BITMAP *pokedex, ALLEGRO_BITMAP *pokemons[], ALLEGRO_SAMPLE *sompokemon[], ALLEGRO_BITMAP *tipos[], ALLEGRO_FONT *font80, ALLEGRO_FONT *font64, ALLEGRO_FONT *font40){
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

void Grafico::graficoMapa (ALLEGRO_BITMAP *fundomapa[], ALLEGRO_BITMAP *caixa, ALLEGRO_FONT *font80, ALLEGRO_FONT *font){
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

void Grafico::graficoTcard (ALLEGRO_BITMAP *cards[], ALLEGRO_FONT *font80, ALLEGRO_FONT *font64, ALLEGRO_FONT *font){
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

void Grafico::graficoBag (ALLEGRO_BITMAP *itens[], ALLEGRO_FONT *font80, ALLEGRO_FONT *font64){
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

void Grafico::graficoTequipe (ALLEGRO_BITMAP *equipe[], ALLEGRO_BITMAP *pokemonsi[], ALLEGRO_FONT *font80, ALLEGRO_FONT *font){
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

void Grafico::graficoIntroBatalha (ALLEGRO_BITMAP *fundobatalha, ALLEGRO_BITMAP *barra, ALLEGRO_BITMAP *barraop, ALLEGRO_BITMAP *vs, ALLEGRO_BITMAP *barraeu, ALLEGRO_BITMAP *treinador, ALLEGRO_BITMAP *rivais[], ALLEGRO_FONT *font80, ALLEGRO_SAMPLE_INSTANCE *instancemusicabatalha){
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

void Grafico::graficoBatalha (ALLEGRO_EVENT ev, ALLEGRO_BITMAP *fundobatalha, ALLEGRO_BITMAP *barraopcao, ALLEGRO_BITMAP *hp, ALLEGRO_BITMAP *hp2, ALLEGRO_BITMAP *sem, ALLEGRO_FONT *font, ALLEGRO_BITMAP *opcoes, ALLEGRO_BITMAP *treinadorcosta, ALLEGRO_BITMAP *pokebola, ALLEGRO_BITMAP *pokebolaa, ALLEGRO_BITMAP *pokemons[], ALLEGRO_BITMAP *pokemonsb[], ALLEGRO_BITMAP *animacaoataques[], ALLEGRO_BITMAP *barra, ALLEGRO_BITMAP *barraop, ALLEGRO_BITMAP *vs, ALLEGRO_BITMAP *barraeu, ALLEGRO_BITMAP *treinador, ALLEGRO_BITMAP *rivais[], ALLEGRO_FONT *font80, ALLEGRO_FONT *font64, ALLEGRO_SAMPLE_INSTANCE *instancemusicabatalha, ALLEGRO_SAMPLE *sompokemon[]){
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
				// depois adicionar um com coordenadas para seu pokemon sendo atacado
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
				// depois adicionar um com coordenadas para seu pokemon sendo atacado
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
				// depois adicionar um com coordenadas para seu pokemon sendo atacado
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
				// depois adicionar um com coordenadas para seu pokemon sendo atacado
			}
		}
	}

	if(tela3 > 495){
		tela3 = 495;
	}
}*/