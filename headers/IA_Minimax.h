///Mov rainha padrao Processando Threads sendo usados: 7...Estimativa:2[Real:-7]<Checks:1121709> time:629.000000
#ifndef IA_MINIMAX_H_INCLUDED
#define IA_MINIMAX_H_INCLUDED
#include "movimentos.h"
#include <thread>
#include <future>
#include <atomic>
#define PROFUNDIDADE 5
template<typename R>bool is_ready(std::future<R> const& f){ return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }
int __Escolha(Xadrez board,int profundidade,int alpha,int beta,bool maximizing_player) {
    if(!profundidade) return board.EvaluateBoard(preto);///Hardcoded!!!Atencao!!!
    if(maximizing_player) {
        int valor = -INT_MAX;
        GenericALLMpreto GenMove(board);
        if(GenMove.Stalemate()) return 0;
        inicio:
            ///HARDCODED AQUI
            for(auto& x : GenMove()) {
                Xadrez newboard = board;
                if(newboard.Move(x)) return JOGADA_PERFEITA * (PROFUNDIDADE - profundidade);
                valor = std::max(valor,__Escolha(newboard,profundidade-1,alpha,beta,false));
                alpha = std::max(alpha,valor);
                if(alpha>=beta) break; ///Corta a arvore
            }
        if(!GenMove.IsKing()) goto inicio;
        return valor;
    }else {
        int valor = INT_MAX;
            GenericALLMbranco GenMove(board);///HARDCODED AQUI TAMBEM
            if(GenMove.Stalemate()) return 0;
            iniciob:
                for(auto& x : GenMove()) {
                    Xadrez newboard = board;
                    if(newboard.Move(x)) return -(JOGADA_PERFEITA * (PROFUNDIDADE - profundidade));
                    valor = std::min(valor,__Escolha(newboard,profundidade-1,alpha,beta,true));
                    beta = std::min(valor,beta);
                    if(beta<=alpha) break; ///Corta a arvore
                }
            if(!GenMove.IsKing()) goto iniciob;
        return valor;
    }
}
inline void Empty(){}
void Jogar(std::promise<Movimento> movimento_futuro,Xadrez tabuleiro) {
    const static unsigned int AvailableCores = std::thread::hardware_concurrency()-1?std::thread::hardware_concurrency()-1:1;
    printf("Processando Threads sendo usados: %u...",AvailableCores);
    const auto begin_time = std::clock();
    int jg = -INT_MAX;
    Movimento dec;
    GenericALLMpreto GenMove(tabuleiro);
    Movimento fts[AvailableCores];
    std::future<int> futuro[AvailableCores];
    bool Indisponivel[8];memset(Indisponivel,0,sizeof(Indisponivel));
    int threads_disponiveis = AvailableCores;
    inicial:
        for(auto& x : GenMove()){///HARDCODED!!! IMPORTANTE
            Xadrez newboard = tabuleiro;
            newboard.Move(x);
            for(int i = 0; i != AvailableCores;++i) {
                if(!Indisponivel[i]){
                    Indisponivel[i] = true;
                    --threads_disponiveis;
                    futuro[i] = std::async(std::launch::async,__Escolha,newboard,PROFUNDIDADE-1,jg,INT_MAX,false);
                    fts[i] = x;
                    break;
                }
            }
            if(!threads_disponiveis) {
                for(;;) {
                    for(int i = 0; i != AvailableCores;++i) {
                        if(is_ready(futuro[i])) {
                            Indisponivel[i] = false;
                            ++threads_disponiveis;
                            int resp = futuro[i].get();
                            if(resp>jg) {
                                jg = resp;
                                dec = fts[i];
                            }
                            goto continuar;
                        }
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(3));
                }
            }
            continuar:
            Empty();
        }
    if(!GenMove.IsKing()) goto inicial;
    else {
        if(threads_disponiveis!=AvailableCores) {
            for(;threads_disponiveis!=AvailableCores;) {
                    for(int i = 0; i != AvailableCores;++i) {
                    if(Indisponivel[i])
                        if(is_ready(futuro[i])) {
                            Indisponivel[i] = false;
                            ++threads_disponiveis;
                            int resp = futuro[i].get();
                            if(resp>jg) {
                                jg = resp;
                                dec = fts[i];
                            }
                        }
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(3));
                }
        }
    }
    movimento_futuro.set_value(dec);
    const auto end_time = std::clock();
    printf("Estimativa:%d[Real:%d]<Profundidade:%d> time:%f\n",jg,tabuleiro.EvaluateBoard(preto),(int)PROFUNDIDADE,float(end_time - begin_time));
}
#endif // IA_MINIMAX_H_INCLUDED
