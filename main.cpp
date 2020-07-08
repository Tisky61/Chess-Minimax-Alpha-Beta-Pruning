#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <chrono>
#include "headers/IA_Minimax.h"
#include "headers/FastVector.h"
#define lff loadFromFile
Xadrez jogo;
sf::Color BlendColors(sf::Color a,sf::Color b) {return sf::Color(((a.r+b.r)>>1),((a.g+b.g)>>1),((a.b+b.b)>>1),((a.a+b.a)>>1));}
int main()
{
    printf("%d",jogo.EvaluateBoard(branco));
    sf::SoundBuffer PIMVbuffer;PIMVbuffer.loadFromFile("sounds/mov.wav");sf::Sound move_sound;move_sound.setBuffer(PIMVbuffer);
    jogo.Reiniciar();
    sf::RenderWindow window(sf::VideoMode(45*8+300, 45*8), "Chess Game!");
    sf::Texture pecas[6];pecas[0].lff("sprites/pawn.png");pecas[1].lff("sprites/tower.png");pecas[2].lff("sprites/horse.png");
    pecas[3].lff("sprites/priest.png");pecas[4].lff("sprites/queen.png");pecas[5].lff("sprites/king.png");
    pii SelectedPiece(-1,-1);
    bool ValidMovements[8][8];
    Movimento MovimentacaoInimiga = Movimento(pii(-1,-1),pii(-1,-1));
    memset(&ValidMovements[0][0],0,sizeof(ValidMovements));
    bool PromiseOpen = false;
    bool Focused = true;
    std::future<Movimento> movimento_ia;
    window.setFramerateLimit(120);
    while (window.isOpen())
    {
        #ifndef DISABLE_AI
            if(PromiseOpen) {
                if(is_ready(movimento_ia)) {
                    PromiseOpen = false;
                    MovimentacaoInimiga = movimento_ia.get();
                    jogo.Move(MovimentacaoInimiga);
                    move_sound.play();
                    const pii pretend = SelectedPiece;
                    memset(&ValidMovements[0][0],0,sizeof(ValidMovements));
                    if(jogo.tabuleiro[pretend.second][pretend.first]>0)
                        for(auto& x :GenericMove(jogo,pii(pretend.second,pretend.first))) {
                             ValidMovements[x.first.first][x.first.second] = true;
                        }
                    else SelectedPiece = pii(-1,-1);
                }
            }
        #endif
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::R) && Focused) {
                jogo.Reiniciar();
            }
            if(event.type == sf::Event::LostFocus)   Focused = false;
            if(event.type == sf::Event::GainedFocus) Focused = true;
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && Focused) {
                const sf::Vector2i pos = sf::Mouse::getPosition(window);
                const pii pretend = pii(floor(pos.x/45),floor(pos.y/45));
                if(pretend.second<0 ||pretend.second>8||pretend.first<0 ||pretend.first>8) break;
                if(
                   #ifndef DISABLE_AI
                        jogo.tabuleiro[pretend.second][pretend.first]>0
                   #else
                        SelectedPiece == pii(-1,-1)
                   #endif
                        ){
                        SelectedPiece = pretend;
                        memset(&ValidMovements[0][0],0,sizeof(ValidMovements));
                        int moves = 0;
                        for(auto& x :GenericMove(jogo,pii(pretend.second,pretend.first))) {
                            ValidMovements[x.first.first][x.first.second] = true;
                            ++moves;
                        }
                        printf("%d",moves);
                }else {///Limpeza
                    if(ValidMovements[pretend.second][pretend.first]
                           #ifndef DISABLE_AI
                           && !PromiseOpen
                           #endif
                       ) {///Jogar
                        jogo.Move(Movimento(pii(pretend.second,pretend.first),pii(SelectedPiece.second,SelectedPiece.first)));
                        move_sound.play();
                        #ifndef DISABLE_AI
                            std::promise<Movimento> promeca_mov;
                            movimento_ia = promeca_mov.get_future();
                            std::thread t(Jogar,std::move(promeca_mov),jogo);t.detach();
                            PromiseOpen = true;
                        #endif
                    }///Limpar selecao
                    memset(&ValidMovements[0][0],0,sizeof(ValidMovements));SelectedPiece = pii(-1,-1);
                    }
            }
        }

        window.clear(sf::Color(230,230,230));
        for(int i = 0; i != 8;++i) {
            for(int j = 0; j != 8; ++j){
                sf::RectangleShape quad(sf::Vector2f(45,45));
                quad.setPosition(i*45,j*45);
                if((i+j)&1)quad.setFillColor(sf::Color::White); else quad.setFillColor(sf::Color(210,105,30));
                if(pii(j,i)==MovimentacaoInimiga.first) quad.setFillColor(sf::Color(200,250,230,175));
                else if(pii(j,i)==MovimentacaoInimiga.second) quad.setFillColor(sf::Color(200,250,230,175));
                if(pii(i,j)==SelectedPiece){quad.setFillColor(BlendColors(quad.getFillColor(),sf::Color::Green));}
                if(ValidMovements[j][i]) quad.setFillColor(BlendColors(quad.getFillColor(),sf::Color::Magenta));
                window.draw(quad);
                if(jogo.tabuleiro[j][i]) {
                    sf::RectangleShape peca(sf::Vector2f(45,45));
                    peca.setTexture(&pecas[abs(jogo.tabuleiro[j][i])-1]);
                    peca.setPosition(i*45,j*45);
                    if(jogo.tabuleiro[j][i]<0)peca.setFillColor(sf::Color(20,20,20)); else peca.setFillColor(sf::Color(150,150,150));
                    window.draw(peca);
                }
            }
        }
        window.display();
    }

    return 0;
}
