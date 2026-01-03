#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <ctime>


/*
    Utils contiene funzioni di supporto generiche

    - evitare duplicazione di codice nel main e nei menu
    - centralizzare lettura dell'input (getline + parsing)
    - funzioni di utilità (trim, conferma S/N, validazione nome)
    - uso std::getline per leggere tutta la riga e poi converto (stoi/stod)
      così si evitano i problemi classici di std::cin >>
*/

// Pulisce l'input
void pulisciInput();

// Rimuove spazi e tab a inizio/fine stringa
std::string trim(const std::string& s);

// Legge un intero da console (ripete finché valido)
int leggiIntero(const std::string& messaggio);

// Legge un double da console (ripete finché valido)
double leggiDouble(const std::string& messaggio);

// Chiede una conferma (S/N) e ritorna true/false
bool chiediConferma(const std::string& domanda);

// Controllo sul nome cliente: solo lettere/spazi/'/-
bool nomeClienteValido(const std::string& s);

/*
std::localtime utilizza un buffer statico interno condiviso (non è thread-safe).
Se due thread (es. salvataggio e generazione ID) lo chiamano insieme, si rischia una Data Race.
Uso un mutex statico locale per serializzare l'accesso e proteggere il buffer.
*/
std::tm getLocalTimeSafe();
#endif // UTILS_H
