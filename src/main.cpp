#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <list>
#include <atomic>
#include <chrono>

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

std::mutex gConsoleMutex;

int main() {
    std::cout << "=== PREVENTIVATORE EDILCOLOR ===\n\n";

    auto listino = std::make_shared<ListinoPrezzi>();
    try {
        caricaListinoDefault(*listino);
        std::cout << "Listino caricato correttamente.\n\n";
    } catch (const std::exception& e) {
        std::cerr << "Errore caricamento listino: " << e.what() << std::endl;
        return 1;
    }

    std::string id = generaIdPreventivo();
    std::cout << "ID preventivo assegnato automaticamente: " << id << "\n\n";

    std::string cliente;
    while (true) {
        std::cout << "Inserisci nome e cognome: ";
        std::string linea;
        if (!std::getline(std::cin, linea)) return 1;

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

    GradoDifficolta grado = GestioneInputUI::chiediGradoDifficolta();
    Preventivo preventivo(id, cliente, grado);

    RegolaTinteggiatura regolaTinteggiatura;
    RegolaCartongesso  regolaCartongesso;
    CalcolatorePreventivo calcolatore;

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
            if (isCartongesso) calcolatore.setRegola(&regolaCartongesso);
            else calcolatore.setRegola(&regolaTinteggiatura);

            calcolatore.aggiungiLavoro(preventivo, nomeCiclo, mq, listino, grado);
            std::cout << " -> Aggiunta voce: " << nomeCiclo << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Errore creazione voce: " << e.what() << "\n";
        }

        continua = chiediConferma("\nAggiungere altra voce?");
    }

    preventivo.ordinaPerNome();

    std::cout << "\n=== RIEPILOGO PREVENTIVO ===\n";
    std::cout << preventivo.riepilogo() << std::endl;

    std::string baseName = preventivo.getId();
    if (baseName.empty()) baseName = "preventivo";

    // =======================
    // SALVATAGGIO (LIST + ATOMIC)
    // =======================
    std::cout << "\nSalvataggio in corso";

    // Commento da studente:
    // Qui uso std::list<std::thread> come "mini thread pool" estendibile:
    // ogni task è un thread (txt, csv). In futuro potrei aggiungere PDF, invio mail, ecc.
    std::list<std::thread> threadPool;

    std::atomic<int> taskFiniti(0);          // quanti task hanno terminato
    std::atomic<bool> salvataggioOk(true);   // diventa false se almeno un task fallisce
    constexpr  int NUM_TASK = 2;                  // txt + csv

    // Task 1: TXT (con simulazione caricamento)
    threadPool.emplace_back([&]() {
        try {
            std::this_thread::sleep_for(std::chrono::seconds(2)); // simulazione "operazione lunga"
            salvaPreventivoSuTxt(preventivo, baseName + ".txt");
        } catch (const std::exception& e) {
            salvataggioOk = false;
            std::lock_guard<std::mutex> lock(gConsoleMutex);
            std::cerr << "\nErrore salvataggio TXT: " << e.what() << "\n";
        }
        ++taskFiniti;
    });

    // Task 2: CSV
    // Task 2: CSV
    threadPool.emplace_back([&]() {
        try {
            std::this_thread::sleep_for(std::chrono::seconds(2)); // simulazione "operazione lunga"
            salvaPreventivoSuCsv(preventivo, baseName + ".csv");
        } catch (const std::exception& e) {
            salvataggioOk = false;
            std::lock_guard<std::mutex> lock(gConsoleMutex);
            std::cerr << "\nErrore salvataggio CSV: " << e.what() << "\n";
        }
        ++taskFiniti;
    });

    // Puntini finché non finiscono entrambi i task
    while (taskFiniti.load() < NUM_TASK) {
        {
            std::lock_guard<std::mutex> lock(gConsoleMutex);
            std::cout << "." << std::flush;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // join dei thread
    for (std::thread& t : threadPool) {
        if (t.joinable()) t.join();
    }

    if (salvataggioOk) {
        std::cout << "\nSalvataggio CSV e TXT completato! File: " << baseName << "\n";
    } else {
        std::cout << "\nSalvataggio NON riuscito \n Controlla il percorso.\n";
    }

    std::cout << "Premi INVIO per uscire";
    std::string dummy;
    std::getline(std::cin, dummy);

    return 0;
}
