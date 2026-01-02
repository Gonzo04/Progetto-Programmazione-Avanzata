#ifndef GESTIONEINPUTUI_H
#define GESTIONEINPUTUI_H

#include <string>
#include <memory>
#include "ListinoPrezzi.h"
#include "GradoDifficolta.h"

// Definizioni pubbliche: chiunque includa questo header saprà cosa sono le Categorie.

enum class CategoriaLavoro {
    Interno, Esterno, Cartongesso
};

enum class SottoCategoriaLavoro {
    InternoCivile, InternoIndustriale, EsternoCivile, EsternoIndustriale,
    Cart_Contropareti, Cart_ParetiDivisorie, Cart_Controsoffitti, Cart_FinituraMuratura
};


// Struttura dati per i cicli.

struct CicloInfo {
    const char* nome;
    double prezzoMq;
    CategoriaLavoro categoria;
    SottoCategoriaLavoro sottocategoria;
};

class GestioneInputUI {
public:
    // Sostituisce la vecchia 'popolaListino'
    static void inizializzaListino(ListinoPrezzi& listino);

    // Funzioni di input utente (Sostituiscono le varie 'chiedi...')
    static GradoDifficolta chiediGradoDifficolta();
    static CategoriaLavoro chiediCategoriaLavoro();
    static SottoCategoriaLavoro chiediSottoCategoriaLavoro(CategoriaLavoro categoria);

    // Gestione del Menu (Sostituisce 'menuCicliPerCategoria')
    // Ritorna l'indice dell'array, o -1 se l'utente torna indietro.
    static int menuCicliPerCategoria(CategoriaLavoro categoria, SottoCategoriaLavoro sottocategoria);

    // [NUOVE FUNZIONI HELPER]
    // Siccome nasconderemo l'array dei nomi nel .cpp, ci servono funzioni
    // per recuperare le info a partire dall'indice scelto dall'utente.
    static std::string getNomeCicloDaIndice(int index);
    static bool isCartongessoIndex(int index);

    // Generazione ID (Sostituisce 'generaIdPreventivoAutomatico')
    static std::string generaIdPreventivoAutomatico();
};

#endif