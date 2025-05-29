/*#include "Teclas.hpp"

void Teclas::menuTeclas(ALLEGRO_EVENT ev, ALLEGRO_SAMPLE *escolha){
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

void Teclas::tpokedexTeclas (ALLEGRO_EVENT ev, ALLEGRO_SAMPLE *escolha){
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

void Teclas::mapaTeclas(ALLEGRO_EVENT ev, ALLEGRO_SAMPLE *escolha, ALLEGRO_SAMPLE_INSTANCE *instancemusicamenu){
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

void Teclas::tcardTeclas (ALLEGRO_EVENT ev, ALLEGRO_SAMPLE *escolha){
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

void Teclas::bagTeclas (ALLEGRO_EVENT ev, ALLEGRO_SAMPLE *escolha){
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

void Teclas::tequipeTeclas (ALLEGRO_EVENT ev, ALLEGRO_SAMPLE *escolha){
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

void Teclas::teclasBatalha (ALLEGRO_EVENT ev, ALLEGRO_SAMPLE *escolha, ALLEGRO_SAMPLE *somataques[]){
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
}*/