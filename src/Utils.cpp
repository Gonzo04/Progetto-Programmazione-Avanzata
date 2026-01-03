
#include "Utils.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <algorithm>


void pulisciInput() {
    /*
     clear() rimuove eventuali flag di errore
     ignore() scarta il resto della riga
    */
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string trim(const std::string& s) {

    // Trova il primo carattere senza contare spazi/tab
    std::size_t first = s.find_first_not_of(" \t");
    if (first == std::string::npos) return "";

    // Trova l'ultimo carattere
    std::size_t last = s.find_last_not_of(" \t");

    // Estraggo la stringa "pulita"
    return s.substr(first, last - first + 1);
}

static bool restoSoloSpazi(const std::string& str, std::size_t pos) {
    /*
        Funzione helper locale non presente nell'header:
        controlla se dopo pos ci sono solo spazi/tab.
        Serve per evitare l'input tipo "12abc" venga accettato.
    */
    for (std::size_t i = pos; i < str.size(); ++i) {
        if (str[i] != ' ' && str[i] != '\t') return false;
    }
    return true;
}

int leggiIntero(const std::string& messaggio) {
    while (true) {
        std::cout << messaggio;

        std::string linea;
        if (!std::getline(std::cin, linea)) {
            // Se lo stream si chiude, non permette la continuazione
            throw std::runtime_error("Errore irreversibile di input stream.");
        }

        linea = trim(linea);
        if (linea.empty()) {
            std::cout << "Input vuoto. Inserisci un numero.\n";
            continue;
        }

        try {
            std::size_t pos = 0;
            int valore = std::stoi(linea, &pos);

            // Se dopo il numero ci sono caratteri lo considera come input non valido
            if (!restoSoloSpazi(linea, pos)) {
                std::cout << "Input non valido. Sono presenti caratteri extra.\n";
                continue;
            }

            return valore;
        }
        catch (...) {
            std::cout << "Input non valido. Inserisci un numero intero.\n";
        }
    }
}

double leggiDouble(const std::string& messaggio) {
    while (true) {
        std::cout << messaggio;

        std::string linea;
        if (!std::getline(std::cin, linea)) {
            throw std::runtime_error("Errore irreversibile di input stream.");
        }

        linea = trim(linea);
        if (linea.empty()) {
            std::cout << "Input vuoto. Inserisci un numero.\n";
            continue;
        }

        try {
            std::size_t pos = 0;
            double valore = std::stod(linea, &pos);

            if (!restoSoloSpazi(linea, pos)) {
                std::cout << "Input non valido. Sono presenti caratteri extra.\n";
                continue;
            }

            return valore;
        }
        catch (...) {
            std::cout << "Input non valido. Inserisci un numero decimale (usa il punto).\n";
        }
    }
}

bool chiediConferma(const std::string& domanda) {
    while (true) {
        std::cout << domanda << " (S/N): ";

        std::string linea;
        if (!std::getline(std::cin, linea)) {
            // Se lo stream si chiude, scelgo false per default
            return false;
        }

        std::string pulita = trim(linea);
        if (pulita.empty()) continue;

        auto first = static_cast<unsigned char>(pulita[0]);

        char c = static_cast<char>(std::toupper(first));

        if (c == 'S') return true;
        if (c == 'N') return false;

        std::cout << "Risposta non valida. Inserisci S o N.\n";
    }
}

bool nomeClienteValido(const std::string& s) {
    /*
        Regole:
        - il nome non può essere vuoto
        - deve contenere almeno una lettera
        - sono ammessi: lettere, spazio, apostrofo, trattino

        Uso std::all_of (STL) per controllare tutti i caratteri.
    */
    if (s.empty()) return false;

    bool haAlmenoUnaLettera = false;

    const bool ok = std::all_of(s.begin(), s.end(),
        [&](unsigned char ch) {
            if (std::isalpha(ch)) {
                haAlmenoUnaLettera = true;
                return true;
            }
            if (ch == ' ' || ch == '\'' || ch == '-') return true;
            return false;
        }
    );

    return ok && haAlmenoUnaLettera;
}