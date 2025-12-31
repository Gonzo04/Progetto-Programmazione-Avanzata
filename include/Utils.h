#ifndef UTILS_H
#define UTILS_H

#include <string>



// Pulisce il buffer di input
void pulisciInput();

// Rimuove spazi bianchi a inizio e fine stringa
std::string trim(const std::string& s);

// Legge un intero gestendo gli eventuali errori
int leggiIntero(const std::string& messaggio);

// Legge un double gestendo gli eventuali errori
double leggiDouble(const std::string& messaggio);

// Chiede conferma (S/N) all'utente
bool chiediConferma(const std::string& domanda);

#endif
