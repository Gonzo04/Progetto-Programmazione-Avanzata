#ifndef SALVATAGGIO_PREVENTIVO_H
#define SALVATAGGIO_PREVENTIVO_H

#include <string>

class Preventivo;

void salvaPreventivoSuTxt(const Preventivo& p, const std::string& filename);

void salvaPreventivoSuCsv(const Preventivo& p, const std::string& filename);

// Salva TXT e CSV in parallelo (thread + atomic). Ritorna true se tutto ok.
bool salvaPreventivoConcorrente(const Preventivo& p, const std::string& baseName);

#endif
