#include <iostream>
#include <memory>     // std::unique_ptr
#include <limits>     // std::numeric_limits
#include <stdexcept>  // per std::exception / std::runtime_error
#include "Preventivo.h"
#include "VoceTinteggiatura.h"
#include "ListinoPrezzi.h"
#include "GradoDifficolta.h"

// Struttura di appoggio per memorizzare nome ciclo + prezzo base
struct CicloInfo {
    const char* nome;
    double prezzoMq;
};

// Elenco cicli che voglio avere nel listino
static const CicloInfo CICLI_TINTEGGIATURA[] = {
    // Interno civile
    {"Idropittura traspirante",        5.00},
    {"Idropittura lavabile acrilica",  6.50},
    {"Smalto all'acqua interni",       8.50},
    {"Ciclo decorativo",              15.00},
    {"Carta da parati",               18.00},

    // Interno industriale
    {"Smalto industriale",             7.00},
    {"Traspirante a spruzzo",          4.00},

    // Esterno civile
    {"Intonachino colorato",          18.00},
    {"Rasatura armata + Intonachino", 25.00},
    {"Rasatura panno + Lavabile",     22.00},
    {"Cappotto termico",              50.00},
    {"Lavabile autopulente esterno",  10.00},

    // Esterno industriale
    {"Lavabile esterno industriale",   9.00},

    // Cartongesso (per quando aggiungerò le voci specifiche)
    {"Controparete isolata",          35.00},
    {"Controparete non isolata",      28.00},
    {"Parete divisoria isolata",      42.00},
    {"Parete divisoria non isolata",  38.00},
    {"Parete Idro (bagni)",           45.00},
    {"Parete REI (antincendio)",      55.00},
    {"Controsoffitto standard",       32.00},
    {"Controsoffitto fonoassorbente", 48.00},
    {"Controsoffitto fibra minerale", 30.00},
    {"Rasatura gesso su muratura",    12.00}
};

static const std::size_t NUM_CICLI = sizeof(CICLI_TINTEGGIATURA) / sizeof(CicloInfo);

// Funzione che inizializza il listino con i cicli definiti sopra

void popolaListino(ListinoPrezzi& listino) {
    // Inserisco tutti i cicli
    for (std::size_t i = 0; i < NUM_CICLI; ++i) {
        listino.impostaPrezzoMq(CICLI_TINTEGGIATURA[i].nome,
                                CICLI_TINTEGGIATURA[i].prezzoMq);
    }

    // Coefficienti di difficoltà (stessi per tutti i cicli)
    listino.impostaCoeff(GradoDifficolta::Nuovo,      1.00); // cantiere "facile"
    listino.impostaCoeff(GradoDifficolta::Disabitato, 1.05); // +5%
    listino.impostaCoeff(GradoDifficolta::Abitato,    1.15); // +15%
}

// Funzione di utilità per "pulire" il buffer di input dopo errori
void pulisciInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Chiedo all'utente il grado di difficoltà e lo converto in enum
GradoDifficolta chiediGradoDifficolta() {
    int scelta = 0;
    while (true) {
        std::cout << "Seleziona il grado di difficolta' del cantiere:\n";
        std::cout << " 1) Nuovo\n";
        std::cout << " 2) Disabitato\n";
        std::cout << " 3) Abitato\n";
        std::cout << "Scelta: ";
        std::cin >> scelta;

        if (!std::cin) {
            pulisciInput();
            std::cout << "Input non valido, riprova.\n\n";
            continue;
        }

        if (scelta == 1) return GradoDifficolta::Nuovo;
        if (scelta == 2) return GradoDifficolta::Disabitato;
        if (scelta == 3) return GradoDifficolta::Abitato;

        std::cout << "Scelta fuori range, riprova.\n\n";
    }
}

// Mostra il menu dei cicli disponibili e ritorna l'indice scelto (oppure -1 per finire)
int menuCicli() {
    std::cout << "\n--- Seleziona un ciclo di lavorazione ---\n";
    for (std::size_t i = 0; i < NUM_CICLI; ++i) {
        std::cout << " " << (i + 1) << ") " << CICLI_TINTEGGIATURA[i].nome
                  << " (" << CICLI_TINTEGGIATURA[i].prezzoMq << " €/mq)"
                  << "\n";
    }
    std::cout << " 0) Fine inserimento voci\n";

    int scelta = -1;
    while (true) {
        std::cout << "Scelta: ";
        std::cin >> scelta;

        if (!std::cin) {
            pulisciInput();
            std::cout << "Input non valido, riprova.\n";
            continue;
        }

        if (scelta == 0) {
            return -1; // codice per "termina inserimento"
        }

        if (scelta >= 1 && scelta <= static_cast<int>(NUM_CICLI)) {
            return scelta - 1; // converto in indice [0..NUM_CICLI-1]
        }

        std::cout << "Scelta fuori range, riprova.\n";
    }
}

int main() {
    std::cout << "=== PREVENTIVATORE EDILCOLOR ===\n\n";

    // 1) Creo il listino e lo popolo con tutti i cicli
    ListinoPrezzi listino;
    try {
        popolaListino(listino);
        std::cout << "Listino caricato con " << NUM_CICLI << " cicli disponibili.\n\n";
    } catch (const std::exception& e) {
        std::cerr << "Errore nel caricamento del listino: " << e.what() << std::endl;
        return 1;
    }

    // 2) Chiedo i dati del cliente e del preventivo
    std::string id;
    std::string cliente;

    std::cout << "Inserisci ID preventivo (es. P2025-001): ";
    std::getline(std::cin, id);
    if (id.empty()) {
        // Nel caso in cui ci fosse rimasto un '\n' nel buffer
        std::getline(std::cin, id);
    }

    std::cout << "Inserisci nome cliente: ";
    std::getline(std::cin, cliente);

    GradoDifficolta grado = chiediGradoDifficolta();
    std::cout << "\nCreo il preventivo per " << cliente << "...\n\n";

    Preventivo preventivo(id, cliente, grado);

    // 3) Ciclo di inserimento voci
    while (true) {
        int indiceCiclo = menuCicli();
        if (indiceCiclo < 0) {
            // L'utente ha scelto 0 -> termina inserimento
            break;
        }

        const char* nomeCiclo = CICLI_TINTEGGIATURA[indiceCiclo].nome;

        double mq = 0.0;
        while (true) {
            std::cout << "Inserisci i metri quadri per \"" << nomeCiclo << "\": ";
            std::cin >> mq;

            if (!std::cin) {
                pulisciInput();
                std::cout << "Input non valido, riprova.\n";
                continue;
            }

            if (mq <= 0.0) {
                std::cout << "I metri quadri devono essere > 0.\n";
                continue;
            }

            break;
        }

        try {
            // Creo la voce di tinteggiatura con il grado del preventivo
            std::unique_ptr<VoceCosto> voce(
                new VoceTinteggiatura(nomeCiclo,
                                      mq,
                                      listino,
                                      grado)
            );

            // Aggiungo la voce al preventivo
            preventivo.aggiungiVoce(std::move(voce));
            std::cout << " -> Aggiunta voce: " << nomeCiclo
                      << " (" << mq << " mq)\n";

        } catch (const std::exception& e) {
            std::cerr << "Errore durante la creazione della voce: "
                      << e.what() << std::endl;
        }
    }

    // 4) Stampo il riepilogo finale del preventivo
    std::cout << "\n=== RIEPILOGO PREVENTIVO ===\n";
    std::cout << preventivo.riepilogo() << std::endl;

    return 0;
}
