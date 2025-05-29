/*#ifndef GRAFICO_HPP
#define GRAFICO_HPP

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

class Grafico {
    public:

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
        
        void graficoMenu(ALLEGRO_BITMAP *fundomenu, ALLEGRO_FONT *font, ALLEGRO_FONT *font64,
                        ALLEGRO_SAMPLE_INSTANCE *instancemusicamenu);

        void graficoTpokedex (ALLEGRO_BITMAP *pokedex, ALLEGRO_BITMAP *pokemons[],
                            ALLEGRO_SAMPLE *sompokemon[], ALLEGRO_BITMAP *tipos[], ALLEGRO_FONT *font80,
                            ALLEGRO_FONT *font64, ALLEGRO_FONT *font40);

        void graficoMapa (ALLEGRO_BITMAP *fundomapa[], ALLEGRO_BITMAP *caixa, ALLEGRO_FONT *font80,
                        ALLEGRO_FONT *font);

        void graficoTcard (ALLEGRO_BITMAP *cards[], ALLEGRO_FONT *font80, ALLEGRO_FONT *font64, ALLEGRO_FONT *font);

        void graficoBag (ALLEGRO_BITMAP *itens[], ALLEGRO_FONT *font80, ALLEGRO_FONT *font64);

        void graficoTequipe (ALLEGRO_BITMAP *equipe[], ALLEGRO_BITMAP *pokemonsi[], ALLEGRO_FONT *font80,
                            ALLEGRO_FONT *font);

        void graficoIntroBatalha (ALLEGRO_BITMAP *fundobatalha, ALLEGRO_BITMAP *barra, 
                            ALLEGRO_BITMAP *barraop, ALLEGRO_BITMAP *vs, ALLEGRO_BITMAP *barraeu,
                            ALLEGRO_BITMAP *treinador, ALLEGRO_BITMAP *rivais[], ALLEGRO_FONT *font80,
                            ALLEGRO_SAMPLE_INSTANCE *instancemusicabatalha);

        void graficoBatalha (ALLEGRO_EVENT ev, ALLEGRO_BITMAP *fundobatalha, ALLEGRO_BITMAP *barraopcao,
                            ALLEGRO_BITMAP *hp, ALLEGRO_BITMAP *hp2, ALLEGRO_BITMAP *sem, ALLEGRO_FONT *font,
                            ALLEGRO_BITMAP *opcoes, ALLEGRO_BITMAP *treinadorcosta,
                            ALLEGRO_BITMAP *pokebola, ALLEGRO_BITMAP *pokebolaa,
                            ALLEGRO_BITMAP *pokemons[], ALLEGRO_BITMAP *pokemonsb[],
                            ALLEGRO_BITMAP *animacaoataques[], ALLEGRO_BITMAP *barra,
                            ALLEGRO_BITMAP *barraop, ALLEGRO_BITMAP *vs, ALLEGRO_BITMAP *barraeu,
                            ALLEGRO_BITMAP *treinador, ALLEGRO_BITMAP *rivais[], ALLEGRO_FONT *font80,
                            ALLEGRO_FONT *font64, ALLEGRO_SAMPLE_INSTANCE *instancemusicabatalha,
                            ALLEGRO_SAMPLE *sompokemon[]);

};

#endif*/