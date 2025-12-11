#include <iostream>
#include <memory>
#include <limits>
#include <stdexcept>

#include "Preventivo.h"
#include "VoceTinteggiatura.h"
#include "VoceCartongesso.h"
#include "ListinoPrezzi.h"
#include "GradoDifficolta.h"
#include "RegolaTinteggiatura.h"
#include "RegolaCartongesso.h"
#include "CalcolatorePreventivo.h"

// Macro-categoria del lavoro
enum class CategoriaLavoro {
    Interno,
    Esterno,
    Cartongesso
};

// Sottocategoria allineata al PDF
enum class SottoCategoriaLavoro {
    InternoCivile,
    InternoIndustriale,
    EsternoCivile,
    EsternoIndustriale,
    Cart_Contropareti,
    Cart_ParetiDivisorie,
    Cart_Controsoffitti,
    Cart_FinituraMuratura
};

// Dati di un ciclo di lavorazione
struct CicloInfo {
    const char* nome;
    double prezzoMq;
    CategoriaLavoro categoria;
    SottoCategoriaLavoro sottocategoria;
};

// Elenco completo dei cicli (allineato al PDF)
static const CicloInfo CICLI_TINTEGGIATURA[] = {
    // === INTERNO ===
    // Interno civile
    {"Idropittura traspirante",        5.00,
     CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},
    {"Idropittura lavabile acrilica",  6.50,
     CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},
    {"Smalto all'acqua interni",       8.50,
     CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},
    {"Ciclo decorativo",              15.00,
     CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},
    {"Carta da parati",               18.00,
     CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},

    // Interno industriale
    {"Smalto industriale",             7.00,
     CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoIndustriale},
    {"Traspirante a spruzzo",          4.00,
     CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoIndustriale},

    // === ESTERNO ===
    // Esterno civile
    {"Intonachino colorato",          18.00,
     CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},
    {"Rasatura armata + Intonachino", 25.00,
     CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},
    {"Rasatura panno + Lavabile",     22.00,
     CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},
    {"Cappotto termico",              50.00,
     CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},
    {"Lavabile autopulente esterno",  10.00,
     CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},

    // Esterno industriale
    {"Lavabile esterno industriale",   9.00,
     CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoIndustriale},

    // === CARTONGESSO ===
    // Contropareti
    {"Controparete isolata",          35.00,
     CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Contropareti},
    {"Controparete non isolata",      28.00,
     CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Contropareti},

    // Pareti divisorie
    {"Parete divisoria isolata",      42.00,
     CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_ParetiDivisorie},
    {"Parete divisoria non isolata",  38.00,
     CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_ParetiDivisorie},
    {"Parete Idro (bagni)",           45.00,
     CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_ParetiDivisorie},
    {"Parete REI (antincendio)",      55.00,
     CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_ParetiDivisorie},

    // Controsoffitti
    {"Controsoffitto standard",       32.00,
     CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Controsoffitti},
    {"Controsoffitto fonoassorbente", 48.00,
     CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Controsoffitti},
    {"Controsoffitto fibra minerale", 30.00,
     CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Controsoffitti},

    // Finitura su muratura
    {"Rasatura gesso su muratura",    12.00,
     CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_FinituraMuratura}
};

static const std::size_t NUM_CICLI =
    sizeof(CICLI_TINTEGGIATURA) / sizeof(CicloInfo);

bool isCartongessoIndex(std::size_t index) {
    return CICLI_TINTEGGIATURA[index].categoria == CategoriaLavoro::Cartongesso;
}

void popolaListino(ListinoPrezzi& listino) {
    for (std::size_t i = 0; i < NUM_CICLI; ++i) {
        listino.impostaPrezzoMq(CICLI_TINTEGGIATURA[i].nome,
                                CICLI_TINTEGGIATURA[i].prezzoMq);
    }
    listino.impostaCoeff(GradoDifficolta::Nuovo,      1.00);
    listino.impostaCoeff(GradoDifficolta::Disabitato, 1.05);
    listino.impostaCoeff(GradoDifficolta::Abitato,    1.15);
}

void pulisciInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

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

CategoriaLavoro chiediCategoriaLavoro() {
    while (true) {
        std::cout << "\nSeleziona il tipo di lavoro:\n";
        std::cout << " 1) Interno\n";
        std::cout << " 2) Esterno\n";
        std::cout << " 3) Cartongesso\n";
        std::cout << "Scelta: ";

        int scelta = 0;
        std::cin >> scelta;

        if (!std::cin) {
            pulisciInput();
            std::cout << "Input non valido, riprova.\n";
            continue;
        }

        switch (scelta) {
            case 1: return CategoriaLavoro::Interno;
            case 2: return CategoriaLavoro::Esterno;
            case 3: return CategoriaLavoro::Cartongesso;
            default:
                std::cout << "Scelta fuori range, riprova.\n";
        }
    }
}

// Nuova funzione: chiede la sottocategoria in base alla macro-categoria
SottoCategoriaLavoro chiediSottoCategoriaLavoro(CategoriaLavoro categoria)
{
    while (true) {
        std::cout << "\nSeleziona la sottocategoria:\n";

        if (categoria == CategoriaLavoro::Interno) {
            std::cout << " 1) Interno civile\n";
            std::cout << " 2) Interno industriale\n";
        } else if (categoria == CategoriaLavoro::Esterno) {
            std::cout << " 1) Esterno civile\n";
            std::cout << " 2) Esterno industriale\n";
        } else { // Cartongesso
            std::cout << " 1) Contropareti\n";
            std::cout << " 2) Pareti divisorie\n";
            std::cout << " 3) Controsoffitti\n";
            std::cout << " 4) Finitura su muratura\n";
        }

        std::cout << "Scelta: ";
        int scelta = 0;
        std::cin >> scelta;

        if (!std::cin) {
            pulisciInput();
            std::cout << "Input non valido, riprova.\n";
            continue;
        }

        if (categoria == CategoriaLavoro::Interno) {
            if (scelta == 1) return SottoCategoriaLavoro::InternoCivile;
            if (scelta == 2) return SottoCategoriaLavoro::InternoIndustriale;
        } else if (categoria == CategoriaLavoro::Esterno) {
            if (scelta == 1) return SottoCategoriaLavoro::EsternoCivile;
            if (scelta == 2) return SottoCategoriaLavoro::EsternoIndustriale;
        } else { // Cartongesso
            if (scelta == 1) return SottoCategoriaLavoro::Cart_Contropareti;
            if (scelta == 2) return SottoCategoriaLavoro::Cart_ParetiDivisorie;
            if (scelta == 3) return SottoCategoriaLavoro::Cart_Controsoffitti;
            if (scelta == 4) return SottoCategoriaLavoro::Cart_FinituraMuratura;
        }

        std::cout << "Scelta non valida, riprova.\n";
    }
}

// Ritorna l'indice globale del ciclo scelto, oppure -1 se l'utente torna indietro
int menuCicliPerCategoria(CategoriaLavoro categoria,
                          SottoCategoriaLavoro sottocategoria) {
    std::cout << "\n--- Seleziona un ciclo di lavorazione ---\n";

    int voceMenu = 1;
    int mappaSceltaVersoIndice[NUM_CICLI];

    for (std::size_t i = 0; i < NUM_CICLI; ++i) {
        const CicloInfo& ciclo = CICLI_TINTEGGIATURA[i];

        if (ciclo.categoria == categoria &&
            ciclo.sottocategoria == sottocategoria) {
            std::cout << " " << voceMenu << ") " << ciclo.nome
                      << " (" << ciclo.prezzoMq << " €/mq)\n";
            mappaSceltaVersoIndice[voceMenu] = static_cast<int>(i);
            ++voceMenu;
        }
    }

    if (voceMenu == 1) {
        std::cout << "Nessun ciclo disponibile per questa categoria.\n";
        return -1;
    }

    std::cout << " 0) Torna indietro\n";

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
            return -1;
        }

        if (scelta >= 1 && scelta < voceMenu) {
            return mappaSceltaVersoIndice[scelta];
        }

        std::cout << "Scelta fuori range, riprova.\n";
    }
}

// Chiede se continuare dopo aver inserito una voce
bool chiedeContinuaInserimento() {
    pulisciInput(); // pulisco buffer prima di leggere il carattere

    while (true) {
        std::cout << "\nVuoi aggiungere un'altra voce? (S/N): ";
        char risposta;
        std::cin >> risposta;

        if (!std::cin) {
            pulisciInput();
            std::cout << "Input non valido, riprova.\n";
            continue;
        }

        if (risposta == 'S' || risposta == 's') {
            return true;
        }
        if (risposta == 'N' || risposta == 'n') {
            return false;
        }

        std::cout << "Risposta non valida. Inserisci S o N.\n";
    }
}

int main() {
    std::cout << "=== PREVENTIVATORE EDILCOLOR ===\n\n";

    // 1) Carico il listino
    ListinoPrezzi listino;
    try {
        popolaListino(listino);
        std::cout << "Listino caricato con " << NUM_CICLI
                  << " cicli disponibili.\n\n";
    } catch (const std::exception& e) {
        std::cerr << "Errore nel caricamento del listino: "
                  << e.what() << std::endl;
        return 1;
    }

    // 2) Dati cliente e preventivo
    std::string id;
    std::string cliente;

    std::cout << "Inserisci ID preventivo (es. P2025-001): ";
    std::getline(std::cin, id);

    if (id.empty()) {
        std::getline(std::cin, id);
    }

    std::cout << "Inserisci nome cliente: ";
    std::getline(std::cin, cliente);

    GradoDifficolta grado = chiediGradoDifficolta();
    std::cout << "\nCreo il preventivo per " << cliente << "...\n";

    Preventivo preventivo(id, cliente, grado);

    // Strategy e calcolatore
    RegolaTinteggiatura regolaTinteggiatura;
    RegolaCartongesso  regolaCartongesso;
    CalcolatorePreventivo calcolatore;

    // 3) CICLO INSERIMENTO VOCI
    bool continuaInserimento = true;

    while (continuaInserimento) {
        // Scelgo la macro-categoria
        CategoriaLavoro categoria = chiediCategoriaLavoro();

        // Scelgo la sottocategoria in base al tipo scelto
        SottoCategoriaLavoro sottocat = chiediSottoCategoriaLavoro(categoria);

        // Mostro i cicli della categoria + sottocategoria
        int indiceCiclo = menuCicliPerCategoria(categoria, sottocat);

        if (indiceCiclo < 0) {
            // L'utente ha premuto "0" (torna indietro)
            continuaInserimento = chiedeContinuaInserimento();
            continue;
        }

        const char* nomeCiclo = CICLI_TINTEGGIATURA[indiceCiclo].nome;

        // Chiedo i metri quadri
        double mq = 0.0;
        while (true) {
            std::cout << "Inserisci i metri quadri per \""
                      << nomeCiclo << "\": ";
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

        // Creo e aggiungo la voce
        try {
            std::string nomeCicloStr = nomeCiclo;

            if (isCartongessoIndex(static_cast<std::size_t>(indiceCiclo))) {
                calcolatore.setRegola(&regolaCartongesso);
            } else {
                calcolatore.setRegola(&regolaTinteggiatura);
            }

            calcolatore.aggiungiLavoro(preventivo,
                                       nomeCicloStr,
                                       mq,
                                       listino,
                                       grado);

            std::cout << " -> Aggiunta voce: " << nomeCiclo
                      << " (" << mq << " mq)\n";

        } catch (const std::exception& e) {
            std::cerr << "Errore durante la creazione della voce: "
                      << e.what() << std::endl;
        }

        // Dopo aver aggiunto la voce, chiedo subito se continuare
        continuaInserimento = chiedeContinuaInserimento();
    }

    // 4) Riepilogo finale
    std::cout << "\n=== RIEPILOGO PREVENTIVO ===\n";
    std::cout << preventivo.riepilogo() << std::endl;

    return 0;
}
