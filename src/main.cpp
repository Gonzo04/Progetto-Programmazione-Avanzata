#include <iostream>
#include <memory>       // per std::shared_ptr, std::make_shared
#include <limits>
#include <stdexcept>
#include <cctype>
#include <thread>       // per std::thread
#include <mutex>
#include <vector>
#include <functional>
#include <fstream>
#include <ctime>
#include <atomic>
#include <iomanip>
#include <chrono>
#include <set>
#include <list>

#include "Preventivo.h"
#include "VoceTinteggiatura.h"
#include "VoceCartongesso.h"
#include "ListinoPrezzi.h"
#include "GradoDifficolta.h"
#include "RegolaTinteggiatura.h"
#include "RegolaCartongesso.h"
#include "CalcolatorePreventivo.h"
#include "Utils.h"      // <--- INCLUDE LE TUE NUOVE FUNZIONI DI UTILITÀ

static const double MQ_MAX_REALISTICI = 1000000.0;

// Mutex per proteggere l'output su console (usato dal thread di salvataggio)
std::mutex gConsoleMutex;

// Macro-categoria
enum class CategoriaLavoro {
    Interno,
    Esterno,
    Cartongesso
};

// Sottocategorie
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

// Elenco completo dei cicli
static const CicloInfo CICLI_TINTEGGIATURA[] = {
    // === INTERNO ===
    {"Idropittura traspirante",        5.00, CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},
    {"Idropittura lavabile acrilica",  6.50, CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},
    {"Smalto all'acqua interni",       8.50, CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},
    {"Ciclo decorativo",              15.00, CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},
    {"Carta da parati",               18.00, CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoCivile},
    {"Smalto industriale",             7.00, CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoIndustriale},
    {"Traspirante a spruzzo",          4.00, CategoriaLavoro::Interno, SottoCategoriaLavoro::InternoIndustriale},

    // === ESTERNO ===
    {"Intonachino colorato",          18.00, CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},
    {"Rasatura armata + Intonachino", 25.00, CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},
    {"Rasatura panno + Lavabile",     22.00, CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},
    {"Cappotto termico",              50.00, CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},
    {"Lavabile autopulente esterno",  10.00, CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoCivile},
    {"Lavabile esterno industriale",   9.00, CategoriaLavoro::Esterno, SottoCategoriaLavoro::EsternoIndustriale},

    // === CARTONGESSO ===
    {"Controparete isolata",          35.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Contropareti},
    {"Controparete non isolata",      28.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Contropareti},
    {"Parete divisoria isolata",      42.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_ParetiDivisorie},
    {"Parete divisoria non isolata",  38.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_ParetiDivisorie},
    {"Parete Idro (bagni)",           45.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_ParetiDivisorie},
    {"Parete REI (antincendio)",      55.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_ParetiDivisorie},
    {"Controsoffitto standard",       32.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Controsoffitti},
    {"Controsoffitto fonoassorbente", 48.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Controsoffitti},
    {"Controsoffitto fibra minerale", 30.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_Controsoffitti},
    {"Rasatura gesso su muratura",    12.00, CategoriaLavoro::Cartongesso, SottoCategoriaLavoro::Cart_FinituraMuratura}
};

static const std::size_t NUM_CICLI = sizeof(CICLI_TINTEGGIATURA) / sizeof(CicloInfo);

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

// -- FUNZIONI DI INPUT REFACTORIZZATE (Usano utils.h) --

GradoDifficolta chiediGradoDifficolta() {
    while (true) {
        std::cout << "Seleziona la tipologia del cantiere:\n";
        std::cout << " 1) Nuovo\n";
        std::cout << " 2) Disabitato\n";
        std::cout << " 3) Abitato\n";

        // Uso la tua nuova funzione robusta
        int scelta = leggiIntero("Scelta: ");

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

        int scelta = leggiIntero("Scelta: ");

        switch (scelta) {
            case 1: return CategoriaLavoro::Interno;
            case 2: return CategoriaLavoro::Esterno;
            case 3: return CategoriaLavoro::Cartongesso;
            default:
                std::cout << "Scelta fuori range, riprova.\n";
        }
    }
}

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

        int scelta = leggiIntero("Scelta: ");

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
    // Mappa locale per convertire "scelta menu" (1,2,3) -> "indice array globale"
    int mappaSceltaVersoIndice[NUM_CICLI];

    for (std::size_t i = 0; i < NUM_CICLI; ++i) {
        const CicloInfo& ciclo = CICLI_TINTEGGIATURA[i];

        if (ciclo.categoria == categoria &&
            ciclo.sottocategoria == sottocategoria) {
            std::cout << " " << voceMenu << ") " << ciclo.nome
                      << " (" << ciclo.prezzoMq << " euro/mq)\n";
            mappaSceltaVersoIndice[voceMenu] = static_cast<int>(i);
            ++voceMenu;
        }
    }

    if (voceMenu == 1) {
        std::cout << "Nessun ciclo disponibile per questa categoria.\n";
        return -1;
    }

    std::cout << " 0) Torna indietro\n";

    while (true) {
        int scelta = leggiIntero("Scelta: ");

        if (scelta == 0) return -1;

        if (scelta >= 1 && scelta < voceMenu) {
            return mappaSceltaVersoIndice[scelta];
        }
        std::cout << "Scelta fuori range, riprova.\n";
    }
}

// Funzione helper per salvataggio TXT
void salvaPreventivoSuTxt(const Preventivo& p, const std::string& filename) {
    std::ofstream out(filename.c_str());
    if (!out) {
        // Uso il mutex perché questa funzione potrebbe essere chiamata dal thread secondario
        std::lock_guard<std::mutex> lock(gConsoleMutex);
        std::cerr << "Impossibile aprire il file di testo: " << filename << "\n";
        return;
    }
    out << p.riepilogo();
}

// Funzione helper per salvataggio CSV
void salvaPreventivoSuCsv(const Preventivo& p, const std::string& filename) {
    std::ofstream out(filename.c_str());
    if (!out) {
        std::lock_guard<std::mutex> lock(gConsoleMutex);
        std::cerr << "Impossibile aprire il file CSV: " << filename << "\n";
        return;
    }

    out << std::fixed << std::setprecision(2);
    out << "TipoVoce;Nome;Unita;Quantita;PrezzoUnitario;Coefficiente;Subtotale\n";

    const std::vector<std::unique_ptr<VoceCosto>>& voci = p.getVoci();
    std::set<std::string> nomiCicliUsati;

    for (const auto& vocePtr : voci) {
        if (!vocePtr) continue;

        const VoceCartongesso* vc = dynamic_cast<const VoceCartongesso*>(vocePtr.get());
        std::string tipo = (vc ? "Cartongesso" : "Tinteggiatura");

        nomiCicliUsati.insert(vocePtr->getNome());

        out << tipo                     << ";"
            << vocePtr->getNome()        << ";"
            << vocePtr->getUnitaMisura() << ";"
            << vocePtr->getQuantita()    << ";"
            << vocePtr->getPrezzoUnitario() << ";"
            << vocePtr->getCoefficiente()   << ";"
            << vocePtr->subtotale()
            << "\n";
    }

    double imponibile = p.totale();
    double iva = imponibile * 0.22;
    double totaleIvato = imponibile + iva;

    out << "TotaleImponibile;;;;;"  << imponibile  << "\n";
    out << "IVA(22%);;;;;"          << iva         << "\n";
    out << "TotaleComplessivo;;;;;" << totaleIvato << "\n";
    out << "NumeroTipiCiclo;;;;;" << nomiCicliUsati.size() << "\n";
}

std::string generaIdPreventivoAutomatico() {
    std::time_t now = std::time(nullptr);
    std::tm* ptm = std::localtime(&now);
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "P%Y%m%d-%H%M%S", ptm);
    return std::string(buffer);
}

int main() {
    std::cout << "=== PREVENTIVATORE EDILCOLOR ===\n\n";

    // 1) CREAZIONE LISTINO CON SHARED POINTER
    // Soddisfa il requisito "smart pointer" per la risorsa condivisa
    auto listino = std::make_shared<ListinoPrezzi>();

    try {
        popolaListino(*listino); // passo il valore puntato
        std::cout << "Listino caricato con " << NUM_CICLI
                  << " cicli disponibili.\n\n";
    } catch (const std::exception& e) {
        std::cerr << "Errore nel caricamento del listino: "
                  << e.what() << std::endl;
        return 1;
    }

    // 2) INPUT DATI CLIENTE
    std::string id = generaIdPreventivoAutomatico();
    std::cout << "ID preventivo assegnato automaticamente: " << id << "\n\n";

    std::string cliente;
    while (true) {
        std::cout << "Inserisci il tuo nome e cognome: ";
        std::string linea;
        if (!std::getline(std::cin, linea)) {
            // Se c'è un errore grave nello stream (es. CTRL+Z / CTRL+D), esco
            return 1;
        }

        // Nota: trim() l'hai spostata in utils.cpp, quindi la chiamiamo da lì
        cliente = trim(linea);
        if (cliente.empty()) {
            std::cout << "Il nome non puo' essere vuoto. Riprova.\n\n";
            continue;
        }
        break;
    }

    GradoDifficolta grado = chiediGradoDifficolta();

    Preventivo preventivo(id, cliente, grado);

    // Strategy e calcolatore
    RegolaTinteggiatura regolaTinteggiatura;
    RegolaCartongesso  regolaCartongesso;
    CalcolatorePreventivo calcolatore;

    // Variabili stato loop
    CategoriaLavoro categoriaCorrente;
    SottoCategoriaLavoro sottocatCorrente;
    bool haCategoriaCorrente = false;
    bool continuaInserimento = true;

    // 3) CICLO INSERIMENTO VOCI
    while (continuaInserimento) {
        if (!haCategoriaCorrente) {
            categoriaCorrente = chiediCategoriaLavoro();
            sottocatCorrente  = chiediSottoCategoriaLavoro(categoriaCorrente);
            haCategoriaCorrente = true;
        }

        int indiceCiclo = menuCicliPerCategoria(categoriaCorrente, sottocatCorrente);

        if (indiceCiclo < 0) {
            haCategoriaCorrente = false;
            continue;
        }

        const char* nomeCiclo = CICLI_TINTEGGIATURA[indiceCiclo].nome;

        // Loop mq robusto
        double mq = 0.0;
        while (true) {
            // Uso la funzione helper per i double
            mq = leggiDouble("Inserisci i metri quadri per \"" + std::string(nomeCiclo) + "\": ");

            if (mq <= 0.0) {
                std::cout << "I metri quadri devono essere > 0.\n";
                continue;
            }
            if (mq > MQ_MAX_REALISTICI) {
                std::cout << "Valore troppo grande. Inserisci un valore realistico (<= "
                          << MQ_MAX_REALISTICI << ").\n";
                continue;
            }
            break;
        }

        try {
            std::string nomeCicloStr = nomeCiclo;

            if (isCartongessoIndex(static_cast<std::size_t>(indiceCiclo))) {
                calcolatore.setRegola(&regolaCartongesso);
            } else {
                calcolatore.setRegola(&regolaTinteggiatura);
            }

            // Nota: Qui passiamo *listino (reference).
            // Se aggiorni CalcolatorePreventivo per prendere shared_ptr, passa direttamente 'listino'.
            calcolatore.aggiungiLavoro(preventivo,
                                       nomeCicloStr,
                                       mq,
                                       *listino,
                                       grado);

            std::cout << " -> Aggiunta voce: " << nomeCiclo
                      << " (" << mq << " mq)\n";

        } catch (const std::exception& e) {
            std::cerr << "Errore durante la creazione della voce: "
                      << e.what() << std::endl;
        }

        continuaInserimento = chiediConferma("\nVuoi aggiungere un'altra voce?");
    }

    preventivo.ordinaPerNome();

    // RIEPILOGO FINALE
    std::cout << "\n=== RIEPILOGO PREVENTIVO ===\n";
    std::cout << preventivo.riepilogo() << std::endl;

    // 4) THREADING: SALVATAGGIO IN BACKGROUND
    // Questo approccio è "reale": sposto l'operazione lenta (I/O su disco) su un altro thread
    // per non bloccare l'eventuale interfaccia (simulata qui dal main thread che attende)

    std::cout << "\n[INFO] Avvio salvataggio file in background (thread parallelo)...\n";

    std::thread t_salvataggio([&preventivo]() {
        // Simuliamo un leggero ritardo per rendere evidente il background work
        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::string baseName = preventivo.getId();
        if (baseName.empty()) baseName = "preventivo";

        salvaPreventivoSuTxt(preventivo, baseName + ".txt");
        salvaPreventivoSuCsv(preventivo, baseName + ".csv");

        // Uso il mutex per stampare a video senza sovrappormi al main
        std::lock_guard<std::mutex> lock(gConsoleMutex);
        std::cout << "\n[BACKGROUND] Salvataggio completato! File generati: "
                  << baseName << ".txt / .csv\n"
                  << "Premi INVIO per uscire definitivamente.\n";
    });

    std::cout << "Il programma principale ha terminato l'inserimento.\n";
    std::cout << "Attendo completamento salvataggio...\n";

    // Attendo che il thread finisca prima di chiudere il programma
    if (t_salvataggio.joinable()) {
        t_salvataggio.join();
    }

    // Pausa finale per permettere di leggere
    std::string dummy;
    std::getline(std::cin, dummy);

    return 0;
}