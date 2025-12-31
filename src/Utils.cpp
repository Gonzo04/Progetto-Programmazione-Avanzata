
#include "Utils.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <cctype> // per std::isspace

void pulisciInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string trim(const std::string& s) {
    size_t first = s.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    size_t last = s.find_last_not_of(" \t");
    return s.substr(first, last - first + 1);
}

int leggiIntero(const std::string& messaggio) {
    while (true) {
        std::cout << messaggio;
        std::string linea;
        if (!std::getline(std::cin, linea)) {
             throw std::runtime_error("Errore irreversibile di input stream.");
        }

        if (trim(linea).empty()) {
            std::cout << "Input vuoto. Inserisci un numero.\n";
            continue;
        }

        try {
            size_t pos;
            int valore = std::stoi(linea, &pos);
            // Controllo se ci sono caratteri extra dopo il numero (es. "12abc")
            if (pos != linea.length()) {
                 // Opzionale: controlla se il resto è solo spazio, altrimenti errore
            }
            return valore;
        } catch (...) {
            std::cout << "Input non valido. Inserisci un numero intero.\n";
        }
    }
}

double leggiDouble(const std::string& messaggio) {
    while (true) {
        std::cout << messaggio;
        std::string linea;
        if (!std::getline(std::cin, linea)) throw std::runtime_error("Errore stream");

        if (trim(linea).empty()) {
            std::cout << "Input vuoto. Inserisci un numero.\n";
            continue;
        }

        try {
            size_t pos;
            double valore = std::stod(linea, &pos);
            return valore;
        } catch (...) {
            std::cout << "Input non valido. Inserisci un numero decimale (usa il punto).\n";
        }
    }
}

bool chiediConferma(const std::string& domanda) {
    while (true) {
        std::cout << domanda << " (S/N): ";
        std::string linea;
        if (!std::getline(std::cin, linea)) return false;

        std::string pulita = trim(linea);
        if (pulita.empty()) continue;

        char c = std::toupper(pulita[0]);
        if (c == 'S') return true;
        if (c == 'N') return false;

        std::cout << "Risposta non valida. Inserisci S o N.\n";
    }
}
