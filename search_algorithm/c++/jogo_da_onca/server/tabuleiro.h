#ifndef __TABULEIRO_H__
#define __TABULEIRO_H__

// Add bridge to allow C++ code to call C functions
#ifdef __cplusplus
extern "C" {
#endif

void tabuleiro_conecta(int, char**);
void tabuleiro_envia(char*);
void tabuleiro_recebe(char*);

#ifdef __cplusplus
}
#endif

#endif
