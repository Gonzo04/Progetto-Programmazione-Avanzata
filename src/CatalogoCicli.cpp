#include "CatalogoCicli.h"
#include <stdexcept>

/*
    Array statico "privato" del modulo:
    -static perchè visibile solo in questo file
    -per aggiungere un ciclo nuovo basta aggiungerlo qui
*/
static const CicloInfo CICLI_LAVORAZIONI[] = {
    {"Idropittura traspirante", 5.00, CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},
    {"Idropittura lavabile acrilica", 6.50, CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},
    {"Smalto all'acqua interni", 8.50, CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},
    {"Ciclo decorativo", 15.00, CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},
    {"Carta da parati", 18.00, CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},
    {"Smalto industriale", 7.00, CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoIndustriale},
    {"Traspirante a spruzzo", 4.00, CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoIndustriale},

    {"Intonachino colorato", 18.00, CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},
    {"Rasatura armata + Intonachino", 25.00, CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},
    {"Rasatura panno + Lavabile", 22.00, CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},
    {"Cappotto termico", 50.00, CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},
    {"Lavabile autopulente esterno", 10.00, CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},
    {"Lavabile esterno industriale", 9.00, CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoIndustriale},

    {"Controparete isolata", 35.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Contropareti},
    {"Controparete non isolata", 28.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Contropareti},
    {"Parete divisoria isolata", 42.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_ParetiDivisorie},
    {"Parete divisoria non isolata", 38.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_ParetiDivisorie},
    {"Parete Idro (bagni)", 45.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_ParetiDivisorie},
    {"Parete REI (antincendio)", 55.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_ParetiDivisorie},
    {"Controsoffitto standard", 32.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Controsoffitti},
    {"Controsoffitto fonoassorbente", 48.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Controsoffitti},
    {"Controsoffitto fibra minerale", 30.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Controsoffitti},
    {"Rasatura gesso su muratura", 12.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_FinituraMuratura}
};

// Nunmero elementi calcolato a compile-time
static constexpr  std::size_t NUM_CICLI = sizeof(CICLI_LAVORAZIONI) / sizeof(CicloInfo);

std::size_t getNumeroCicli() {
    return NUM_CICLI;
}

const CicloInfo& getCiclo(std::size_t index) {
    // Qui controllo l'indice
    if (index >= NUM_CICLI) throw std::out_of_range("Indice ciclo fuori range");
    return CICLI_LAVORAZIONI[index];
}

std::string getNomeCicloDaIndice(int index) {
    /*
     La UI lavora con int dato che le scelte sono int
     Faccio il check e poi restituisco nome del ciclo

    Confronto con static_cast<int>(NUM_CICLI) perchè NUM_CICLI è size_t (unsigned)
    evitando confronti signed/unsigned
    */
    if (index < 0 || index >= static_cast<int>(NUM_CICLI)) {
        throw std::out_of_range("Indice ciclo fuori range");
    }
    return CICLI_LAVORAZIONI[index].nome;
}

bool isCartongessoIndex(int index) {
    if (index < 0 || index >= static_cast<int>(NUM_CICLI)) return false;
    return CICLI_LAVORAZIONI[index].categoria == CategoriaLavoro::Cartongesso;
}
