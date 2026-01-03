#ifndef CATALOGO_CICLI_H
#define CATALOGO_CICLI_H

#include <cstddef>
#include <string>
/*
 CatalogoCili = elenco statico dei cicli/lavorazioni disponibili

 Idea:
    -questi dati sono di "dominio" non di interfaccia: nome, prezzo base, categoria, sottocategoria
    -li tengo in un array statico nel .cpp e utilizzo funzioni per accedervi in modo controllato

*/

// Categorie Principali
enum class CategoriaLavoro { Interno, Esterno, Cartongesso };

// SottoCategoria
enum class SottoCategoriaLavoro {
    InternoCivile, InternoIndustriale,
    EsternoCivile, EsternoIndustriale,
    Cart_Contropareti, Cart_ParetiDivisorie,
    Cart_Controsoffitti, Cart_FinituraMuratura
};

struct CicloInfo {
    const char* nome;
    double prezzoMq;
    CategoriaLavoro categoria;
    SottoCategoriaLavoro sottocategoria;
};

// Accesso controllato al catalogo
std::size_t getNumeroCicli();
const CicloInfo& getCiclo(std::size_t index);

// Helper utili a UI/main
std::string getNomeCicloDaIndice(int index);
bool isCartongessoIndex(int index);

#endif
