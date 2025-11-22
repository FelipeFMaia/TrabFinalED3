/* integrantes: Felipe Freitas Maia (15451360)*/
/* integrantes: Bruno Baremaker Moraes (15443854) */

#include <stdio.h>
#include "func.h"

void main() {

    int func;

    scanf("%i", &func);     //le o numero correspondente a funcao digitada pelo usuario

    switch(func){           //seleciona corretamente a funcao lida
        case 1:
            func1();
            break;

        case 2:
            func2();
            break;

        case 3:
            func3();
            break;

        case 4:
            func4();
            break;

        case 5:
            func5();
            break;

        case 6:
            func6();
            break;
        
        case 7:
            func7();
            break;
        
        case 8:
            func8();
            break;

        case 9:
            func9();
            break;

        case 10:
            func10();
            break;
        case 11:
            func11();
            break;
        case 12:
            func12();
            break;
    }
    return;         //termina o programa
}
