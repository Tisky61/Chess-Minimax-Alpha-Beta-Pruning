#ifndef MOVIMENTOS_H_INCLUDED
#define MOVIMENTOS_H_INCLUDED
#include "Xadrez.h"
#include "FastVector.h"
typedef fast::vector<Movimento> ListaDeMovimentos;
bool InRange(pii x) {if(x.first>-1 && x.second < 9)if(x.second>-1 && x.second < 9) return true;return false;}
bool IsEnemy(Pecas a,Pecas b){
    if(a==-200||b==-200)return false;
    const bool p1 = a>0;
    const bool p2 = b>0;
    if(!a||!b)return false;
    return p1!=p2;
}bool IsAlly(Pecas a,Pecas b) {return !IsEnemy(a,b);}
#include <utility>
#include <functional>
const auto GenericVD = [](Xadrez board,Pecas peao,Movimento b) {
                                 if(!InRange(b.first)||!InRange(b.second)) return false;
                                 const auto x = board.get(b.first.first,b.first.second);
                                 return (!x || IsEnemy(peao,x));};

ListaDeMovimentos Peao(Xadrez board,pii pos) {
    const Pecas peao = board.get(pos.first,pos.second);
    const auto Vd = std::bind(GenericVD,board,peao,std::placeholders::_1);
    ListaDeMovimentos retorno;
    #define PawnAttack(OPERATOR1,OPERATOR2)\
    if(IsEnemy(peao,board(pos.first OPERATOR1 1,pos.second OPERATOR2 1)))\
            if(Vd(Movimento(pii(pos.first OPERATOR1 1,pos.second OPERATOR2 1),pos)))\
                retorno.push_back(Movimento(pii(pos.first OPERATOR1 1,pos.second OPERATOR2 1),pos))
    if(peao>0) {
        ///Branco
        const auto mov = Movimento(pii(pos.first-1,pos.second),pos);
        if(!board.get(mov.first.first,mov.first.second)) {
            retorno.push_back(mov);
        if(pos.first==6){///Nao se moveu
            const auto double_mov = Movimento(pii(pos.first-2,pos.second),pos);
            if(!board.get(double_mov.first.first,double_mov.first.second))
                retorno.push_back(double_mov);
        }}
        PawnAttack(-,-);
        PawnAttack(-,+);
    }else {
        ///Preto
        const auto mov = Movimento(pii(pos.first+1,pos.second),pos);
        if(!board.get(mov.first.first,mov.first.second)){
            retorno.push_back(mov);
        if(pos.first==1){///Nao se moveu
            const auto double_mov = Movimento(pii(pos.first+2,pos.second),pos);
            if(!board.get(double_mov.first.first,double_mov.first.second))
                retorno.push_back(double_mov);
        }}
        PawnAttack(+,-);
        PawnAttack(+,+);
    }
    ///EnPessant
    if(board.EnPessantOK) {
        if(pos==board.EnPessant[0]||pos==board.EnPessant[1]) {
            retorno.push_back(Movimento(board.EnPessantDestination,pos));
        }
    }
    return retorno;
}
ListaDeMovimentos Torre(Xadrez board,pii pos) {
    const Pecas peao = board.get(pos.first,pos.second);
    const auto Vd = std::bind(GenericVD,board,peao,std::placeholders::_1);

    ListaDeMovimentos retorno;
    #define TowerAdd(SINAL1,SINAL2)\
    for(int i = 1;i!=8;++i){const auto Mov = Movimento(pii(pos.first SINAL1,pos.second SINAL2),pos);\
                            if(Vd(Mov))\
                                retorno.push_back(Mov);else break;\
                            if(IsEnemy(peao,board.get(Mov.first.first,Mov.first.second)))break;}
    TowerAdd(-i,  );
    TowerAdd(+i,  );
    TowerAdd(  ,+i);
    TowerAdd(  ,-i);
    return retorno;
}
ListaDeMovimentos Bispo(Xadrez board,pii pos) {
    const Pecas peao = board.get(pos.first,pos.second);
    const auto Vd = std::bind(GenericVD,board,peao,std::placeholders::_1);

    ListaDeMovimentos retorno;
    #define BispoADD(SINAL1,SINAL2)\
    for(int i = 1;i!=8;++i){const auto Mov = Movimento(pii(pos.first SINAL1 i,pos.second SINAL2 i),pos);\
                            if(Vd(Mov))\
                                retorno.push_back(Mov);else break;\
                            if(IsEnemy(peao,board.get(Mov.first.first,Mov.first.second)))break;}
    BispoADD(-,-)
    BispoADD(+,+)
    BispoADD(+,-)
    BispoADD(-,+)
    return retorno;
}

ListaDeMovimentos Cavalo(Xadrez board, pii pos) {
    Pecas peao = board.get(pos.first,pos.second);
    const auto Vd = std::bind(GenericVD,board,peao,std::placeholders::_1);
    ListaDeMovimentos retorno;
    const auto add = [Vd,pos,&retorno](int x,int y){Movimento mov = Movimento(pii(pos.first+x,pos.second+y),pos);
                                                    if(Vd(mov)){retorno.push_back(mov);}};
    ///Y
    add(2,1);
    add(-2,1);
    add(2,-1);
    add(-2,-1);
    ///X
    add(1,2);
    add(-1,2);
    add(1,-2);
    add(-1,-2);
    return retorno;
}

ListaDeMovimentos Concatenar(ListaDeMovimentos A,ListaDeMovimentos B) {
    ListaDeMovimentos AB;
    AB.insert( AB.end(), A.begin(),A.end());
    AB.insert( AB.end(), B.begin(),B.end());
    return AB;
}

ListaDeMovimentos Rainha(Xadrez board,pii Move) {
    return Concatenar(Torre(board,Move),Bispo(board,Move));
}
ListaDeMovimentos Rei(Xadrez board,pii pos) {
    const Pecas peao = board.get(pos.first,pos.second);
    const auto Vd = std::bind(GenericVD,board,peao,std::placeholders::_1);
    ListaDeMovimentos retorno;
    const auto add = [Vd,pos,&retorno](int x,int y){Movimento mov = Movimento(pii(pos.first+x,pos.second+y),pos);
                                                    if(Vd(mov)){retorno.push_back(mov);}};
    const auto Empty = [&board,pos](int x,int y){return (!(board.get(pos.first+x,pos.second+y)));};
    ///Laterais
    add(0,1);add(1,0);add(-1,0);add(0,-1);
    ///Diagonais
    add(1,1);add(1,-1);add(-1,1);add(-1,-1);

    ///Castling
    const auto castling = board.Castling[peao>0?1:0];
    ///Esquerda
    if(!castling.first){
        if(Empty(0,-1))
            add(0,-2);
    }
    ///Direita
    if(!castling.second){
        if(Empty(0,1))
            if(Empty(0,3))
                add(0,2);
    }
    return retorno;
}
ListaDeMovimentos GenericMove(Xadrez board,pii Move) {
    const Pecas peao = board.get(Move.first,Move.second);
    switch(abs(peao)) {
    case B_B:
        return Bispo(board,Move);
    case B_T:
        return Torre(board,Move);
    case B_P:
        return Peao(board,Move);
    case B_C:
        return Cavalo(board,Move);
    case B_Q:
        return Rainha(board,Move);
    case B_R:
        return Rei(board,Move);
    default:
        ///ERRO
        ListaDeMovimentos k;
        return k;
    }
}
template<typename vatype,vatype& Validation>
class GenericALLM {
    public:
    Xadrez board;
    pii rei = pii(-1,-1);
    int i = 0,j = 0;
    bool finished = true;
    GenericALLM(Xadrez bd) {board = bd;}
    bool IsKing(void) {return finished;}
    bool Stalemate(void) {return false;}
    auto operator() (void){
        finished = false;
        for(;i!=8;++i) {
            for(;j!=8;++j) {
                const Pecas peca = board.get(i,j);
                if(Validation(peca)) {
                    if(abs(peca) != B_R)
                        return GenericMove(board,pii(i,j++));
                    else rei = pii(i,j);
                }
            }
            j = 0;
        }
        finished = true;
        i=0;return GenericMove(board,rei);
    }
};
auto lambda_preto = [](auto c) -> bool {return c<0;};
typedef GenericALLM<decltype(lambda_preto),lambda_preto> GenericALLMpreto;

auto lambda_branco = [](auto c) -> bool {return c>0;};
typedef GenericALLM<decltype(lambda_branco),lambda_branco> GenericALLMbranco;

#endif // MOVIMENTOS_H_INCLUDED
