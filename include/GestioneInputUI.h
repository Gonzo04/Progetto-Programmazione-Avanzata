#ifndef GESTIONEINPUTUI_H
#define GESTIONEINPUTUI_H

#include <string>
#include "GradoDifficolta.h"
#include "CatalogoCicli.h"   // enum CategoriaLavoro / SottoCategoriaLavoro

/*
    GestioneInputUI = strato di interfaccia testuale

    - chiede scelte all'utente (grado, categoria, sottocategoria, ciclo)
    - valida l'input (range) e ripropone il menu finché non è corretto

    Nota:
    - uso metodi static perché la classe non ha stato interno
*/

class GestioneInputUI {
public:

    static GradoDifficolta chiediGradoDifficolta();

    static CategoriaLavoro chiediCategoriaLavoro();

    static SottoCategoriaLavoro chiediSottoCategoriaLavoro(CategoriaLavoro categoria);

    /*
     Stampa l'elenco cicli filtrati per categoria e sottocategoria
     Ritorna:
     -indice reale nel CatalogoCIcli se scelta valida
     - se l'utente vuole tornare indeitro (scelta 0)
    */
    static int menuCicliPerCategoria(CategoriaLavoro categoria, SottoCategoriaLavoro sottocategoria);

};

#endif
