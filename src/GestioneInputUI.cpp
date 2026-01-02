#include "GestioneInputUI.h"
#include "Utils.h"
#include "CatalogoCicli.h"

#include <iostream>
#include <vector>



GradoDifficolta GestioneInputUI::chiediGradoDifficolta() {
    while (true) {
        std::cout << "Seleziona la tipologia del cantiere:\n 1) Nuovo\n 2) Disabitato\n 3) Abitato\n";
        int scelta = leggiIntero("Scelta: ");
        if (scelta == 1) return GradoDifficolta::Nuovo;
        if (scelta == 2) return GradoDifficolta::Disabitato;
        if (scelta == 3) return GradoDifficolta::Abitato;
        std::cout << "Scelta fuori range.\n\n";
    }
}

CategoriaLavoro GestioneInputUI::chiediCategoriaLavoro() {
    while (true) {
        std::cout << "\nSeleziona il tipo di lavoro:\n 1) Interno\n 2) Esterno\n 3) Cartongesso\n";
        int scelta = leggiIntero("Scelta: ");
        switch (scelta) {
            case 1: return CategoriaLavoro::Interno;
            case 2: return CategoriaLavoro::Esterno;
            case 3: return CategoriaLavoro::Cartongesso;
            default: std::cout << "Scelta fuori range.\n";
        }
    }
}

SottoCategoriaLavoro GestioneInputUI::chiediSottoCategoriaLavoro(CategoriaLavoro categoria) {
    while (true) {
        std::cout << "\nSeleziona la sottocategoria:\n";
        if (categoria == CategoriaLavoro::Interno) {
            std::cout << " 1) Interno civile\n 2) Interno industriale\n";
            int scelta = leggiIntero("Scelta: ");
            if (scelta == 1) return SottoCategoriaLavoro::InternoCivile;
            if (scelta == 2) return SottoCategoriaLavoro::InternoIndustriale;
        } else if (categoria == CategoriaLavoro::Esterno) {
            std::cout << " 1) Esterno civile\n 2) Esterno industriale\n";
            int scelta = leggiIntero("Scelta: ");
            if (scelta == 1) return SottoCategoriaLavoro::EsternoCivile;
            if (scelta == 2) return SottoCategoriaLavoro::EsternoIndustriale;
        } else {
            std::cout << " 1) Contropareti\n 2) Pareti divisorie\n 3) Controsoffitti\n 4) Finitura su muratura\n";
            int scelta = leggiIntero("Scelta: ");
            if (scelta == 1) return SottoCategoriaLavoro::Cart_Contropareti;
            if (scelta == 2) return SottoCategoriaLavoro::Cart_ParetiDivisorie;
            if (scelta == 3) return SottoCategoriaLavoro::Cart_Controsoffitti;
            if (scelta == 4) return SottoCategoriaLavoro::Cart_FinituraMuratura;
        }
        std::cout << "Scelta non valida.\n";
    }
}

int GestioneInputUI::menuCicliPerCategoria(CategoriaLavoro categoria, SottoCategoriaLavoro sottocategoria) {
    std::cout << "\n--- Seleziona un ciclo di lavorazione ---\n";

    std::vector<int> indiciDisponibili;
    indiciDisponibili.reserve(getNumeroCicli());

    for (std::size_t i = 0; i < getNumeroCicli(); ++i) {
        const CicloInfo& ciclo = getCiclo(i);
        if (ciclo.categoria == categoria && ciclo.sottocategoria == sottocategoria) {
            indiciDisponibili.push_back(static_cast<int>(i));
            int voceMenu = static_cast<int>(indiciDisponibili.size());
            std::cout << " " << voceMenu << ") " << ciclo.nome
                      << " (" << ciclo.prezzoMq << " euro/mq)\n";
        }
    }

    if (indiciDisponibili.empty()) {
        std::cout << "Nessun ciclo disponibile per questa categoria.\n";
        return -1;
    }

    std::cout << " 0) Torna indietro\n";
    while (true) {
        int scelta = leggiIntero("Scelta: ");
        if (scelta == 0) return -1;
        if (scelta >= 1 && scelta <= static_cast<int>(indiciDisponibili.size())) {
            return indiciDisponibili[scelta - 1];
        }
        std::cout << "Scelta fuori range.\n";
    }
}
