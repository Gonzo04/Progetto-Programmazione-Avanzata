#include <iostream>
#include <memory>
#include <thread>
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
#include <sstream>

#include "Preventivo.h"
#include "VoceTinteggiatura.h"
#include "VoceCartongesso.h"
#include "ListinoPrezzi.h"
#include "GradoDifficolta.h"
#include "RegolaTinteggiatura.h"
#include "RegolaCartongesso.h"
#include "CalcolatorePreventivo.h"
#include "utils.h"

static constexpr double MQ_MAX_REALISTICI = 1000000.0;

// Mutex per proteggere l'output su console
std::mutex gConsoleMutex;

// [REQUISITO ATOMIC]
// Contatore atomico per garantire univocità anche in ambiente multithread.
// Anche se qui usiamo un solo thread principale per creare, dimostra la conoscenza.
static std::atomic<int> g_contatorePreventivi(1);

// ... (Enum e Struct rimangono uguali: CategoriaLavoro, SottoCategoriaLavoro, CicloInfo) ...
// ... (Copia qui le Enum e struct CicloInfo dal codice precedente) ...
// PER BREVITÀ NON RIPETO LE ENUM E L'ARRAY CICLI_TINTEGGIATURA CHE SONO IDENTICI
// ASSICURATI DI AVERLI QUI (CategoriaLavoro, CICLI_TINTEGGIATURA, ecc.)

enum class CategoriaLavoro {
    Interno, Esterno, Cartongesso
};

enum class SottoCategoriaLavoro {
    InternoCivile, InternoIndustriale, EsternoCivile, EsternoIndustriale,
    Cart_Contropareti, Cart_ParetiDivisorie, Cart_Controsoffitti, Cart_FinituraMuratura
};

struct CicloInfo {
    const char* nome;
    double prezzoMq;
    CategoriaLavoro categoria;
    SottoCategoriaLavoro sottocategoria;
};

// ... Inserisci qui l'array CICLI_TINTEGGIATURA come prima ...
// (Lo ometto per non intasare la risposta, ma nel file finale deve esserci)
// Se vuoi te lo ricopio, ma è identico a prima.
static const CicloInfo CICLI_TINTEGGIATURA[] = {
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
static constexpr std::size_t NUM_CICLI = sizeof(CICLI_TINTEGGIATURA) / sizeof(CicloInfo);

bool isCartongessoIndex(std::size_t index) {
    return CICLI_TINTEGGIATURA[index].categoria == CategoriaLavoro::Cartongesso;
}

void popolaListino(ListinoPrezzi& listino) {
    for (std::size_t i = 0; i < NUM_CICLI; ++i) {
        listino.impostaPrezzoMq(CICLI_TINTEGGIATURA[i].nome, CICLI_TINTEGGIATURA[i].prezzoMq);
    }
    listino.impostaCoeff(GradoDifficolta::Nuovo,      1.00);
    listino.impostaCoeff(GradoDifficolta::Disabitato, 1.05);
    listino.impostaCoeff(GradoDifficolta::Abitato,    1.15);
}

// -- FUNZIONI DI INPUT (Copiate da prima) --
GradoDifficolta chiediGradoDifficolta() {
    while (true) {
        std::cout << "Seleziona la tipologia del cantiere:\n 1) Nuovo\n 2) Disabitato\n 3) Abitato\n";
        int scelta = leggiIntero("Scelta: ");
        if (scelta == 1) return GradoDifficolta::Nuovo;
        if (scelta == 2) return GradoDifficolta::Disabitato;
        if (scelta == 3) return GradoDifficolta::Abitato;
        std::cout << "Scelta fuori range.\n\n";
    }
}

CategoriaLavoro chiediCategoriaLavoro() {
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

SottoCategoriaLavoro chiediSottoCategoriaLavoro(CategoriaLavoro categoria) {
    // ... (Logica identica a prima, usa leggiIntero) ...
    // Per brevità non la ricopio tutta, è quella del messaggio precedente.
    // [IMPORTANTE] Assicurati che sia presente nel file.
     while (true) {
        std::cout << "\nSeleziona la sottocategoria:\n";
        if (categoria == CategoriaLavoro::Interno) {
            std::cout << " 1) Interno civile\n 2) Interno industriale\n";
        } else if (categoria == CategoriaLavoro::Esterno) {
            std::cout << " 1) Esterno civile\n 2) Esterno industriale\n";
        } else {
            std::cout << " 1) Contropareti\n 2) Pareti divisorie\n 3) Controsoffitti\n 4) Finitura su muratura\n";
        }
        int scelta = leggiIntero("Scelta: ");

        if (categoria == CategoriaLavoro::Interno) {
            if (scelta == 1) return SottoCategoriaLavoro::InternoCivile;
            if (scelta == 2) return SottoCategoriaLavoro::InternoIndustriale;
        } else if (categoria == CategoriaLavoro::Esterno) {
            if (scelta == 1) return SottoCategoriaLavoro::EsternoCivile;
            if (scelta == 2) return SottoCategoriaLavoro::EsternoIndustriale;
        } else {
            if (scelta == 1) return SottoCategoriaLavoro::Cart_Contropareti;
            if (scelta == 2) return SottoCategoriaLavoro::Cart_ParetiDivisorie;
            if (scelta == 3) return SottoCategoriaLavoro::Cart_Controsoffitti;
            if (scelta == 4) return SottoCategoriaLavoro::Cart_FinituraMuratura;
        }
        std::cout << "Scelta non valida.\n";
    }
}

int menuCicliPerCategoria(CategoriaLavoro categoria, SottoCategoriaLavoro sottocategoria) {
    std::cout << "\n--- Seleziona un ciclo di lavorazione ---\n";
    int voceMenu = 1;
    int mappaSceltaVersoIndice[NUM_CICLI];
    for (std::size_t i = 0; i < NUM_CICLI; ++i) {
        const CicloInfo& ciclo = CICLI_TINTEGGIATURA[i];
        if (ciclo.categoria == categoria && ciclo.sottocategoria == sottocategoria) {
            std::cout << " " << voceMenu << ") " << ciclo.nome << " (" << ciclo.prezzoMq << " euro/mq)\n";
            mappaSceltaVersoIndice[voceMenu] = static_cast<int>(i);
            ++voceMenu;
        }
    }
    if (voceMenu == 1) { std::cout << "Nessun ciclo disp.\n"; return -1; }
    std::cout << " 0) Torna indietro\n";
    while (true) {
        int scelta = leggiIntero("Scelta: ");
        if (scelta == 0) return -1;
        if (scelta >= 1 && scelta < voceMenu) return mappaSceltaVersoIndice[scelta];
        std::cout << "Scelta fuori range.\n";
    }
}

void salvaPreventivoSuTxt(const Preventivo& p, const std::string& filename) {
    std::ofstream out(filename.c_str());
    if (!out) {
        std::lock_guard<std::mutex> lock(gConsoleMutex);
        std::cerr << "Impossibile aprire file TXT: " << filename << "\n";
        return;
    }
    out << p.riepilogo();
}

void salvaPreventivoSuCsv(const Preventivo& p, const std::string& filename) {
    std::ofstream out(filename.c_str());
    if (!out) {
        std::lock_guard<std::mutex> lock(gConsoleMutex);
        std::cerr << "Impossibile aprire file CSV: " << filename << "\n";
        return;
    }
    // ... Logica salvataggio CSV identica a prima ...
    out << std::fixed << std::setprecision(2);
    out << "TipoVoce;Nome;Unita;Quantita;PrezzoUnitario;Coefficiente;Subtotale\n";
    const auto& voci = p.getVoci();
    std::set<std::string> nomiCicliUsati;
    for (const auto& vocePtr : voci) {
        if (!vocePtr) continue;
        const VoceCartongesso* vc = dynamic_cast<const VoceCartongesso*>(vocePtr.get());
        std::string tipo = (vc ? "Cartongesso" : "Tinteggiatura");
        nomiCicliUsati.insert(vocePtr->getNome());
        out << tipo << ";" << vocePtr->getNome() << ";" << vocePtr->getUnitaMisura() << ";"
            << vocePtr->getQuantita() << ";" << vocePtr->getPrezzoUnitario() << ";"
            << vocePtr->getCoefficiente() << ";" << vocePtr->subtotale() << "\n";
    }
    double imp = p.totale();
    out << "TotaleImponibile;;;;;" << imp << "\n";
    out << "IVA(22%);;;;;" << imp * 0.22 << "\n";
    out << "TotaleComplessivo;;;;;" << imp * 1.22 << "\n";
    out << "NumeroTipiCiclo;;;;;" << nomiCicliUsati.size() << "\n";
}

std::string generaIdPreventivoAutomatico() {
    std::time_t now = std::time(nullptr);
    std::tm* ptm = std::localtime(&now);
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d", ptm);

    // [UTILIZZO ATOMIC] Incremento atomico del contatore globale
    // Anche se oggi siamo single-thread nell'inserimento, questo è "Thread Safe" per design.
    int progressivo = g_contatorePreventivi.fetch_add(1);

    std::ostringstream oss;
    oss << "P" << buffer << "-" << std::setw(3) << std::setfill('0') << progressivo;
    return oss.str();
}

int main() {
    std::cout << "=== PREVENTIVATORE EDILCOLOR ===\n\n";

    auto listino = std::make_shared<ListinoPrezzi>();
    try {
        popolaListino(*listino);
        std::cout << "Listino caricato con " << NUM_CICLI << " cicli.\n\n";
    } catch (const std::exception& e) {
        std::cerr << "Errore caricamento listino: " << e.what() << std::endl;
        return 1;
    }

    std::string id = generaIdPreventivoAutomatico();
    std::cout << "ID preventivo assegnato automaticamente: " << id << "\n\n";

    std::string cliente;
    while (true) {
        std::cout << "Inserisci nome e cognome: ";
        std::string linea;
        if (!std::getline(std::cin, linea)) return 1;
        cliente = trim(linea); // trim è in utils
        if (!cliente.empty()) break;
        std::cout << "Il nome non puo' essere vuoto.\n";
    }

    GradoDifficolta grado = chiediGradoDifficolta();
    Preventivo preventivo(id, cliente, grado);

    RegolaTinteggiatura regolaTinteggiatura;
    RegolaCartongesso  regolaCartongesso;
    CalcolatorePreventivo calcolatore;

    CategoriaLavoro catCorr;
    SottoCategoriaLavoro sottoCatCorr;
    bool haCat = false;
    bool continua = true;

    while (continua) {
        if (!haCat) {
            catCorr = chiediCategoriaLavoro();
            sottoCatCorr = chiediSottoCategoriaLavoro(catCorr);
            haCat = true;
        }

        int idx = menuCicliPerCategoria(catCorr, sottoCatCorr);
        if (idx < 0) { haCat = false; continue; }

        const char* nomeCiclo = CICLI_TINTEGGIATURA[idx].nome;
        double mq = 0.0;
        while (true) {
            mq = leggiDouble("Inserisci mq per \"" + std::string(nomeCiclo) + "\": ");
            if (mq > 0 && mq <= MQ_MAX_REALISTICI) break;
            std::cout << "Valore non valido (deve essere > 0 e <= " << MQ_MAX_REALISTICI << ")\n";
        }

        try {
            if (isCartongessoIndex(idx)) calcolatore.setRegola(&regolaCartongesso);
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

    std::cout << "\nSalvataggio in corso";

    // [REQUISITO LIST]
    // Uso una lista di thread per gestire i task in background.
    // Dimostra che sai usare std::list con oggetti non copiabili (thread).
    std::atomic<bool> SalvataggioInCorso(true);

    std::list<std::thread> threadPool;

    threadPool.emplace_back([&preventivo,&SalvataggioInCorso]() {


        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::string baseName = preventivo.getId();

        if (baseName.empty()) baseName = "preventivo";

        salvaPreventivoSuTxt(preventivo, baseName + ".txt");
        salvaPreventivoSuCsv(preventivo, baseName + ".csv");

        SalvataggioInCorso = false;

        std::lock_guard<std::mutex> lock(gConsoleMutex);

    });
    while (SalvataggioInCorso) {
        {
            std::lock_guard<std::mutex> lock(gConsoleMutex);
            std::cout << "." << std::flush;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "\nSalvataggio in csv e txt completato! File: " << baseName << "\n";


    // Attesa fine thread (Join)
    for (std::thread& t : threadPool) {
        if (t.joinable()) t.join();
    }
    std::cout << "Premi INVIO per uscire";
    std::string dummy;
    std::getline(std::cin, dummy);

    return 0;
}