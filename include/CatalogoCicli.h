#ifndef CATALOGO_CICLI_H
#define CATALOGO_CICLI_H

#include <cstddef>
#include <string>

// Concetti del dominio (non UI): classificano i cicli
enum class CategoriaLavoro { Interno, Esterno, Cartongesso };

enum class SottoCategoriaLavoro {
    InternoCivile, InternoIndustriale,
    EsternoCivile, EsternoIndustriale,
    Cart_Contropareti, Cart_ParetiDivisorie,
    Cart_Controsoffitti, Cart_FinituraMuratura
};

// Dato statico: un ciclo in catalogo (nome + prezzo base + classificazione)
struct CicloInfo {
    const char* nome;
    double prezzoMq;
    CategoriaLavoro categoria;
    SottoCategoriaLavoro sottocategoria;
};

// Accesso controllato al catalogo (l'array resta nascosto nel .cpp)
std::size_t getNumeroCicli();
const CicloInfo& getCiclo(std::size_t index);

// Helper utili a UI/main
std::string getNomeCicloDaIndice(int index);
bool isCartongessoIndex(int index);

#endif
