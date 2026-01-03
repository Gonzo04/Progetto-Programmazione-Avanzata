#ifndef SALVATAGGIO_PREVENTIVO_H
#define SALVATAGGIO_PREVENTIVO_H

#include <string>

class Preventivo;

// Salva riepilogo completo in TXT
void salvaPreventivoSuTxt(const Preventivo& p, const std::string& filename);

// Salva riepilogo completo in CSV
void salvaPreventivoSuCsv(const Preventivo& p, const std::string& filename);

/*
    Salvataggio concorrente:
    - crea due thread in parallelo per il salvataggio (TXT e CSV)
    - attende la fine tramite atomic
    - ritorna true se entrambi completano senza eccezioni
*/
bool salvaPreventivoConcorrente(const Preventivo& p, const std::string& baseName);

#endif
