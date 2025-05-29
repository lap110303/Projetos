/*#include "PokemonEletrico.hpp"
#include "Mochila.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "Mochila.hpp"
#include "Grafico.hpp"
#include "Teclas.hpp"


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

int main(int argc, char **argv){

	Grafico* grafico = new Grafico();
	Teclas* tecla = new Teclas();

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

	// carrega o arquivo fonte padrÃ£o do pokemon e inicializa com difentes tipos de tamanho
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
	escolha = al_load_sample("C:/Tp PDS2/KIT_DEV_ALLEGRO/Sons/escolha.ogg");
	comecajogo = al_load_sample("C:/Tp PDS2/KIT_DEV_ALLEGRO/Sons/comecajogo.ogg");

	// musica geral
	musicamenu = al_load_sample("C:/Tp PDS2/KIT_DEV_ALLEGRO/Sons/musicamenu.ogg");
	instancemusicamenu = al_create_sample_instance(musicamenu);
	al_attach_sample_instance_to_mixer(instancemusicamenu, al_get_default_mixer());

	// musica usada na batalha
	musicabatalha = al_load_sample("C:/Tp PDS2/KIT_DEV_ALLEGRO/Sons/musicabatalha.ogg");
	instancemusicabatalha = al_create_sample_instance(musicabatalha);
	al_attach_sample_instance_to_mixer(instancemusicabatalha, al_get_default_mixer());

	// som de cada pokemon
	for (i = 1; i <= 151; ++i) {
        char caminho[200];
        sprintf(caminho, "C:/Tp PDS2/KIT_DEV_ALLEGRO/Sons/Pokemon/%d.ogg", i);
        sompokemon[i - 1] = al_load_sample(caminho);
    }

	// som de cada ataque
	for (i = 1; i <= 17; ++i) {
        char caminho[200];
        sprintf(caminho, "C:/Tp PDS2/KIT_DEV_ALLEGRO/Sons/Ataques/%d.ogg", i);
        somataques[i - 1] = al_load_sample(caminho);
    }
	
	// carregamento das imagens do jogo
	fundomenu = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/menu.png");
	pokedex = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/pokedexfundo.png");
	caixa = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/menu2.png");
	barra = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha/telapreta.png");
	barraeu = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha/barraeu.png");
	barraop = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha/barraop.png");
	barraopcao = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/barraopcao.png");
	vs = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha/vs.png");
	fundobatalha = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha/fundobatalha.png");
	treinador = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha/eu.png");
	treinadorcosta = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/eu.png");
	pokebola = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/pokebola.png");
	pokebolaa = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/pokebolaa.png");
	hp = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/hp.png");
	hp2 = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/hp2.png");
	vivo = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/vivo.png");
	desmaiado = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/desmaiado.png");
	sem = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/sem.png");
	opcoes = al_load_bitmap("C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha2/opcoes.png");
	
	// carregamento de cada ataque
	for (i = 1; i <= 18; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Ataques/%d.png", i);
        animacaoataques[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento de cada pokemon
	for (i = 1; i <= 151; ++i) {
        char caminho[200];
        sprintf(caminho, "C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/firered-leafgreen/%d.png", i);
        pokemons[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento das costas dos pokemons
	for (i = 1; i <= 151; ++i) {
        char caminho[200];
        sprintf(caminho, "C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/firered-leafgreen/back/%d.png", i);
        pokemonsb[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento dos icones dos pokemons
	for (i = 1; i <= 151; ++i) {
        char caminho[200];
        sprintf(caminho, "C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/pokemon/icons/%d.png", i);
        pokemonsi[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento dos tipos dos pokemons
	for (i = 1; i <= 17; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Tipos/%d.png", i);
        tipos[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento dos efeitos
	for (i = 1; i <= 6; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Efeitos/%d.png", i);
        efeitos[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento das telas dos mapas
	for (i = 1; i <= 5; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Mapas/%d.png", i);
        fundomapa[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento das telas do historico
	for (i = 1; i <= 5; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Cards/%d.png", i);
        cards[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento das telas da mochila
	for (i = 1; i <= 10; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Mochila/%d.png", i);
        itens[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento das telas de equipe
	for (i = 1; i <= 6; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Equipe/%d.png", i);
        equipe[i - 1] = al_load_bitmap(caminho);
    }

	// carregamento dos treinadores rivais
	for (i = 1; i <= 7; ++i) {
        char caminho[100];
        sprintf(caminho, "C:/Tp PDS2/KIT_DEV_ALLEGRO/Imagens/Backgrounds/Batalha/%d.png", i);
        rivais[i - 1] = al_load_bitmap(caminho);
    }


	al_play_sample(comecajogo, 0.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);

	while(playing) 
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if(menu == 1){
			grafico->graficoMenu(fundomenu, font, font64, instancemusicamenu);

			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            	tecla->menuTeclas(ev, escolha);
        	}
		}

		if(tpokedex == 1){
			grafico->graficoTpokedex(pokedex, pokemons, sompokemon, tipos, font80, font64, font40);

			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            	tecla->tpokedexTeclas(ev, escolha);
        	}
		}

		if(mapa == 1){
			grafico->graficoMapa(fundomapa, caixa, font80, font);
			
			
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            	tecla->mapaTeclas(ev, escolha, instancemusicamenu);
        	}
		}

		if(tcard == 1){
			grafico->graficoTcard (cards, font80,font64, font);
			
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            	tecla->tcardTeclas (ev, escolha);
        	}
		}
		
		if(bag == 1){
			grafico->graficoBag (itens, font80, font64);

			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
				tecla->teclasBag (ev, escolha);
        	}
		}

		if(tequipe == 1){
			grafico->graficoTequipe (equipe, pokemonsi, font80, font);
			
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
				tecla->teclasTequipe (ev, escolha);
        	}
		}

		if(introbatalha == 1){
			grafico->graficoIntroBatalha (fundobatalha, barra, barraop, vs, barraeu, treinador, rivais, font80, instancemusicabatalha);
		}

		if(batalha == 1){
			grafico->graficoBatalha (ev, fundobatalha, barraopcao, hp, hp2, sem, font, opcoes, treinadorcosta, pokebola, pokebolaa, pokemons, pokemonsb, animacaoataques, barra, barraop, vs, barraeu, treinador, rivais, font80, font64, instancemusicabatalha, sompokemon);
			
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
				tecla->teclasBatalha (ev, escolha, somataques);
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

    /*//Testando Mochila e Item
    Item* cura = new Item("cura", 500.0);
    Insignia* dragao = new Insignia("dragão", "fogo");

    Mochila* m = new Mochila();

    m->adicionarItem(cura);
    m->adicionarInsignia(dragao);
    m->informarItens();
    m->informarInsignias();
    
    //Testando Pokemons
    PokemonEletrico* p = new PokemonEletrico("pikachu", 1, 50.0, 1000.0, 1.0, "ataque rapido", "ataque do trovao");
    PokemonEletrico* r = new PokemonEletrico("raichu", 1, 50.0, 1000.0, 1.0, "ataque rapido", "ataque do trovao");

    std::cout << p->imprimirInfos();
    std::cout << r->imprimirInfos();
    
    p->atacar(r, "ataque rapido");

    std::cout << p->imprimirInfos();
    std::cout << r->imprimirInfos();

    delete cura;
    delete p;
    delete r;
    delete m;

    return 0;
}*/