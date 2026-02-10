#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define NUM_PRATOS 8

const float FPS = 100;  

//parametros da tela
const int SCREEN_W = 1200;
const int SCREEN_H = 540;

//parametros do prato
const int PRATO_W = 40;
const int PRATO_H = 20;

//parametros do poste
const int POSTE_W = 5;
//sistema para fazer o poste mudar de cor
const int MINIHASTE = 91;

//parametros da grama
const int GRASS_H = 80;

//parametros do jogador
const int JOGADOR_W = 50;
const int JOGADOR_H = 100;

// setor de structs

typedef struct jogador{
	float x;//define o ponto central do jogador
	float vel;//velocidade do jogador
	int dir, esq;
	ALLEGRO_COLOR cor;

	//parametros de animacao
	int maxFrame;
	int curFrame;
	int frameCount;
	int frameDelay;
	int frameWidth;
	int frameHeight;
	int animationColumns;
	int animationDirection;

	int animationRow;

	ALLEGRO_BITMAP *sprite;
} jogador;

typedef struct prato {
	float x;//define o ponto central do prato
	float y;// queda do prato
	float queda;
	float energia;// valor de equilibrio do prato
	bool existe;
	float poste1;
	float poste2;
	float poste3;// para mudar a cor do poste
	
} prato;

//faz o fundo

void faz_floresta(ALLEGRO_BITMAP *floresta){
	al_draw_scaled_bitmap(floresta, 0, 0, al_get_bitmap_width(floresta),
									 al_get_bitmap_height(floresta),
									 0, 0, SCREEN_W, SCREEN_H, 0);
}

void faz_deserto(ALLEGRO_BITMAP *deserto){
	al_draw_scaled_bitmap(deserto, 0, 0, al_get_bitmap_width(deserto),
									 al_get_bitmap_height(deserto),
									 0, 0, SCREEN_W, SCREEN_H, 0);
}

void faz_oceano(ALLEGRO_BITMAP *oceano){
	al_draw_scaled_bitmap(oceano, 0, 0, al_get_bitmap_width(oceano),
									 al_get_bitmap_height(oceano),
									 0, 0, SCREEN_W, SCREEN_H, 0);
}

void faz_ceu(ALLEGRO_BITMAP *ceu){
	al_draw_scaled_bitmap(ceu, 0, 0, al_get_bitmap_width(ceu),
									 al_get_bitmap_height(ceu),
									 0, 0, SCREEN_W, SCREEN_H, 0);
}

// funções de jogador

void comecajogador(jogador *j, ALLEGRO_BITMAP *sprite) {
	j->x = SCREEN_W / 2;
	j->cor = al_map_rgb(81, 40, 4);
	j->vel = 1.5;
	j->dir = 0;
	j->esq = 0;

	//parametros de animacao
	j->maxFrame = 3;
	j->curFrame = 1;
	j->frameCount = 0;
	j->frameDelay = 50;
	j->frameWidth = 87;
	j->frameHeight = 86;
	j->animationColumns = 3;
	j->animationDirection = 1;

	j->animationRow = 2;

	j->sprite = sprite;
}

void faz_jogador(jogador j) {
	int fx = (j.curFrame % j.animationColumns) * j.frameWidth;
	int fy = j.animationRow * j.frameHeight;

	al_draw_bitmap_region(j.sprite, fx, fy, j.frameWidth,
		j.frameHeight, j.x - j.frameWidth/2, SCREEN_H - (GRASS_H/2 + JOGADOR_H), 0);
	
}

void animacao_jogador(jogador *j){
	if (j->dir == 1){
		if(++j->frameCount >= j->frameDelay){
			if(++j->curFrame >= j->maxFrame){
				j->animationRow = 1;
				j->curFrame = 0;
			}
		j->frameCount = 0;
		}
	}
	else if (j->esq == 1){
		if(++j->frameCount >= j->frameDelay){
			if(++j->curFrame >= j->maxFrame){
				j->animationRow = 0;
				j->curFrame = 0;
			}
		j->frameCount = 0;
		}
	}
}

void reset_animacao_jogador(jogador *j, int reset){
	if(reset == 1)
		j->animationRow = 2;
		j->curFrame = 1;
}

void att_jogador(jogador *j, ALLEGRO_SAMPLE_INSTANCE *instancegrama){
	if(j->esq) {
		al_set_sample_instance_gain(instancegrama, 0.4);
		al_set_sample_instance_speed(instancegrama, 1.5);
		al_play_sample_instance(instancegrama);
		j->animationRow = 0;
		if(j->x - j->vel > 10)
			j->x -= j->vel;
	}
	if(j->dir) {
		al_set_sample_instance_gain(instancegrama, 0.4);
		al_set_sample_instance_speed(instancegrama, 1.5);
		al_play_sample_instance(instancegrama);
		j->animationRow = 1;
		if(j->x + j->vel < SCREEN_W - 10)
			j->x += j->vel;
	}	
}

void att_jogador_oceano(jogador *j, ALLEGRO_SAMPLE_INSTANCE *instanceareia){
	if(j->esq) {
		al_set_sample_instance_gain(instanceareia, 0.7);
		al_set_sample_instance_speed(instanceareia, 0.8);
		al_play_sample_instance(instanceareia);
		j->animationRow = 0;
		if(j->x - j->vel > 10 )
			j->x -= j->vel;
	}
	if(j->dir) {
		al_set_sample_instance_gain(instanceareia, 0.7);
		al_set_sample_instance_speed(instanceareia, 0.8);
		al_play_sample_instance(instanceareia);
		j->animationRow = 1;
		if(j->x + j->vel < SCREEN_W - 10)
			j->x += j->vel;
	}	
}

void comecajogadordeserto(jogador *j, ALLEGRO_BITMAP *spritedeserto) {
	j->x = SCREEN_W / 2;
	j->cor = al_map_rgb(81, 40, 4);
	j->vel = 1.5;
	j->dir = 0;
	j->esq = 0;

	//parametros de animacao
	j->maxFrame = 4;
	j->curFrame = 1;
	j->frameCount = 0;
	j->frameDelay = 50;
	j->frameWidth = 96;
	j->frameHeight = 96;
	j->animationColumns = 4;
	j->animationDirection = 1;

	j->animationRow = 3;

	j->sprite = spritedeserto;
}

void animacao_jogador_deserto(jogador *j){
	if (j->dir == 1){
		if(++j->frameCount >= j->frameDelay){
			if(++j->curFrame >= j->maxFrame){
				j->animationRow = 2;
				j->curFrame = 0;
			}
			j->frameCount = 0;
		}
	}
	else if (j->esq == 1){
		if(++j->frameCount >= j->frameDelay){
			if(++j->curFrame >= j->maxFrame){
				j->animationRow = 1;
				j->curFrame = 0;
			}
			j->frameCount = 0;
		}
	}
	else{
		if(++j->frameCount >= j->frameDelay){
			if(++j->curFrame >= j->maxFrame){
				j->animationRow = 3;
				j->curFrame = 0;
			}
			j->frameCount = 0;
		}
	}
}

void reset_animacao_jogador_deserto(jogador *j, int reset){
	if(reset == 1)
		j->animationRow = 3;
		j->curFrame = 1;
}

void att_jogador_deserto(jogador *j, ALLEGRO_SAMPLE_INSTANCE *instancedesertoaudio){
	if(j->esq) {
		al_set_sample_instance_gain(instancedesertoaudio, 0.7);
		al_set_sample_instance_speed(instancedesertoaudio, 1);
		al_play_sample_instance(instancedesertoaudio);
		j->animationRow = 1;
		if(j->x - j->vel > 10)
			j->x -= j->vel;
	}
	if(j->dir) {
		al_set_sample_instance_gain(instancedesertoaudio, 0.7);
		al_set_sample_instance_speed(instancedesertoaudio, 1);
		al_play_sample_instance(instancedesertoaudio);
		j->animationRow = 2;
		if(j->x + j->vel < SCREEN_W - 10)
			j->x += j->vel;
	}	
}

void comecajogadorceu(jogador *j, ALLEGRO_BITMAP *spriteceu) {
	j->x = SCREEN_W / 2;
	j->cor = al_map_rgb(81, 40, 4);
	j->vel = 1.5;
	j->dir = 0;
	j->esq = 0;

	//parametros de animacao
	j->maxFrame = 4;
	j->curFrame = 1;
	j->frameCount = 0;
	j->frameDelay = 50;
	j->frameWidth = 96;
	j->frameHeight = 96;
	j->animationColumns = 4;
	j->animationDirection = 1;

	j->animationRow = 3;

	j->sprite = spriteceu;
}

void animacao_jogador_ceu(jogador *j){
	if (j->dir == 1){
		if(++j->frameCount >= j->frameDelay){
			if(++j->curFrame >= j->maxFrame){
				j->animationRow = 2;
				j->curFrame = 0;
			}
			j->frameCount = 0;
		}
	}
	else if (j->esq == 1){
		if(++j->frameCount >= j->frameDelay){
			if(++j->curFrame >= j->maxFrame){
				j->animationRow = 1;
				j->curFrame = 0;
			}
			j->frameCount = 0;
		}
	}
	else{
		if(++j->frameCount >= j->frameDelay){
			if(++j->curFrame >= j->maxFrame){
				j->animationRow = 3;
				j->curFrame = 0;
			}
			j->frameCount = 0;
		}
	}
}

void reset_animacao_jogador_ceu(jogador *j, int reset){
	if(reset == 1)
		j->animationRow = 3;
		j->curFrame = 1;
}

void att_jogador_ceu(jogador *j, ALLEGRO_SAMPLE_INSTANCE *instanceasas){
	if(j->esq) {
		al_set_sample_instance_gain(instanceasas, 0.7);
		al_set_sample_instance_speed(instanceasas, 1.2);
		al_play_sample_instance(instanceasas);
		j->animationRow = 1;
		if(j->x - j->vel > 10)
			j->x -= j->vel;
	}
	if(j->dir) {
		al_set_sample_instance_gain(instanceasas, 0.7);
		al_set_sample_instance_speed(instanceasas, 1.2);
		al_play_sample_instance(instanceasas);
		j->animationRow = 2;
		if(j->x + j->vel < SCREEN_W - 10)
			j->x += j->vel;
	}
	else{
		al_set_sample_instance_gain(instanceasas, 0.7);
		al_set_sample_instance_speed(instanceasas, 1.2);
		al_play_sample_instance(instanceasas);
	}
}

// funções de prato

void comecaprato(prato *prato){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		prato[i].energia = 255;
		prato[i].x = (SCREEN_W/9)*(i+1);
		prato[i].y = 110;
		prato[i].queda = 0;
		prato[i].existe = false;
		prato[i].poste1 = 186;
		prato[i].poste2 = 81;
		prato[i].poste3 = 0;
	}
}

void faz_prato(prato *prato, ALLEGRO_BITMAP *pokeball, ALLEGRO_BITMAP *greatball, ALLEGRO_BITMAP *ultraball, ALLEGRO_BITMAP *masterball, ALLEGRO_BITMAP *poste, ALLEGRO_BITMAP *spriteprato, ALLEGRO_BITMAP *spriteprato2, ALLEGRO_SAMPLE_INSTANCE *instancevidabaixa){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		//desenhando as pokeballs
		if(prato[i].existe){
			if(i == 3 || i == 4){
				al_draw_scaled_bitmap(pokeball, 0, 0, al_get_bitmap_width(pokeball),
									 al_get_bitmap_height(pokeball),
									 prato[i].x - 20, prato[i].y - 40, 40, 40, 0);
			}
			if(i == 2 || i == 5){
				al_draw_scaled_bitmap(greatball, 0, 0, al_get_bitmap_width(greatball),
									 al_get_bitmap_height(greatball),
									 prato[i].x - 20, prato[i].y - 40, 40, 40, 0);
			}
			if(i == 1 || i == 6){
				al_draw_scaled_bitmap(ultraball, 0, 0, al_get_bitmap_width(ultraball),
									 al_get_bitmap_height(ultraball),
									 prato[i].x - 20, prato[i].y - 40, 40, 40, 0);
			}
			if(i == 0 || i == 7){
				al_draw_scaled_bitmap(masterball, 0, 0, al_get_bitmap_width(masterball),
									 al_get_bitmap_height(masterball),
									 prato[i].x - 20, prato[i].y - 40, 40, 40, 0);
			}
			att_energia(prato);
			//desenhando o prato e os minipratos
			al_draw_scaled_bitmap(spriteprato, 0, 0, al_get_bitmap_width(spriteprato),
									al_get_bitmap_height(spriteprato),
									prato[i].x - 40, prato[i].y, 80, 20, 0);
			al_draw_tinted_bitmap(spriteprato2, al_map_rgb(255,prato[i].energia,prato[i].energia), 
									prato[i].x - 20, prato[i].y, 0);
			al_draw_tinted_bitmap(spriteprato2, al_map_rgb(255,prato[i].energia,prato[i].energia), 
									prato[i].x - 40, prato[i].y, 0);
			al_draw_tinted_bitmap(spriteprato2, al_map_rgb(255,prato[i].energia,prato[i].energia), 
									prato[i].x + 20, prato[i].y, 0);
			al_draw_tinted_bitmap(spriteprato2, al_map_rgb(255,prato[i].energia,prato[i].energia), 
									prato[i].x, prato[i].y, 0);
			//som de vida baixa
			if(prato[i].energia <=50 && prato[i].energia > 10){
				al_set_sample_instance_gain(instancevidabaixa, 0.6);
				al_set_sample_instance_speed(instancevidabaixa, 0.9);
				al_play_sample_instance(instancevidabaixa);
			}
			//desenhando o poste
			al_draw_scaled_bitmap(poste, 0, 0, al_get_bitmap_width(poste),
									al_get_bitmap_height(poste),
									prato[i].x - 26, prato[i].y - 4, 49, 360, 0);
		}
	}
}

void faz_prato2(prato *prato, ALLEGRO_BITMAP *pokeball, ALLEGRO_BITMAP *greatball, ALLEGRO_BITMAP *ultraball, ALLEGRO_BITMAP *masterball, ALLEGRO_BITMAP *poste, ALLEGRO_BITMAP *spriteprato, ALLEGRO_BITMAP *spriteprato2){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		//desenhando as pokeballs
		if(prato[i].existe){
			if(i == 3 || i == 4){
				al_draw_scaled_bitmap(pokeball, 0, 0, al_get_bitmap_width(pokeball),
									 al_get_bitmap_height(pokeball),
									 prato[i].x - 20, prato[i].y - 40, 40, 40, 0);
			}
			if(i == 2 || i == 5){
				al_draw_scaled_bitmap(greatball, 0, 0, al_get_bitmap_width(greatball),
									 al_get_bitmap_height(greatball),
									 prato[i].x - 20, prato[i].y - 40, 40, 40, 0);
			}
			if(i == 1 || i == 6){
				al_draw_scaled_bitmap(ultraball, 0, 0, al_get_bitmap_width(ultraball),
									 al_get_bitmap_height(ultraball),
									 prato[i].x - 20, prato[i].y - 40, 40, 40, 0);
			}
			if(i == 0 || i == 7){
				al_draw_scaled_bitmap(masterball, 0, 0, al_get_bitmap_width(masterball),
									 al_get_bitmap_height(masterball),
									 prato[i].x - 20, prato[i].y - 40, 40, 40, 0);
			}
			att_energia(prato);
			//desenhando o prato e os minipratos
			al_draw_scaled_bitmap(spriteprato, 0, 0, al_get_bitmap_width(spriteprato),
									al_get_bitmap_height(spriteprato),
									prato[i].x - 40, prato[i].y, 80, 20, 0);
			al_draw_tinted_bitmap(spriteprato2, al_map_rgb(255,prato[i].energia,prato[i].energia), 
									prato[i].x - 20, prato[i].y, 0);
			al_draw_tinted_bitmap(spriteprato2, al_map_rgb(255,prato[i].energia,prato[i].energia), 
									prato[i].x - 40, prato[i].y, 0);
			al_draw_tinted_bitmap(spriteprato2, al_map_rgb(255,prato[i].energia,prato[i].energia), 
									prato[i].x + 20, prato[i].y, 0);
			al_draw_tinted_bitmap(spriteprato2, al_map_rgb(255,prato[i].energia,prato[i].energia), 
									prato[i].x, prato[i].y, 0);
			//desenhando o poste
			al_draw_scaled_bitmap(poste, 0, 0, al_get_bitmap_width(poste),
									al_get_bitmap_height(poste),
									prato[i].x - 26, prato[i].y - 4, 49, 360, 0);
		}
	}
}

//temporizador de aparecimento dos pratos
void gera_prato(prato *prato, ALLEGRO_SAMPLE *pratoaparece){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		if(!prato[i].existe){
			if(i == 3 || i == 4){
				if(rand() % 300 == 0){
					prato[i].existe = true;
					al_play_sample(pratoaparece, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
					break;
				}
			}
			if(i == 2 || i == 5){
				if(rand() % 1500 == 0){
					prato[i].existe = true;
					al_play_sample(pratoaparece, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
					break;
				}
			}
			else{
				if(rand() % 2500 == 0){
					prato[i].existe = true;
					al_play_sample(pratoaparece, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
					break;
				}
			}
			
		}
	}
}

float energiadecayrate = 1.0; // Quantidade de vida a ser reduzida por segundo
void att_energia(prato *prato)
{
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		if(prato[i].existe){
    		float tempofps = 1.0 / FPS*2; // Tempo decorrido desde o último quadro

    		prato[i].energia -= (energiadecayrate * tempofps); // Reduz a vida com base no tempo decorrido
    		if (prato[i].energia < 0){
        		prato[i].energia = 0;
			}
		}
	}
}

//adiciona energia ao apertar espaço
void modifica_energia(prato *prato, jogador *j, float adicionar, ALLEGRO_SAMPLE_INSTANCE *instancebump){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		if (adicionar){
			if(j->x - 8 > prato[i].x - 5 && j->x - 8< prato[i].x + 5 && j->dir == 0 && j->esq == 0){
				al_set_sample_instance_gain(instancebump, 0.6);
				al_set_sample_instance_speed(instancebump, 0.8);
				al_play_sample_instance(instancebump);
				prato[i].energia++;
				prato[i].poste1 = rand() % 256;
				prato[i].poste2 = rand() % 256;
				prato[i].poste3 = rand() % 256;
				if (prato[i].energia > 255){
					prato[i].energia = 255;
				}
			}
		}
		if(!adicionar){
			prato[i].poste1 = 186;
			prato[i].poste2 = 81;
			prato[i].poste3 = 0;
		}
		if(j->esq == 1 || j->dir == 1){
			adicionar = 0;
		}	
	}
}

void modifica_energia_deserto(prato *prato, jogador *j, float adicionar, ALLEGRO_SAMPLE_INSTANCE *instancebump){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		if (adicionar){
			if(j->x + 1 > prato[i].x - 5 && j->x + 1 < prato[i].x + 5 && j->dir == 0 && j->esq == 0){
				al_set_sample_instance_gain(instancebump, 0.6);
				al_set_sample_instance_speed(instancebump, 0.8);
				al_play_sample_instance(instancebump);
				prato[i].energia++;
				prato[i].poste1 = rand() % 256;
				prato[i].poste2 = rand() % 256;
				prato[i].poste3 = rand() % 256;
				if (prato[i].energia > 255){
					prato[i].energia = 255;
				}
			}
		}
		if(!adicionar){
			prato[i].poste1 = 186;
			prato[i].poste2 = 81;
			prato[i].poste3 = 0;
		}
		if(j->esq == 1 || j->dir == 1){
			adicionar = 0;
		}	
	}
}

void modifica_energia_ceu(prato *prato, jogador *j, float adicionar, ALLEGRO_SAMPLE_INSTANCE *instancebump){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		if (adicionar){
			if(j->x + 3 > prato[i].x - 5 && j->x + 3 < prato[i].x + 5 && j->dir == 0 && j->esq == 0){
				al_set_sample_instance_gain(instancebump, 0.6);
				al_set_sample_instance_speed(instancebump, 0.8);
				al_play_sample_instance(instancebump);
				prato[i].energia++;
				prato[i].poste1 = rand() % 256;
				prato[i].poste2 = rand() % 256;
				prato[i].poste3 = rand() % 256;
				if (prato[i].energia > 255){
					prato[i].energia = 255;
				}
			}
		}
		if(!adicionar){
			prato[i].poste1 = 186;
			prato[i].poste2 = 81;
			prato[i].poste3 = 0;
		}
		if(j->esq == 1 || j->dir == 1){
			adicionar = 0;
		}	
	}
}

//desenha o prato no chão no final
void prato_caiu(prato *prato, ALLEGRO_BITMAP *spriteprato2, ALLEGRO_BITMAP *pokeball, ALLEGRO_BITMAP *greatball, ALLEGRO_BITMAP *ultraball, ALLEGRO_BITMAP *masterball, ALLEGRO_SAMPLE *vida0){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		if(prato[i].queda){
			al_draw_tinted_bitmap(spriteprato2, al_map_rgb(255,prato[i].energia,prato[i].energia), 
									prato[i].x - 20, (SCREEN_H - GRASS_H/2), 0);
			al_draw_tinted_bitmap(spriteprato2, al_map_rgb(255,prato[i].energia,prato[i].energia), 
									prato[i].x - 40, (SCREEN_H - GRASS_H/2), 0);
			al_draw_tinted_bitmap(spriteprato2, al_map_rgb(255,prato[i].energia,prato[i].energia), 
									prato[i].x + 20, (SCREEN_H - GRASS_H/2), 0);
			al_draw_tinted_bitmap(spriteprato2, al_map_rgb(255,prato[i].energia,prato[i].energia), 
									prato[i].x, (SCREEN_H - GRASS_H/2), 0);
			//al_play_sample(vida0, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
			if(i == 3 || i == 4){
				al_draw_scaled_bitmap(pokeball, 0, 0, al_get_bitmap_width(pokeball),
									 al_get_bitmap_height(pokeball),
									 prato[i].x - 20, (SCREEN_H - GRASS_H/2) - 40, 40, 40, 0);
			}
			if(i == 2 || i == 5){
				al_draw_scaled_bitmap(greatball, 0, 0, al_get_bitmap_width(greatball),
									 al_get_bitmap_height(greatball),
									 prato[i].x - 20, (SCREEN_H - GRASS_H/2) - 40, 40, 40, 0);
			}
			if(i == 1 || i == 6){
				al_draw_scaled_bitmap(ultraball, 0, 0, al_get_bitmap_width(ultraball),
									 al_get_bitmap_height(ultraball),
									 prato[i].x - 20, (SCREEN_H - GRASS_H/2) - 40, 40, 40, 0);
			}
			if(i == 0 || i == 7){
				al_draw_scaled_bitmap(masterball, 0, 0, al_get_bitmap_width(masterball),
									 al_get_bitmap_height(masterball),
									 prato[i].x - 20, (SCREEN_H - GRASS_H/2) - 40, 40, 40, 0);
			}
		}
	}
}

void faz_pokemon(prato *prato, ALLEGRO_BITMAP *pikachu, ALLEGRO_BITMAP *bulbassauro, ALLEGRO_BITMAP *charmander, ALLEGRO_BITMAP *squirtle, ALLEGRO_BITMAP *gastly, ALLEGRO_BITMAP *pidgey, ALLEGRO_BITMAP *caterpie, ALLEGRO_BITMAP *abra){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		//desenha as sprites dos pokemons
		if(prato[i].existe){
			if(i == 0){
				al_draw_scaled_bitmap(caterpie, 0, 0, al_get_bitmap_width(caterpie),
									 al_get_bitmap_height(caterpie),
									 prato[i].x - 20, prato[i].y - 80, 80, 80, 0);
			}
			if(i == 1){
				al_draw_scaled_bitmap(pidgey, 0, 0, al_get_bitmap_width(pidgey),
									 al_get_bitmap_height(pidgey),
									 prato[i].x - 20, prato[i].y - 80, 100, 100, 0);
			}
			if(i == 2){
				al_draw_scaled_bitmap(bulbassauro, 0, 0, al_get_bitmap_width(bulbassauro),
									 al_get_bitmap_height(bulbassauro),
									 prato[i].x - 20, prato[i].y - 80, 90, 80, 0);
			}
			if(i == 3){
				al_draw_scaled_bitmap(charmander, 0, 0, al_get_bitmap_width(charmander),
									 al_get_bitmap_height(charmander),
									 prato[i].x - 20, prato[i].y - 80, 80, 80, 0);
			}
			if(i == 4){
				al_draw_scaled_bitmap(squirtle, 0, 0, al_get_bitmap_width(squirtle),
									 al_get_bitmap_height(squirtle),
									 prato[i].x - 20, prato[i].y - 80, 90, 90, 0);
			}
			if(i == 5){
				al_draw_scaled_bitmap(pikachu, 0, 0, al_get_bitmap_width(pikachu),
									 al_get_bitmap_height(pikachu),
									 prato[i].x - 20, prato[i].y - 80, 95, 95, 0);
			}
			if(i == 6){
				al_draw_scaled_bitmap(gastly, 0, 0, al_get_bitmap_width(gastly),
									 al_get_bitmap_height(gastly),
									 prato[i].x - 20, prato[i].y - 80, 90, 90, 0);
			}
			if(i == 7){
				al_draw_scaled_bitmap(abra, 0, 0, al_get_bitmap_width(abra),
									 al_get_bitmap_height(abra),
									 prato[i].x - 20, prato[i].y - 80, 85, 85, 0);
			}
		}
	}
}

void faz_pokemon_deserto(prato *prato, ALLEGRO_BITMAP *sandshrew, ALLEGRO_BITMAP *rapidash, ALLEGRO_BITMAP *dugtrio, ALLEGRO_BITMAP *flygon, ALLEGRO_BITMAP *gabite, ALLEGRO_BITMAP *gliscor, ALLEGRO_BITMAP *metaton, ALLEGRO_BITMAP *shelgon){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		//desenha as sprites dos pokemons
		if(prato[i].existe){
			if(i == 0){
				al_draw_scaled_bitmap(sandshrew, 0, 0, al_get_bitmap_width(sandshrew),
									 al_get_bitmap_height(sandshrew),
									 prato[i].x - 20, prato[i].y - 80, 80, 80, 0);
			}
			if(i == 1){
				al_draw_scaled_bitmap(rapidash, 0, 0, al_get_bitmap_width(rapidash),
									 al_get_bitmap_height(rapidash),
									 prato[i].x - 20, prato[i].y - 80, 80, 80, 0);
			}
			if(i == 2){
				al_draw_scaled_bitmap(dugtrio, 0, 0, al_get_bitmap_width(dugtrio),
									 al_get_bitmap_height(dugtrio),
									 prato[i].x - 10, prato[i].y - 80, 90, 80, 0);
			}
			if(i == 3){
				al_draw_scaled_bitmap(flygon, 0, 0, al_get_bitmap_width(flygon),
									 al_get_bitmap_height(flygon),
									 prato[i].x - 20, prato[i].y - 80, 80, 80, 0);
			}
			if(i == 4){
				al_draw_scaled_bitmap(gabite, 0, 0, al_get_bitmap_width(gabite),
									 al_get_bitmap_height(gabite),
									 prato[i].x - 20, prato[i].y - 80, 90, 90, 0);
			}
			if(i == 5){
				al_draw_scaled_bitmap(gliscor, 0, 0, al_get_bitmap_width(gliscor),
									 al_get_bitmap_height(gliscor),
									 prato[i].x - 20, prato[i].y - 80, 80, 80, 0);
			}
			if(i == 6){
				al_draw_scaled_bitmap(metaton, 0, 0, al_get_bitmap_width(metaton),
									 al_get_bitmap_height(metaton),
									 prato[i].x - 20, prato[i].y - 80, 90, 90, 0);
			}
			if(i == 7){
				al_draw_scaled_bitmap(shelgon, 0, 0, al_get_bitmap_width(shelgon),
									 al_get_bitmap_height(shelgon),
									 prato[i].x - 20, prato[i].y - 80, 85, 85, 0);
			}
		}
	}
}

void faz_pokemon_oceano(prato *prato, ALLEGRO_BITMAP *gyarados, ALLEGRO_BITMAP *starmie, ALLEGRO_BITMAP *kingdra, ALLEGRO_BITMAP *swampert, ALLEGRO_BITMAP *sharpedo, ALLEGRO_BITMAP *floatzel, ALLEGRO_BITMAP *lapras, ALLEGRO_BITMAP *feraliggator){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		//desenha as sprites dos pokemons
		if(prato[i].existe){
			if(i == 0){
				al_draw_scaled_bitmap(gyarados, 0, 0, al_get_bitmap_width(gyarados),
									 al_get_bitmap_height(gyarados),
									 prato[i].x - 20, prato[i].y - 75, 75, 75, 0);
			}
			if(i == 1){
				al_draw_scaled_bitmap(starmie, 0, 0, al_get_bitmap_width(starmie),
									 al_get_bitmap_height(starmie),
									 prato[i].x - 20, prato[i].y - 80, 75, 75, 0);
			}
			if(i == 2){
				al_draw_scaled_bitmap(kingdra, 0, 0, al_get_bitmap_width(kingdra),
									 al_get_bitmap_height(kingdra),
									 prato[i].x - 20, prato[i].y - 80, 90, 80, 0);
			}
			if(i == 3){
				al_draw_scaled_bitmap(swampert, 0, 0, al_get_bitmap_width(swampert),
									 al_get_bitmap_height(swampert),
									 prato[i].x - 20, prato[i].y - 80, 80, 80, 0);
			}
			if(i == 4){
				al_draw_scaled_bitmap(sharpedo, 0, 0, al_get_bitmap_width(sharpedo),
									 al_get_bitmap_height(sharpedo),
									 prato[i].x - 10, prato[i].y - 80, 75, 75, 0);
			}
			if(i == 5){
				al_draw_scaled_bitmap(floatzel, 0, 0, al_get_bitmap_width(floatzel),
									 al_get_bitmap_height(floatzel),
									 prato[i].x - 20, prato[i].y - 80, 80, 80, 0);
			}
			if(i == 6){
				al_draw_scaled_bitmap(lapras, 0, 0, al_get_bitmap_width(lapras),
									 al_get_bitmap_height(lapras),
									 prato[i].x - 15, prato[i].y - 80, 80, 80, 0);
			}
			if(i == 7){
				al_draw_scaled_bitmap(feraliggator, 0, 0, al_get_bitmap_width(feraliggator),
									 al_get_bitmap_height(feraliggator),
									 prato[i].x - 20, prato[i].y - 80, 85, 85, 0);
			}
		}
	}
}

void faz_pokemon_ceu(prato *prato, ALLEGRO_BITMAP *articuno, ALLEGRO_BITMAP *mew, ALLEGRO_BITMAP *hooh, ALLEGRO_BITMAP *rayquaza, ALLEGRO_BITMAP *mewtwo, ALLEGRO_BITMAP *zapdos, ALLEGRO_BITMAP *dialga, ALLEGRO_BITMAP *arceus){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		//desenha as sprites dos pokemons
		if(prato[i].existe){
			if(i == 0){
				al_draw_scaled_bitmap(articuno, 0, 0, al_get_bitmap_width(articuno),
									 al_get_bitmap_height(articuno),
									 prato[i].x - 20, prato[i].y - 75, 80, 80, 0);
			}
			if(i == 1){
				al_draw_scaled_bitmap(mew, 0, 0, al_get_bitmap_width(mew),
									 al_get_bitmap_height(mew),
									 prato[i].x - 20, prato[i].y - 80, 80, 80, 0);
			}
			if(i == 2){
				al_draw_scaled_bitmap(hooh, 0, 0, al_get_bitmap_width(hooh),
									 al_get_bitmap_height(hooh),
									 prato[i].x - 10, prato[i].y - 80, 70, 70, 0);
			}
			if(i == 3){
				al_draw_scaled_bitmap(rayquaza, 0, 0, al_get_bitmap_width(rayquaza),
									 al_get_bitmap_height(rayquaza),
									 prato[i].x - 20, prato[i].y - 80, 80, 80, 0);
			}
			if(i == 4){
				al_draw_scaled_bitmap(mewtwo, 0, 0, al_get_bitmap_width(mewtwo),
									 al_get_bitmap_height(mewtwo),
									 prato[i].x - 20, prato[i].y - 80, 90, 90, 0);
			}
			if(i == 5){
				al_draw_scaled_bitmap(zapdos, 0, 0, al_get_bitmap_width(zapdos),
									 al_get_bitmap_height(zapdos),
									 prato[i].x - 20, prato[i].y - 80, 80, 80, 0);
			}
			if(i == 6){
				al_draw_scaled_bitmap(dialga, 0, 0, al_get_bitmap_width(dialga),
									 al_get_bitmap_height(dialga),
									 prato[i].x - 20, prato[i].y - 90, 80, 80, 0);
			}
			if(i == 7){
				al_draw_scaled_bitmap(arceus, 0, 0, al_get_bitmap_width(arceus),
									 al_get_bitmap_height(arceus),
									 prato[i].x - 15, prato[i].y - 75, 80, 80, 0);
			}
		}
	}
}

// função de poste

void poste_muda(prato *prato, jogador *j, float adicionar, ALLEGRO_BITMAP *haste){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		if(adicionar){
			if(j->x - 8 > prato[i].x - 5 && j->x - 8 < prato[i].x + 5 && j->dir == 0 && j->esq == 0){
				al_draw_tinted_bitmap(haste, al_map_rgb(prato[i].poste1,prato[i].poste2,prato[i].poste3), 
									prato[i].x - 6, prato[i].y + PRATO_H, 0);
				al_draw_tinted_bitmap(haste, al_map_rgb(prato[i].poste1,prato[i].poste2,prato[i].poste3), 
									prato[i].x - 6, prato[i].y + PRATO_H + MINIHASTE, 0);
				al_draw_tinted_bitmap(haste, al_map_rgb(prato[i].poste1,prato[i].poste2,prato[i].poste3), 
									prato[i].x - 6, prato[i].y + PRATO_H + MINIHASTE*2, 0);
			}
		}
	}
}

void poste_muda_deserto(prato *prato, jogador *j, float adicionar, ALLEGRO_BITMAP *haste){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		if(adicionar){
			if(j->x + 1 > prato[i].x - 5 && j->x + 1 < prato[i].x + 5 && j->dir == 0 && j->esq == 0){
				al_draw_tinted_bitmap(haste, al_map_rgb(prato[i].poste1,prato[i].poste2,prato[i].poste3), 
									prato[i].x - 6, prato[i].y + PRATO_H, 0);
				al_draw_tinted_bitmap(haste, al_map_rgb(prato[i].poste1,prato[i].poste2,prato[i].poste3), 
									prato[i].x - 6, prato[i].y + PRATO_H + MINIHASTE, 0);
				al_draw_tinted_bitmap(haste, al_map_rgb(prato[i].poste1,prato[i].poste2,prato[i].poste3), 
									prato[i].x - 6, prato[i].y + PRATO_H + MINIHASTE*2, 0);
			}
		}
	}
}

void poste_muda_ceu(prato *prato, jogador *j, float adicionar, ALLEGRO_BITMAP *haste){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		if(adicionar){
			if(j->x + 3 > prato[i].x - 5 && j->x + 3 < prato[i].x + 5 && j->dir == 0 && j->esq == 0){
				al_draw_tinted_bitmap(haste, al_map_rgb(prato[i].poste1,prato[i].poste2,prato[i].poste3), 
									prato[i].x - 6, prato[i].y + PRATO_H, 0);
				al_draw_tinted_bitmap(haste, al_map_rgb(prato[i].poste1,prato[i].poste2,prato[i].poste3), 
									prato[i].x - 6, prato[i].y + PRATO_H + MINIHASTE, 0);
				al_draw_tinted_bitmap(haste, al_map_rgb(prato[i].poste1,prato[i].poste2,prato[i].poste3), 
									prato[i].x - 6, prato[i].y + PRATO_H + MINIHASTE*2, 0);
			}
		}
	}
}

// implementa o recorde
recorde(int pontuacao){
    FILE *arquivo;
    int recorde;

    arquivo = fopen("recorde.txt", "r");

    if (arquivo != NULL) {
        fscanf(arquivo, "%d", &recorde);

        // Caso a pontuação seja o recorde
        if (pontuacao > recorde) {
            arquivo = fopen("recorde.txt", "w");

            if (arquivo != NULL) {
                //salva o recorde
                fprintf(arquivo, "%d", pontuacao);
                fclose(arquivo);
            }
        }

        fclose(arquivo);
		return recorde;
    }
    else{
        //primeira pontuação
        arquivo = fopen("recorde.txt", "w");

        if (arquivo != NULL) {
            fprintf(arquivo, "%d", pontuacao);
            fclose(arquivo);
        }
		return recorde;
    }
}



int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_FONT *font = NULL;

	// imagem do fundo
	ALLEGRO_BITMAP *floresta = NULL;
	ALLEGRO_BITMAP *deserto = NULL;
	ALLEGRO_BITMAP *oceano = NULL;
	ALLEGRO_BITMAP *ceu = NULL;
	// imagem do menu
	ALLEGRO_BITMAP *fundomenu = NULL;
	ALLEGRO_BITMAP *fundomenu2 = NULL;
	//imagem do jogador
	ALLEGRO_BITMAP *sprite = NULL;
	ALLEGRO_BITMAP *spritedeserto = NULL;
	ALLEGRO_BITMAP *spriteceu = NULL;
	//imagem das pokebolas
	ALLEGRO_BITMAP *pokeball = NULL;
	ALLEGRO_BITMAP *greatball = NULL;
	ALLEGRO_BITMAP *ultraball = NULL;
	ALLEGRO_BITMAP *masterball = NULL;
	//imagem dos pokemons floresta
	ALLEGRO_BITMAP *bulbassauro = NULL;
	ALLEGRO_BITMAP *charmander = NULL;
	ALLEGRO_BITMAP *squirtle = NULL;
	ALLEGRO_BITMAP *pikachu = NULL;
	ALLEGRO_BITMAP *caterpie = NULL;
	ALLEGRO_BITMAP *gastly = NULL;
	ALLEGRO_BITMAP *pidgey = NULL;
	ALLEGRO_BITMAP *abra = NULL;
	//imagem dos pokemons deserto
	ALLEGRO_BITMAP *sandshrew = NULL;
	ALLEGRO_BITMAP *rapidash = NULL;
	ALLEGRO_BITMAP *dugtrio = NULL;
	ALLEGRO_BITMAP *flygon = NULL;
	ALLEGRO_BITMAP *gabite = NULL;
	ALLEGRO_BITMAP *gliscor = NULL;
	ALLEGRO_BITMAP *metaton = NULL;
	ALLEGRO_BITMAP *shelgon = NULL;
	//imagem dos pokemons oceano
	ALLEGRO_BITMAP *gyarados = NULL;
	ALLEGRO_BITMAP *starmie = NULL;
	ALLEGRO_BITMAP *kingdra = NULL;
	ALLEGRO_BITMAP *swampert = NULL;
	ALLEGRO_BITMAP *sharpedo = NULL;
	ALLEGRO_BITMAP *floatzel = NULL;
	ALLEGRO_BITMAP *lapras = NULL;
	ALLEGRO_BITMAP *feraliggator = NULL;
	//imagem dos pokemons ceu
	ALLEGRO_BITMAP *articuno = NULL;
	ALLEGRO_BITMAP *mew = NULL;
	ALLEGRO_BITMAP *hooh = NULL;
	ALLEGRO_BITMAP *rayquaza = NULL;
	ALLEGRO_BITMAP *mewtwo = NULL;
	ALLEGRO_BITMAP *zapdos = NULL;
	ALLEGRO_BITMAP *dialga = NULL;
	ALLEGRO_BITMAP *arceus = NULL;
	//imagem do poste
	ALLEGRO_BITMAP *poste = NULL;
	ALLEGRO_BITMAP *haste = NULL;
	//imagem do prato
	ALLEGRO_BITMAP *spriteprato = NULL;
	ALLEGRO_BITMAP *spriteprato2 = NULL;
	//sons do jogo
	ALLEGRO_SAMPLE *musicamenu = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instancemusicamenu = NULL;
	ALLEGRO_SAMPLE *musicamenu2 = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instancemusicamenu2 = NULL;
	ALLEGRO_SAMPLE *musicafase1 = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instancemusicafase1 = NULL;
	ALLEGRO_SAMPLE *musicafase2 = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instancemusicafase2 = NULL;
	ALLEGRO_SAMPLE *musicafase3 = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instancemusicafase3 = NULL;
	ALLEGRO_SAMPLE *musicafase4 = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instancemusicafase4 = NULL;
	ALLEGRO_SAMPLE *musicafim = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instancemusicafim = NULL;
	ALLEGRO_SAMPLE *escolha = NULL;
	ALLEGRO_SAMPLE *comecajogo = NULL;
	ALLEGRO_SAMPLE *vidabaixa = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instancevidabaixa = NULL;
	ALLEGRO_SAMPLE *vida0 = NULL;
	ALLEGRO_SAMPLE *pratoaparece = NULL;
	ALLEGRO_SAMPLE *bump = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instancebump = NULL;
	ALLEGRO_SAMPLE *grama = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instancegrama = NULL;
	ALLEGRO_SAMPLE *desertoaudio = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instancedesertoaudio = NULL;
	ALLEGRO_SAMPLE *areia = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instanceareia = NULL;
	ALLEGRO_SAMPLE *asas = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instanceasas = NULL;


   
	//----------------------- rotinas de inicializacao ---------------------------------------
    
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
    //inicializa o modulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	al_init_image_addon();

	al_install_audio();

	al_init_acodec_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
 
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    font = al_load_font("pokemon_fire_red.ttf", 48, 1);   
	if(font == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}

 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}
   
	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//instala o mouse
	if(!al_install_mouse()) {
		fprintf(stderr, "failed to initialize mouse!\n");
		return -1;
	}

	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	//registra na fila os eventos de mouse (ex: clicar em um botao do mouse)
	al_register_event_source(event_queue, al_get_mouse_event_source());  	


	//inicia o temporizador
	al_start_timer(timer);
	srand(time(NULL));

	//inicia as structs
	jogador jogador;
	prato prato[NUM_PRATOS];

	//espaço de audio
	al_reserve_samples(20);

	musicamenu = al_load_sample("musicamenu.ogg");
	instancemusicamenu = al_create_sample_instance(musicamenu);
	al_attach_sample_instance_to_mixer(instancemusicamenu, al_get_default_mixer());

	musicamenu2 = al_load_sample("musicamenu2.ogg");
	instancemusicamenu2 = al_create_sample_instance(musicamenu2);
	al_attach_sample_instance_to_mixer(instancemusicamenu2, al_get_default_mixer());

	musicafase1 = al_load_sample("musicafase1.ogg");
	instancemusicafase1 = al_create_sample_instance(musicafase1);
	al_attach_sample_instance_to_mixer(instancemusicafase1, al_get_default_mixer());

	musicafase2 = al_load_sample("musicafase2.ogg");
	instancemusicafase2 = al_create_sample_instance(musicafase2);
	al_attach_sample_instance_to_mixer(instancemusicafase2, al_get_default_mixer());

	musicafase3 = al_load_sample("musicafase3.ogg");
	instancemusicafase3 = al_create_sample_instance(musicafase3);
	al_attach_sample_instance_to_mixer(instancemusicafase3, al_get_default_mixer());

	musicafase4 = al_load_sample("musicafase4.ogg");
	instancemusicafase4 = al_create_sample_instance(musicafase4);
	al_attach_sample_instance_to_mixer(instancemusicafase4, al_get_default_mixer());

	musicafim = al_load_sample("musicafim.ogg");
	instancemusicafim = al_create_sample_instance(musicafim);
	al_attach_sample_instance_to_mixer(instancemusicafim, al_get_default_mixer());

	//som de menu
	escolha = al_load_sample("escolha.ogg");
	//ao comecar o jogo
	comecajogo = al_load_sample("comecajogo.ogg");
	//quando o prato fica com pouca vida
	vidabaixa = al_load_sample("vidabaixa.ogg");
	instancevidabaixa = al_create_sample_instance(vidabaixa);
	al_attach_sample_instance_to_mixer(instancevidabaixa, al_get_default_mixer());
	//quando o prato cai
	vida0 = al_load_sample("vida0.ogg");
	//quando o prato aparece
	pratoaparece = al_load_sample("pratoaparece.ogg");
	//quando adiciona energia
	bump = al_load_sample("bump.ogg");
	instancebump = al_create_sample_instance(bump);
	al_attach_sample_instance_to_mixer(instancebump, al_get_default_mixer());
	//andar na grama
	grama = al_load_sample("grama.ogg");
	instancegrama = al_create_sample_instance(grama);
	al_attach_sample_instance_to_mixer(instancegrama, al_get_default_mixer());
	//som de andar no deserto
	desertoaudio = al_load_sample("desertoaudio.ogg");
	instancedesertoaudio = al_create_sample_instance(desertoaudio);
	al_attach_sample_instance_to_mixer(instancedesertoaudio, al_get_default_mixer());
	//som de andar na praia
	areia = al_load_sample("areia.ogg");
	instanceareia = al_create_sample_instance(areia);
	al_attach_sample_instance_to_mixer(instanceareia, al_get_default_mixer());
	//som de asas no ceu
	asas = al_load_sample("asas.ogg");
	instanceasas = al_create_sample_instance(asas);
	al_attach_sample_instance_to_mixer(instanceasas, al_get_default_mixer());

	//configurações do fundo
	floresta = al_load_bitmap("fundo.jpg");
	deserto = al_load_bitmap("deserto.png");
	oceano = al_load_bitmap("oceano.jpg");
	ceu = al_load_bitmap("ceu.jpg");

	//configurações do menu
	fundomenu = al_load_bitmap("menu.png");
	fundomenu2 = al_load_bitmap("menu2.png");

	//configurações do jogador
	sprite = al_load_bitmap("Sprite.png");
	spritedeserto = al_load_bitmap("playerdeserto.png");
	spriteceu = al_load_bitmap("playervoa.png");

	//sprites das pokeballs
	pokeball = al_load_bitmap("pokeball.png");
	al_convert_mask_to_alpha(pokeball, al_map_rgb(238, 238, 238));
	al_convert_mask_to_alpha(pokeball, al_map_rgb(255, 255, 255));

	greatball = al_load_bitmap("greatball.png");
	al_convert_mask_to_alpha(greatball, al_map_rgb(238, 238, 238));
	al_convert_mask_to_alpha(greatball, al_map_rgb(255, 255, 255));

	ultraball = al_load_bitmap("ultraball.png");
	al_convert_mask_to_alpha(ultraball, al_map_rgb(238, 238, 238));
	al_convert_mask_to_alpha(ultraball, al_map_rgb(255, 255, 255));

	masterball = al_load_bitmap("masterball.png");
	al_convert_mask_to_alpha(masterball, al_map_rgb(238, 238, 238));
	al_convert_mask_to_alpha(masterball, al_map_rgb(255, 255, 255));

	//sprite dos pokemons
	bulbassauro = al_load_bitmap("bulbassauro.png");
	charmander = al_load_bitmap("charmander.png");
	squirtle = al_load_bitmap("squirtle.png");
	pikachu = al_load_bitmap("pikachu.png");
	caterpie = al_load_bitmap("caterpie.png");
	gastly = al_load_bitmap("gastly.png");
	pidgey = al_load_bitmap("pidgey.png");
	abra = al_load_bitmap("abra.png");

	sandshrew = al_load_bitmap("sandshrew.png");
	rapidash = al_load_bitmap("rapidash.png");
	dugtrio = al_load_bitmap("dugtrio.png");
	flygon = al_load_bitmap("flygon.png");
	gabite = al_load_bitmap("gabite.png");
	gliscor = al_load_bitmap("gliscor.png");
	metaton = al_load_bitmap("metaton.png");
	shelgon = al_load_bitmap("shelgon.png");

	gyarados = al_load_bitmap("gyarados.png");
	starmie = al_load_bitmap("starmie.png");
	kingdra = al_load_bitmap("kingdra.png");
	swampert = al_load_bitmap("swampert.png");
	sharpedo = al_load_bitmap("sharpedo.png");
	floatzel = al_load_bitmap("floatzel.png");
	lapras = al_load_bitmap("lapras.png");
	feraliggator = al_load_bitmap("feraliggator.png");

	articuno = al_load_bitmap("articuno.png");
	mew = al_load_bitmap("mew.png");
	hooh = al_load_bitmap("ho-oh.png");
	rayquaza = al_load_bitmap("rayquaza.png");
	mewtwo = al_load_bitmap("mewtwo.png");
	zapdos = al_load_bitmap("zapdos.png");
	dialga = al_load_bitmap("dialga.png");
	arceus = al_load_bitmap("arceus.png");

	poste = al_load_bitmap("haste.png");
	haste = al_load_bitmap("haste2.png");

	spriteprato = al_load_bitmap("prato.jpg");
	spriteprato2 = al_load_bitmap("prato2.jpg");

	//declarações de variaveis
	int i, x;
	float adicionar = 0;// faz com que o espaço adicione energia constantemente
	int playing = 1;
	double starttime = 0;
	bool jogo = false;
	int pontuacao = 0;
	int menu, menu2, tutorial, fase1, fase2, fase3, fase4, fimdejogo, reiniciaf1, reiniciaf2, reiniciaf3, reiniciaf4, sound;
	menu = 1;
	menu2 = 0;
	tutorial = 0;
	fase1 = 0;
	fase2 = 0;
	fase3 = 0;
	fase4 = 0;
	fimdejogo = 0;
	reiniciaf1 = 0;
	reiniciaf2 = 0;
	reiniciaf3 = 0;
	reiniciaf4 = 0;
	sound = 1;
	al_play_sample(comecajogo, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
		

	while(playing) 
	{
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);
		if(al_get_timer_count(timer)%(int)FPS == 0){
					printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
		}
		starttime = al_get_time();

		if(menu == 1){
			al_draw_scaled_bitmap(fundomenu, 0, 0, al_get_bitmap_width(fundomenu),
									 al_get_bitmap_height(fundomenu),
									 0, 0, SCREEN_W, SCREEN_H, 0);
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, SCREEN_H/2, ALLEGRO_ALIGN_CENTRE, 
						"Aperte ENTER para selecionar a fase!");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, SCREEN_H/2 + SCREEN_H/5, ALLEGRO_ALIGN_CENTRE, 
						"Aperte ESC caso queira sair do jogo!");
			al_set_sample_instance_gain(instancemusicamenu, 0.6);
			al_set_sample_instance_speed(instancemusicamenu, 1);
			al_play_sample_instance(instancemusicamenu);
			if(ev.keyboard.keycode == ALLEGRO_KEY_ENTER){
				al_play_sample(escolha, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				menu = 0;
				menu2 = 1;
				al_stop_sample_instance(instancemusicamenu);
			}
			if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
				menu = 1;
				playing = 0;
			}
		}

		if(menu2 == 1){
			al_draw_scaled_bitmap(fundomenu2, 0, 0, al_get_bitmap_width(fundomenu2),
									 al_get_bitmap_height(fundomenu2),
									 0, 0, SCREEN_W, SCREEN_H, 0);
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, SCREEN_H/6, ALLEGRO_ALIGN_CENTRE, 
						"Aperte 1, 2, 3 ou 4 para selecionar a fase!");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, SCREEN_H/7 + 50, ALLEGRO_ALIGN_CENTRE, 
						"Aperte BACKSPACE caso queira voltar ao menu");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, SCREEN_H/7 + 350, ALLEGRO_ALIGN_CENTRE, 
						"Aperte T caso queira ler o tutorial do jogo");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/7, SCREEN_H/4 + 50, 0, 
						"1----> Floresta");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/7, SCREEN_H/4 + SCREEN_H/4 + 50, 0, 
						"2----> Oceano");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/7 + SCREEN_W/2, SCREEN_H/4 + 50, 0, 
						"3----> Deserto");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/7 + SCREEN_W/2, SCREEN_H/4 + SCREEN_H/4 + 50, 0, 
						"4----> Ceu");
			al_set_sample_instance_gain(instancemusicamenu2, 0.5);
			al_set_sample_instance_speed(instancemusicamenu2, 1);
			al_play_sample_instance(instancemusicamenu2);
			al_set_sample_instance_gain(instancemusicafase1, 0.6);
			al_set_sample_instance_gain(instancemusicafase2, 0.6);
			al_set_sample_instance_gain(instancemusicafase3, 0.8);
			al_set_sample_instance_gain(instancemusicafase4, 0.6);
			if(ev.keyboard.keycode == ALLEGRO_KEY_1){
				al_play_sample(escolha, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				menu2 = 0;
				comecaprato(prato);
				comecajogador(&jogador, sprite);
				sound = 1;
				fase1 = 1;
				pontuacao = 0;
				al_stop_sample_instance(instancemusicamenu2);
			}
			if(ev.keyboard.keycode == ALLEGRO_KEY_2){
				al_play_sample(escolha, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				menu2 = 0;
				comecaprato(prato);
				comecajogador(&jogador, sprite);
				sound = 1;
				fase2 = 1;
				pontuacao = 0;
				al_stop_sample_instance(instancemusicamenu2);
			}
			if(ev.keyboard.keycode == ALLEGRO_KEY_3){
				al_play_sample(escolha, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				menu2 = 0;
				comecaprato(prato);
				comecajogadordeserto(&jogador, spritedeserto);
				sound = 1;
				fase3 = 1;
				pontuacao = 0;
				al_stop_sample_instance(instancemusicamenu2);
			}
			if(ev.keyboard.keycode == ALLEGRO_KEY_4){
				al_play_sample(escolha, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				menu2 = 0;
				comecaprato(prato);
				comecajogadorceu(&jogador, spriteceu);
				sound = 1;
				fase4 = 1;
				pontuacao = 0;
				al_stop_sample_instance(instancemusicamenu2);
			}
			if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE){
				al_play_sample(escolha, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				menu = 1;
				menu2 = 0;
				al_stop_sample_instance(instancemusicamenu2);
			}
			if(ev.keyboard.keycode == ALLEGRO_KEY_T){
				al_play_sample(escolha, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				tutorial = 1;
				menu2 = 0;
			}
		}

		if(tutorial == 1){
			al_draw_scaled_bitmap(fundomenu2, 0, 0, al_get_bitmap_width(fundomenu2),
									 al_get_bitmap_height(fundomenu2),
									 0, 0, SCREEN_W, SCREEN_H, 0);
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, 70, ALLEGRO_ALIGN_CENTRE, 
						"O objetivo do jogo é manter os pratos equilibrados,");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, 115, ALLEGRO_ALIGN_CENTRE, 
						"a cor deles indica sua energia, branco sendo alta e vermelho sendo baixa.");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, 160, ALLEGRO_ALIGN_CENTRE, 
						"Tambem havera alguns sons para auxiliar, sendo eles,");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, 205, ALLEGRO_ALIGN_CENTRE, 
						"quando os pratos aparecerem e quando estiverem prestes a cair.");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, 250, ALLEGRO_ALIGN_CENTRE, 
						"Os comando do jogo sao bem simples!");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, 295, ALLEGRO_ALIGN_CENTRE, 
						"Para andar para a direita, aperte D.");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, 330, ALLEGRO_ALIGN_CENTRE, 
						"Para andar para a esquerda, aperte A.");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, 375, ALLEGRO_ALIGN_CENTRE, 
						"Para adicionar energia aos pratos, aperte Espaco.");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, 420, ALLEGRO_ALIGN_CENTRE, 
						"Aperte V para voltar!");
			if(ev.keyboard.keycode == ALLEGRO_KEY_V){
				al_play_sample(escolha, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				tutorial = 0;
				menu2 = 1;
			}
		}

		if(fase1 == 1){
			if(ev.type == ALLEGRO_EVENT_TIMER) {

				al_set_sample_instance_speed(instancemusicafase1, 1);
				al_play_sample_instance(instancemusicafase1);
				pontuacao++;
				jogador.frameCount++;

				//função de fundo
				faz_floresta(floresta);
				al_draw_textf(font, al_map_rgb(0, 0, 0),
							5, 5, 0,
							"Pontuacao: %d", pontuacao);

				//funções de jogador
				att_jogador(&jogador, instancegrama);
				faz_jogador(jogador);

				animacao_jogador(&jogador);

				//funções de prato
				gera_prato(prato, pratoaparece);
				faz_prato(prato, pokeball, greatball, ultraball, masterball, poste, spriteprato, spriteprato2, instancevidabaixa);
				faz_pokemon(prato, pikachu, bulbassauro, charmander, squirtle, gastly, pidgey, caterpie, abra);

				//funções ao apertar espaço
				modifica_energia(prato, &jogador, adicionar, instancebump);
				poste_muda(prato, &jogador, adicionar, haste);

				//atualiza a tela (quando houver algo para mostrar)
				al_flip_display();

				//caso a energia seja igual a 0, finaliza o jogo e faz o prato cair
				for(i = 0; i < NUM_PRATOS; i++){
					if(prato[i].energia < 50){
						al_set_sample_instance_gain(instancemusicafase1, 0.3);
					}
					if(prato[i].energia == 0){
						//desenham o prato caido
						prato[i].existe = false;
						prato[i].queda = 1;
						al_stop_sample_instance(instancemusicafase1);
						if(sound == 1 && prato[i].queda == 1){
							al_play_sample(vida0, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
							sound = 0;
						}
						while (al_get_time() - starttime < 2.0){
							faz_floresta(floresta);
							faz_jogador(jogador);
							faz_prato2(prato, pokeball, greatball, ultraball, masterball, poste, spriteprato, spriteprato2);
							faz_pokemon(prato, pikachu, bulbassauro, charmander, squirtle, gastly, pidgey, caterpie, abra);
							prato_caiu(prato, spriteprato2, pokeball, greatball, ultraball, masterball, vida0);
							al_flip_display();
						}
						fase1 = 0;
						fimdejogo = 1;
					}
				}
			}
		}

		if(fase2 == 1){
			if(ev.type == ALLEGRO_EVENT_TIMER) {

				al_set_sample_instance_speed(instancemusicafase2, 1);
				al_play_sample_instance(instancemusicafase2);
				pontuacao++;
				jogador.frameCount++;

				//função de fundo
				faz_oceano(oceano);
				al_draw_textf(font, al_map_rgb(0, 0, 0),
							5, 5, 0,
							"Pontuacao: %d", pontuacao);

				//funções de jogador
				att_jogador_oceano(&jogador, instanceareia);
				faz_jogador(jogador);

				animacao_jogador(&jogador);

				//funções de prato
				gera_prato(prato, pratoaparece);
				faz_prato(prato, pokeball, greatball, ultraball, masterball, poste, spriteprato, spriteprato2, instancevidabaixa);
				faz_pokemon_oceano(prato, gyarados, starmie, kingdra, swampert, sharpedo, floatzel, lapras, feraliggator);

				//funções ao apertar espaço
				modifica_energia(prato, &jogador, adicionar, instancebump);
				poste_muda(prato, &jogador, adicionar, haste);

				//atualiza a tela (quando houver algo para mostrar)
				al_flip_display();

				//caso a energia seja igual a 0, finaliza o jogo e faz o prato cair
				for(i = 0; i < NUM_PRATOS; i++){
					if(prato[i].energia < 50){
						al_set_sample_instance_gain(instancemusicafase2, 0.3);
					}
					if(prato[i].energia == 0){
						//desenham o prato caido
						prato[i].existe = false;
						prato[i].queda = 1;
						al_stop_sample_instance(instancemusicafase2);
						if(sound == 1 && prato[i].queda == 1){
							al_play_sample(vida0, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
							sound = 0;
						}
						while (al_get_time() - starttime < 2.0){
							faz_oceano(oceano);
							faz_jogador(jogador);
							faz_prato2(prato, pokeball, greatball, ultraball, masterball, poste, spriteprato, spriteprato2);
							faz_pokemon_oceano(prato, gyarados, starmie, kingdra, swampert, sharpedo, floatzel, lapras, feraliggator);
							prato_caiu(prato, spriteprato2, pokeball, greatball, ultraball, masterball, vida0);
							al_flip_display();
						}
						fase2 = 0;
						fimdejogo = 1;
					}
				}
			}
		}

		if(fase3 == 1){
			if(ev.type == ALLEGRO_EVENT_TIMER) {

				al_set_sample_instance_speed(instancemusicafase3, 1);
				al_play_sample_instance(instancemusicafase3);
				pontuacao++;
				jogador.frameCount++;

				//função de fundo
				faz_deserto(deserto);
				al_draw_textf(font, al_map_rgb(0, 0, 0),
							5, 5, 0,
							"Pontuacao: %d", pontuacao);

				//funções de jogador
				att_jogador_deserto(&jogador, instancedesertoaudio);
				faz_jogador(jogador);

				animacao_jogador_deserto(&jogador);

				//funções de prato
				gera_prato(prato, pratoaparece);
				faz_prato(prato, pokeball, greatball, ultraball, masterball, poste, spriteprato, spriteprato2, instancevidabaixa);
				faz_pokemon_deserto(prato, sandshrew, rapidash, dugtrio, flygon, gabite, gliscor, metaton, shelgon);

				//funções ao apertar espaço
				modifica_energia_deserto(prato, &jogador, adicionar, instancebump);
				poste_muda_deserto(prato, &jogador, adicionar, haste);

				//atualiza a tela (quando houver algo para mostrar)
				al_flip_display();

				//caso a energia seja igual a 0, finaliza o jogo e faz o prato cair
				for(i = 0; i < NUM_PRATOS; i++){
					if(prato[i].energia < 50){
						al_set_sample_instance_gain(instancemusicafase3, 0.3);
					}
					if(prato[i].energia == 0){
						//desenham o prato caido
						prato[i].existe = false;
						prato[i].queda = 1;
						al_stop_sample_instance(instancemusicafase3);
						if(sound == 1 && prato[i].queda == 1){
							al_play_sample(vida0, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
							sound = 0;
						}
						while (al_get_time() - starttime < 2.0){
							faz_deserto(deserto);
							faz_jogador(jogador);
							faz_prato2(prato, pokeball, greatball, ultraball, masterball, poste, spriteprato, spriteprato2);
							faz_pokemon_deserto(prato, sandshrew, rapidash, dugtrio, flygon, gabite, gliscor, metaton, shelgon);
							prato_caiu(prato, spriteprato2, pokeball, greatball, ultraball, masterball, vida0);
							al_flip_display();
						}
						fase3 = 0;
						fimdejogo = 1;
					}
				}
			}
		}

		if(fase4 == 1){
			if(ev.type == ALLEGRO_EVENT_TIMER) {

				al_set_sample_instance_speed(instancemusicafase4, 1);
				al_play_sample_instance(instancemusicafase4);
				pontuacao++;
				jogador.frameCount++;

				//função de fundo
				faz_ceu(ceu);
				al_draw_textf(font, al_map_rgb(255, 255, 255),
							5, 5, 0,
							"Pontuacao: %d", pontuacao);
				
				//funções de jogador
				att_jogador_ceu(&jogador, instanceasas);
				faz_jogador(jogador);

				animacao_jogador_ceu(&jogador);

				//funções de prato
				gera_prato(prato, pratoaparece);
				faz_prato(prato, pokeball, greatball, ultraball, masterball, poste, spriteprato, spriteprato2, instancevidabaixa);
				faz_pokemon_ceu(prato, articuno, mew, hooh, rayquaza, mewtwo, zapdos, dialga, arceus);

				//funções ao apertar espaço
				modifica_energia_ceu(prato, &jogador, adicionar, instancebump);
				poste_muda_ceu(prato, &jogador, adicionar, haste);

				//atualiza a tela (quando houver algo para mostrar)
				al_flip_display();

				//caso a energia seja igual a 0, finaliza o jogo e faz o prato cair
				for(i = 0; i < NUM_PRATOS; i++){
					if(prato[i].energia < 50){
						al_set_sample_instance_gain(instancemusicafase4, 0.3);
					}
					if(prato[i].energia == 0){
						//desenham o prato caido
						prato[i].existe = false;
						prato[i].queda = 1;
						al_stop_sample_instance(instancemusicafase4);
						if(sound == 1 && prato[i].queda == 1){
							al_play_sample(vida0, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
							sound = 0;
						}
						while (al_get_time() - starttime < 2.0){
							faz_ceu(ceu);
							faz_jogador(jogador);
							faz_prato2(prato, pokeball, greatball, ultraball, masterball, poste, spriteprato, spriteprato2);
							faz_pokemon_ceu(prato, articuno, mew, hooh, rayquaza, mewtwo, zapdos, dialga, arceus);
							prato_caiu(prato, spriteprato2, pokeball, greatball, ultraball, masterball, vida0);
							al_flip_display();
						}
						fase4 = 0;
						fimdejogo = 1;
					}
				}
			}
		}

		if(fimdejogo == 1){
			//al_rest(2);
			al_draw_scaled_bitmap(fundomenu2, 0, 0, al_get_bitmap_width(fundomenu2),
								al_get_bitmap_height(fundomenu2),
								0, 0, SCREEN_W, SCREEN_H, 0);
			al_set_sample_instance_gain(instancemusicafim, 0.7);
			al_set_sample_instance_speed(instancemusicafim, 1);
			al_play_sample_instance(instancemusicafim);
			//ve se a pontuação é o recorde e da a mensagem final dependendo da resposta
			x = recorde(pontuacao);

			if(x > pontuacao){
				al_draw_textf(font, al_map_rgb(0, 0, 0),
						SCREEN_W/2, SCREEN_H/3, ALLEGRO_ALIGN_CENTRE,
						"Fim de jogo! Pontuacao: %d. Recorde: %d", pontuacao, x);
			}
			if(x <= pontuacao){
				al_draw_textf(font, al_map_rgb(0, 0, 0),
						SCREEN_W/2, SCREEN_H/3, ALLEGRO_ALIGN_CENTRE,
						"Fim de jogo! Pontuacao: %d. Novo recorde! Recorde: %d", pontuacao, x);
			}
			if(ev.keyboard.keycode == ALLEGRO_KEY_1){
				al_play_sample(escolha, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				fimdejogo = 0;
				reiniciaf1 = 1;
				al_flip_display();
				al_stop_sample_instance(instancemusicafim);
			}
			if(ev.keyboard.keycode == ALLEGRO_KEY_2){
				al_play_sample(escolha, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				fimdejogo = 0;
				reiniciaf2 = 1;
				al_flip_display();
				al_stop_sample_instance(instancemusicafim);
			}
			if(ev.keyboard.keycode == ALLEGRO_KEY_3){
				al_play_sample(escolha, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				fimdejogo = 0;
				reiniciaf3 = 1;
				al_flip_display();
				al_stop_sample_instance(instancemusicafim);
			}
			if(ev.keyboard.keycode == ALLEGRO_KEY_4){
				al_play_sample(escolha, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				fimdejogo = 0;
				reiniciaf4 = 1;
				al_flip_display();
				al_stop_sample_instance(instancemusicafim);
			}
			if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE){
				al_play_sample(escolha, 0.7, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				fimdejogo = 0;
				menu2 = 1;
				al_flip_display();
				al_stop_sample_instance(instancemusicafim);
			}
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, SCREEN_H/2, ALLEGRO_ALIGN_CENTRE, 
						"Aperte 1, 2, 3 ou 4 de acordo com a fase para tentar denovo!");
			al_draw_textf(font, al_map_rgb(0, 0, 0), 
						SCREEN_W/2, SCREEN_H/2 + 50, ALLEGRO_ALIGN_CENTRE, 
						"Aperte BACKSPACE caso queira voltar ao menu");
			al_flip_display();
		}
		if(reiniciaf1 == 1){
			comecaprato(prato);
			comecajogador(&jogador, sprite);
			fase1 = 1;
			reiniciaf1 = 0;
			pontuacao = 0;
			sound = 1;
			al_set_sample_instance_gain(instancemusicafase1, 0.6);

		}

		if(reiniciaf2 == 1){
			comecaprato(prato);
			comecajogador(&jogador, sprite);
			fase2 = 1;
			reiniciaf2 = 0;
			pontuacao = 0;
			sound = 1;
			al_set_sample_instance_gain(instancemusicafase2, 0.6);
		}

		if(reiniciaf3 == 1){
			comecaprato(prato);
			comecajogadordeserto(&jogador, spritedeserto);
			fase3 = 1;
			reiniciaf3 = 0;
			pontuacao = 0;
			sound = 1;
			al_set_sample_instance_gain(instancemusicafase3, 0.8);
		}

		if(reiniciaf4 == 1){
			comecaprato(prato);
			comecajogadorceu(&jogador, spriteceu);
			fase4 = 1;
			reiniciaf4 = 0;
			pontuacao = 0;
			sound = 1;
			al_set_sample_instance_gain(instancemusicafase4, 0.6);
		}

		al_flip_display();
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		//se o tipo de evento for um clique de mouse
		if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			printf("\nmouse clicado em: %d, %d", ev.mouse.x, ev.mouse.y);
		}

		//se o tipo de evento for um pressionar de uma tecla
		if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {

			//movimentação para a esquerda
			if(ev.keyboard.keycode == ALLEGRO_KEY_A) {
				jogador.esq = 1;
				adicionar = 0;
			}

			//movimentação para a direita
			if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
				jogador.dir = 1;
				adicionar = 0;
			}

			//adicionar a energia ao prato
			else if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE)	{
				adicionar = 1;
			}
				
		}
		//se o tipo de evento for soltar uma tecla
		if(ev.type == ALLEGRO_EVENT_KEY_UP) {

			if(ev.keyboard.keycode == ALLEGRO_KEY_A) {
				jogador.esq = 0;
				if(fase1 == 1){
					reset_animacao_jogador(&jogador, 1);
				}
				if(fase2 == 1){
					reset_animacao_jogador(&jogador, 1);
				}
				if(fase3 == 1){
					reset_animacao_jogador_deserto(&jogador, 1);
				}
				if(fase4 == 1){
					reset_animacao_jogador_ceu(&jogador, 1);
				}
			}
			if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
				jogador.dir = 0;
				if(fase1 == 1){
					reset_animacao_jogador(&jogador, 1);
				}
				if(fase2 == 1){
					reset_animacao_jogador(&jogador, 1);
				}
				if(fase3 == 1){
					reset_animacao_jogador_deserto(&jogador, 1);
				}
				if(fase4 == 1){
					reset_animacao_jogador_ceu(&jogador, 1);
				}
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE)	{
				adicionar = 0;
			}			
		}		

	} //fim do while
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
	al_destroy_timer(timer);
	//destroi o fundo
	al_destroy_bitmap(floresta);
	al_destroy_bitmap(deserto);
	al_destroy_bitmap(oceano);
	al_destroy_bitmap(ceu);
	//destroi o audio
	al_destroy_sample(musicamenu);
	al_destroy_sample_instance(instancemusicamenu);
	al_destroy_sample(musicamenu2);
	al_destroy_sample_instance(instancemusicamenu2);
	al_destroy_sample(musicafase1);
	al_destroy_sample_instance(instancemusicafase1);
	al_destroy_sample(musicafase2);
	al_destroy_sample_instance(instancemusicafase2);
	al_destroy_sample(musicafase3);
	al_destroy_sample_instance(instancemusicafase3);
	al_destroy_sample(musicafase4);
	al_destroy_sample_instance(instancemusicafase4);
	al_destroy_sample(musicafim);
	al_destroy_sample_instance(instancemusicafim);
	al_destroy_sample(escolha);
	al_destroy_sample(comecajogo);
	al_destroy_sample(vidabaixa);
	al_destroy_sample_instance(instancevidabaixa);
	al_destroy_sample(vida0);
	al_destroy_sample(pratoaparece);
	al_destroy_sample(bump);
	al_destroy_sample_instance(instancebump);
	al_destroy_sample(grama);
	al_destroy_sample_instance(instancegrama);
	al_destroy_sample(desertoaudio);
	al_destroy_sample_instance(instancedesertoaudio);
	al_destroy_sample(areia);
	al_destroy_sample_instance(instanceareia);
	al_destroy_sample(asas);
	al_destroy_sample_instance(instanceasas);
	//destroi o menu
	al_destroy_bitmap(fundomenu);
	al_destroy_bitmap(fundomenu2);
	//destroi o jogador
	al_destroy_bitmap(sprite);
	al_destroy_bitmap(spritedeserto);
	al_destroy_bitmap(spriteceu);
	//destroi as pokebolas
	al_destroy_bitmap(pokeball);
	al_destroy_bitmap(greatball);
	al_destroy_bitmap(ultraball);
	al_destroy_bitmap(masterball);
	//destroi os pokemons
	al_destroy_bitmap(bulbassauro);
	al_destroy_bitmap(charmander);
	al_destroy_bitmap(squirtle);
	al_destroy_bitmap(pikachu);
	al_destroy_bitmap(caterpie);
	al_destroy_bitmap(gastly);
	al_destroy_bitmap(pidgey);
	al_destroy_bitmap(abra);

	al_destroy_bitmap(sandshrew);
	al_destroy_bitmap(rapidash);
	al_destroy_bitmap(dugtrio);
	al_destroy_bitmap(flygon);
	al_destroy_bitmap(gabite);
	al_destroy_bitmap(gliscor);
	al_destroy_bitmap(metaton);
	al_destroy_bitmap(shelgon);

	al_destroy_bitmap(gyarados);
	al_destroy_bitmap(starmie);
	al_destroy_bitmap(kingdra);
	al_destroy_bitmap(swampert);
	al_destroy_bitmap(sharpedo);
	al_destroy_bitmap(floatzel);
	al_destroy_bitmap(lapras);
	al_destroy_bitmap(feraliggator);
	
	al_destroy_bitmap(articuno);
	al_destroy_bitmap(mew);
	al_destroy_bitmap(hooh);
	al_destroy_bitmap(rayquaza);
	al_destroy_bitmap(mewtwo);
	al_destroy_bitmap(zapdos);
	al_destroy_bitmap(dialga);
	al_destroy_bitmap(arceus);
	//destroi o poste
	al_destroy_bitmap(poste);
	al_destroy_bitmap(haste);
	//destroi o prato
	al_destroy_bitmap(spriteprato);
	al_destroy_bitmap(spriteprato2);
	//rotinas padrao
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_font(font);
   
 
	return 0;
}