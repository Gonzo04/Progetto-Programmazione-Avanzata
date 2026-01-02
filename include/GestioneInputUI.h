#ifndef GESTIONEINPUTUI_H
#define GESTIONEINPUTUI_H

#include <string>
#include "GradoDifficolta.h"
#include "CatalogoCicli.h"   // enum CategoriaLavoro / SottoCategoriaLavoro

class GestioneInputUI {
public:

    static GradoDifficolta chiediGradoDifficolta();

    static CategoriaLavoro chiediCategoriaLavoro();

    static SottoCategoriaLavoro chiediSottoCategoriaLavoro(CategoriaLavoro categoria);

    static int menuCicliPerCategoria(CategoriaLavoro categoria, SottoCategoriaLavoro sottocategoria);

};

#endif
