#ifndef SALVATAGGIO_PREVENTIVO_H
#define SALVATAGGIO_PREVENTIVO_H

#include <string>

class Preventivo;

// Salva riepilogo completo in TXT
void salvaPreventivoSuTxt(const Preventivo& p, const std::string& filename);

// Salva dettaglio voci + totali in CSV (separatore ;)
void salvaPreventivoSuCsv(const Preventivo& p, const std::string& filename);

#endif
