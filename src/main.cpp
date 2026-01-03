#include <iostream>
#include <memory>
#include <stdexcept>

#include "Preventivo.h"
#include "ListinoPrezzi.h"
#include "CalcolatorePreventivo.h"
#include "RegolaTinteggiatura.h"
#include "RegolaCartongesso.h"
#include "Utils.h"
#include "GestioneInputUI.h"
#include "CatalogoCicli.h"
#include "ListinoDefault.h"
#include "IdPreventivoGenerator.h"
#include "SalvataggioPreventivo.h"

static constexpr double MQ_MAX_REALISTICI = 1000000.0;

struct ContestoApp {
    std::shared_ptr<ListinoPrezzi> listino;
    RegolaTinteggiatura regolaTinteggiatura;
    RegolaCartongesso regolaCartongesso;
    CalcolatorePreventivo calcolatore;
    GradoDifficolta grado;
};

static std::string chiediNomeCliente() {
    std::string cliente;

    while (true) {
        std::cout << "Inserisci nome e cognome: ";
        std::string linea;
        if (!std::getline(std::cin, linea)) return "";

        cliente = trim(linea);

        if (cliente.empty()) {
            std::cout << "Il nome non puo' essere vuoto.\n";
            continue;
        }

        if (!nomeClienteValido(cliente)) {
            std::cout << "Nome non valido: usa solo lettere, spazi, apostrofo (') o trattino (-).\n";
            continue;
        }
        break;
    }

    return cliente;
}

static Preventivo inizializzaPreventivo(ContestoApp& ctx) {
    ctx.listino = std::make_shared<ListinoPrezzi>();

    caricaListinoDefault(*ctx.listino);
    std::cout << "Listino caricato correttamente.\n\n";

    std::string id = generaIdPreventivo();
    std::cout << "ID preventivo assegnato automaticamente: " << id << "\n\n";

    std::string cliente = chiediNomeCliente();
    if (cliente.empty()) {
        throw std::runtime_error("Input terminato.");
    }

    ctx.grado = GestioneInputUI::chiediGradoDifficolta();
    return {id, cliente, ctx.grado};
}

static void inserisciVoci(Preventivo& preventivo, ContestoApp& ctx) {
    CategoriaLavoro catCorr = {};
    SottoCategoriaLavoro sottoCatCorr = {};
    bool haCat = false;
    bool continua = true;

    while (continua) {
        if (!haCat) {
            catCorr = GestioneInputUI::chiediCategoriaLavoro();
            sottoCatCorr = GestioneInputUI::chiediSottoCategoriaLavoro(catCorr);
            haCat = true;
        }

        int idx = GestioneInputUI::menuCicliPerCategoria(catCorr, sottoCatCorr);

        if (idx < 0) {
            haCat = false;
            continue;
        }

        std::string nomeCiclo = getNomeCicloDaIndice(idx);
        bool isCartongesso = isCartongessoIndex(idx);

        double mq = 0.0;
        while (true) {
            mq = leggiDouble("Inserisci mq per \"" + nomeCiclo + "\": ");
            if (mq > 0 && mq <= MQ_MAX_REALISTICI) break;
            std::cout << "Valore non valido (deve essere > 0 e <= " << MQ_MAX_REALISTICI << ")\n";
        }

        try {
            if (isCartongesso) ctx.calcolatore.setRegola(&ctx.regolaCartongesso);
            else ctx.calcolatore.setRegola(&ctx.regolaTinteggiatura);

            ctx.calcolatore.aggiungiLavoro(preventivo, nomeCiclo, mq, ctx.listino, ctx.grado);
            std::cout << " -> Aggiunta voce: " << nomeCiclo << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Errore creazione voce: " << e.what() << "\n";
        }

        continua = chiediConferma("\nAggiungere altra voce?");
    }
}

int main() {
    std::cout << "=== PREVENTIVATORE EDILCOLOR ===\n\n";

    try {
        ContestoApp ctx;
        Preventivo preventivo = inizializzaPreventivo(ctx);

        inserisciVoci(preventivo, ctx);

        preventivo.ordinaPerNome();

        std::cout << "\n=== RIEPILOGO PREVENTIVO ===\n";
        std::cout << preventivo.riepilogo() << std::endl;

        std::string baseName = preventivo.getId();
        if (baseName.empty()) baseName = "preventivo";

        bool ok = salvaPreventivoConcorrente(preventivo, baseName);

        if (ok) {
            std::cout << "\nSalvataggio CSV e TXT completato! File: " << baseName << "\n";
        } else {
            std::cout << "\nSalvataggio NON riuscito \n Controlla il percorso.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Errore: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Premi INVIO per uscire";
    std::string dummy;
    std::getline(std::cin, dummy);
    return 0;
}
