#include <iostream>
#include <memory>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <thread>      // per std::thread
#include <mutex>
#include <vector>      // per std::vector di thread
#include <functional>  // per std::cref
#include <fstream>    // per std::ofstream
#include <ctime>   // per l'ID utente
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

static const double MQ_MAX_REALISTICI = 100000.0; // 100.000 m^2 limite di sicurezza


void stampaRiepilogoAsync(const Preventivo& p,
                          int indice,
                          std::atomic<int>& contatore);

// Mutex per proteggere l'output su console tra i vari thread
std::mutex gConsoleMutex;
double sommaTotaliThread = 0.0;  // variabile condivisa tra i thread



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
        std::cout << "Seleziona la tipologia del cantiere:\n";
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
    // Pulisco eventuali residui (es. '\n') lasciati da operator>>
    pulisciInput();

    std::string linea;

    while (true) {
        std::cout << "\nVuoi aggiungere un'altra voce? (S/N): ";

        // Leggo l'intera riga così com'è
        if (!std::getline(std::cin, linea)) {
            pulisciInput();
            continue;
        }

        // Cerco il primo carattere non bianco
        char risposta = '\0';
        for (char c : linea) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                risposta = c;
                break;
            }
        }

        // Se l'utente ha solo premuto Invio (o solo spazi), non stampo errori:
        // semplicemente ripeto la stessa domanda
        if (risposta == '\0') {
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

// Funzione eseguita dal thread: simula un'elaborazione sul preventivo
void stampaRiepilogoAsync(const Preventivo& p,
                          int indice,
                          std::atomic<int>& contatore) {
    std::this_thread::sleep_for(std::chrono::milliseconds(300 + indice * 150));

    double totale = p.totale();

    // Sezione critica: aggiorno una variabile condivisa
    {
        std::lock_guard<std::mutex> lock(gConsoleMutex);
        sommaTotaliThread += totale;
    }

    ++contatore;
}





void salvaPreventivoSuTxt(const Preventivo& p, const std::string& filename) {
    std::ofstream out(filename.c_str());
    if (!out) {
        std::cerr << "Impossibile aprire il file di testo: " << filename << "\n";
        return;
    }

    // Uso direttamente il riepilogo già formattato
    out << p.riepilogo();

    std::cout << "Preventivo salvato in formato TXT su: " << filename << "\n";
}


//CSV
//CSV
void salvaPreventivoSuCsv(const Preventivo& p, const std::string& filename) {
    std::ofstream out(filename.c_str());
    if (!out) {
        std::cerr << "Impossibile aprire il file CSV: " << filename << "\n";
        return;
    }

    // formattazione numerica a 2 decimali
    out << std::fixed << std::setprecision(2);

    // Intestazione colonne CSV
    out << "TipoVoce;Nome;Unita;Quantita;PrezzoUnitario;Coefficiente;Subtotale\n";

    const std::vector<std::unique_ptr<VoceCosto>>& voci = p.getVoci();

    // Uso std::set per avere l'elenco dei cicli senza duplicati (nomi unici ordinati)
    std::set<std::string> nomiCicliUsati;

    for (std::size_t i = 0; i < voci.size(); ++i) {
        const std::unique_ptr<VoceCosto>& vocePtr = voci[i];
        if (!vocePtr) continue;

        // dynamic_cast: controllo il tipo reale della voce (polimorfismo)
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

    // Righe finali con riepilogo economico
    double imponibile = p.totale();
    const double ALIQUOTA_IVA = 0.22;
    double iva = imponibile * ALIQUOTA_IVA;
    double totaleIvato = imponibile + iva;

    out << "TotaleImponibile;;;;;"  << imponibile  << "\n";
    out << "IVA(22%);;;;;"          << iva         << "\n";
    out << "TotaleComplessivo;;;;;" << totaleIvato << "\n";

    // Uso reale di std::set: numero di tipologie di ciclo diverse usate
    out << "NumeroTipiCiclo;;;;;" << nomiCicliUsati.size() << "\n";

    std::cout << "Preventivo salvato in formato CSV su: " << filename << "\n";
}



std::string generaIdPreventivoAutomatico() {
    std::time_t now = std::time(nullptr);
    std::tm* ptm = std::localtime(&now);

    char buffer[32];
    // Formato: PYYYYMMDD-HHMMSS (es. P20251214-153027)
    std::strftime(buffer, sizeof(buffer), "P%Y%m%d-%H%M%S", ptm);

    return std::string(buffer);
}

std::string trim(const std::string& s) {
    std::size_t first = s.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    std::size_t last = s.find_last_not_of(" \t");
    return s.substr(first, last - first + 1);
}

int main() {
    std::cout << "=== PREVENTIVATORE EDILCOLOR ===\n\n";

    // Uso std::list per lo storico ID dei preventivi creati nella sessione.
    // Qui bastava anche un vector, ma list mi permette di mostrare un contenitore diverso



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

    // Genero automaticamente l'ID in base alla data e ora
    // Genero automaticamente l'ID in base alla data e ora
    id = generaIdPreventivoAutomatico();
    std::cout << "ID preventivo assegnato automaticamente: " << id << "\n\n";


    std::string lineaCliente;
    while (true) {
        std::cout << "Inserisci il tuo nome e cognome: ";
        if (!std::getline(std::cin, lineaCliente)) {
            // errore di input: provo a ripulire e ripetere
            pulisciInput();
            continue;
        }

        cliente = trim(lineaCliente);
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
    CategoriaLavoro categoriaCorrente;
    SottoCategoriaLavoro sottocatCorrente;
    bool haCategoriaCorrente = false;

    // 3) CICLO INSERIMENTO VOCI
    bool continuaInserimento = true;


    while (continuaInserimento) {
        // Se non ho ancora scelto una categoria/sottocategoria, la chiedo
        if (!haCategoriaCorrente) {
            categoriaCorrente = chiediCategoriaLavoro();
            sottocatCorrente  = chiediSottoCategoriaLavoro(categoriaCorrente);
            haCategoriaCorrente = true;
        }

        // Mostro i cicli della categoria + sottocategoria correnti
        int indiceCiclo = menuCicliPerCategoria(categoriaCorrente, sottocatCorrente);

        if (indiceCiclo < 0) {
            // L'utente ha premuto "0" nel menu dei cicli:
            // → torniamo al menu principale per (Interno/Esterno/Cartongesso)
            haCategoriaCorrente = false;
            continue;   // si torna all'inizio del while
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

            if (mq > MQ_MAX_REALISTICI) {
                std::cout << "Valore troppo grande (" << mq
                          << " m^2). Inserisci un valore realistico (<= "
                          << MQ_MAX_REALISTICI << " m^2).\n";
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

    preventivo.ordinaPerNome();

    std::atomic<int> contatorePreventivi(0);

    // Riepilogo finale
    std::cout << "\n=== RIEPILOGO PREVENTIVO ===\n";
    std::cout << preventivo.riepilogo() << std::endl;

    // elaboro più preventivi in parallelo (simulazione "caricamento")
    std::vector<Preventivo> preventiviBatch;
    preventiviBatch.push_back(preventivo);
    preventiviBatch.push_back(preventivo);
    preventiviBatch.push_back(preventivo);

    std::list<std::thread> threads;

    std::cout << "\nElaborazione e salvataggio del preventivo in corso\n";

    for (std::size_t i = 0; i < preventiviBatch.size(); ++i) {
        threads.push_back(std::thread(
            stampaRiepilogoAsync,
            std::cref(preventiviBatch[i]),
            static_cast<int>(i + 1),
            std::ref(contatorePreventivi)
        ));
    }


    // Aspetto che tutti i thread abbiano finito
    for (std::thread& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }


    // Salvo automaticamente il preventivo in TXT e CSV usando l'ID come nome base
    std::string baseName = preventivo.getId();
    if (baseName.empty()) {
        baseName = "preventivo";
    }

    std::string filenameTxt = baseName + ".txt";
    std::string filenameCsv = baseName + ".csv";


    salvaPreventivoSuTxt(preventivo, filenameTxt);
    salvaPreventivoSuCsv(preventivo, filenameCsv);


    return 0;
}





