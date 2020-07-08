#ifndef XADREZ_H_INCLUDED
#define XADREZ_H_INCLUDED
#include <stdexcept>

#define preto false
#define branco true
#define JOGADA_PERFEITA 20000
typedef unsigned char smallint;
typedef std::pair<smallint,smallint> pii;
typedef std::pair<pii,pii> Movimento;
enum Pecas {ERROR_INVALID = -200,P_R = -6,P_Q = -5,P_B = -4,P_C = -3,P_T = -2,P_P = -1,vz = 0,
            B_R =  6,B_Q =  5,B_B =  4,B_C =  3,B_T =  2,B_P =  1};
class Xadrez {
public:
    Pecas tabuleiro[8][8];
    bool Dominio[8][8];
    std::pair<bool,bool> Castling[2];///Castling branco: 0, Castling preto: 1
    bool turno;
    bool Castled[2];
    /**EN PESSANT**/
    pii EnPessant[2];
    pii EnPessantDestination;
    pii EnPessantTarget;
    bool EnPessantOK = false;

    int pontuacao = 0;
    void domzero(void) {memset(&Dominio[0][0],0,sizeof(Dominio));}
    void domset(int x,int y){Dominio[x][y]=true;}void domset(pii a)   {domset (a.first,a.second);}
    void domzero(int x,int y){Dominio[x][y]=false;}void domzero(pii a){domzero(a.first,a.second);}

     int Valor(Pecas c,int coluna = 0,int linha = 4) {
        static const int TabelaPeao[] = {10,10,11,12,12,11,10,10};
        static const int TabelaBranco[] = {100,12,12,11,11,11,10,10};
        static const int TabelaPreto[] =  {10,10,11,11,11,12,12,100};

        //////////////////////////////////////////////////////////
        /////Consertar desbalanceamento entre branco e preto /////
        //////////////////////////////////////////////////////////

        switch(abs(c)){case B_R:return 20000;case B_Q:return 100;case B_B: return 35;
                       case B_C:return 35;case B_T:return 53;case B_P:{
                               const int c = TabelaPeao[coluna];
                               int ln;
                               if(c>0) { ///Branco
                                    ln = TabelaPreto[linha];
                               }else ln = TabelaBranco[linha];
                               return (c+ln)>>1;
                           }
                           break;}
    }

    bool IsSect(int x,int y) {return Dominio[x][y];}

    void KillPeca(pii posicao) {
        if(tabuleiro[posicao.first][posicao.second]>0) {
                pontuacao -= Valor(tabuleiro[posicao.first][posicao.second],posicao.second);
        }else {
                pontuacao += Valor(tabuleiro[posicao.first][posicao.second],posicao.second);
        }
        tabuleiro[posicao.first][posicao.second] = vz;
    }

    int Reiniciar(void) {
        const static Pecas TabuleiroModelo[8][8] {{P_T,P_C,P_B,P_R,P_Q,P_B,P_C,P_T},
                                                  {P_P,P_P,P_P,P_P,P_P,P_P,P_P,P_P},
                                                  {vz ,vz ,vz ,vz ,vz ,vz ,vz ,vz },
                                                  {vz ,vz ,vz ,vz ,vz ,vz ,vz ,vz },
                                                  {vz ,vz ,vz ,vz ,vz ,vz ,vz ,vz },
                                                  {vz ,vz ,vz ,vz ,vz ,vz ,vz ,vz },
                                                  {B_P,B_P,B_P,B_P,B_P,B_P,B_P,B_P},
                                                  {B_T,B_C,B_B,B_R,B_Q,B_B,B_C,B_T}};
        turno = branco;///Coloca o turno para o branco
        memcpy(&tabuleiro[0][0],&TabuleiroModelo[0][0],sizeof(tabuleiro));///Reinicia o tabuleiro
        memset(Castling,0,sizeof(Castling));///Zera Castling
        EnPessantOK = false;///Desabilita EnPessant
        domzero();///Zera o Dominio
    }
    Xadrez(){Reiniciar();}

    int EvaluateBlack(void) {
        int total = 0;
        int bispo = 0;
        for(int i = 0; i != 8;++i){
            for(int j = 0; j != 8;++j){
            const auto y = tabuleiro[j][i];
                if(y<0)
                    switch(y){case P_B:++bispo;default:total+=Valor(y,i,j);}
            }
        }
        if(Castled[0]) total += 3;
        return total;
    }
    int EvaluateWhite(void) {
        int total = 0;
        int bispo = 0;
        for(int i = 0; i != 8;++i){
            for(int j = 0; j != 8;++j){
                const auto y = tabuleiro[j][i];
                if(y>0)
                    switch(y){case B_B:++bispo;default:total+=Valor(y,i,j);}
            }
        }
        if(Castled[1]) total += 3;
        return total;
    }
    int EvaluateBoard(bool position) {
        const auto White = EvaluateWhite();
        const auto Black = EvaluateBlack();
        if(position==branco){
            auto Score = White-Black;
            if(Score>20000){Score = 200000 + Black;}
          //  if(Score<-50) {printf("{%d %d}",White,Black);}
            return Score;
        } else {
            auto Score = Black-White;
            if(Score>20000){Score = 200000 + White;}
         //   if(Score<-50) {printf("{%d %d}",White,Black);}
            return Score;
        }
    }
    Pecas get (int i,int j) {if(i>-1&&i<8&&j>-1&&j<8)return tabuleiro[i][j];else{return ERROR_INVALID;throw std::out_of_range ("received a big value");}}
    auto operator()(int i,int j){return get(i,j);}
    bool Move(Movimento b) {///Importante!!!Adicionar proibicao de castling
        if(b.first.first<-1||b.first.first>7||b.first.second<0||b.first.second>7||b.second.first<0||b.second.first>7||b.second.second<0||b.second.second>7||
           b.first.first<-1||b.first.first>7||b.first.second<0||b.first.second>7||b.second.first<0||b.second.first>7||b.second.second<0||b.second.second>7){
            printf("fora! (%d %d)<%d %d>",b.first.first,b.second.second,b.second.first,b.second.second);
            return false;
        }
        const auto peca = get(b.second.first,b.second.second);
        const auto inimigo = get(b.first.first,b.first.second);
        switch(abs(peca)) {
            case B_R:
                ///Executar o castling
                if(peca>0) {///Branco
                    Castling[1] = std::pair<bool,bool>(true,true);
                    Castled[1] = true;
                    const int castling_identifier = b.first.second-b.second.second;
                    if(abs(castling_identifier)==2) {
                        ///Castling
                        if(castling_identifier>0){///Direita
                            tabuleiro[7][7] = vz;
                            tabuleiro[7][4] = B_T;
                        }else {                   ///Esquerda
                            tabuleiro[7][0] = vz;
                            tabuleiro[7][2] = B_T;
                        }
                        goto normal;
                    }
                    goto normal;
                }else {///Preto
                    Castling[0] = std::pair<bool,bool>(true,true);
                    Castled[0] = true;
                    const int castling_identifier = b.first.second-b.second.second;
                    if(abs(castling_identifier)==2) {
                        ///Castling
                        if(castling_identifier>0){///Direita
                            tabuleiro[0][7] = vz;
                            tabuleiro[0][4] = P_T;
                        }else {///Esquerda
                            tabuleiro[0][0] = vz;
                            tabuleiro[0][2] = P_T;
                        }
                        goto normal;
                    }
                    goto normal;
                }
            break;
            case B_T:
                ///Desativa o Castling
                if(peca>0) {///Branco
                    if(b.second.first==7&&b.second.second==7) {
                        Castling[1].second = true;
                    }else if(b.second.first==7&&b.second.second==0) {
                        Castling[1].first  = true;
                    }
                }else {///Preto
                    if(b.second.first==0&&b.second.second==7) {
                        Castling[0].second = true;
                    }else if(b.second.first==0&&b.second.second==0) {
                        Castling[0].first  = true;
                    }
                }
            goto normal;
            case B_P:
                ///Detectar se esta fazendo um EnPessant
                if(EnPessantOK) {
                    if(b.first==EnPessantDestination && (b.second==EnPessant[0]||b.second==EnPessant[1])) {
                        KillPeca(EnPessantTarget);
                        goto normal;
                    }
                }

                ///Promocao para rainha

                if(peca>0) {///Branco
                    if(!b.first.first) {
                        tabuleiro[b.first.first][b.first.second] = B_Q;///Queening
                        KillPeca(b.second);
                        pontuacao += Valor(B_Q);
                        break;
                    }
                }else {///Preto
                    if(b.first.first==7) {
                        tabuleiro[b.first.first][b.first.second] = P_Q;///Queening
                        KillPeca(b.second);
                        pontuacao -= Valor(B_Q);
                        break;
                    }
                }

                ///Habilitar o En Passant
                if(abs(b.first.first-b.second.first)==2) {
                    EnPessantOK = true;
                    EnPessant[0] = pii(b.first.first,b.first.second+1);
                    EnPessant[1] = pii(b.first.first,b.first.second-1);
                    EnPessantDestination = pii(b.second.first+((b.first.first-b.second.first)>>1),b.second.second);
                    EnPessantTarget = b.first;
                    goto EnPessantGoto;
                }

            default:
                normal:
                EnPessantOK = false;
                EnPessantGoto:
                    if(b.first.second<0 ||b.first.second>8||b.first.first<0 ||b.first.first>8) break;
                    if(b.second.second<0 ||b.second.second>8||b.second.first<0 ||b.second.first>8) break;
                    if(tabuleiro[b.first.first][b.first.second]>0){
                        /**Branco**/
                        pontuacao += Valor(tabuleiro[b.first.first][b.first.second],b.first.second);
                    }else /**Preto**/ pontuacao -= Valor(tabuleiro[b.first.first][b.first.second],b.first.second);
                    tabuleiro[b.first.first][b.first.second] = tabuleiro[b.second.first][b.second.second];
                    KillPeca(b.second);
        }
        if(abs(inimigo) == B_R) return true;
        return false;
    }
};

#endif // XADREZ_H_INCLUDED
