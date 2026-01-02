#include "SalvataggioPreventivo.h"
#include "Preventivo.h"

#include <fstream>
#include <iomanip>
#include <stdexcept>

void salvaPreventivoSuTxt(const Preventivo& p, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("Impossibile aprire file TXT: " + filename);
    }
    out << p.riepilogo();
}

void salvaPreventivoSuCsv(const Preventivo& p, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("Impossibile aprire file CSV: " + filename);
    }

    out << std::fixed << std::setprecision(2);
    out << "TipoVoce;Nome;Unita;Quantita;PrezzoUnitario;Coefficiente;Subtotale\n";

    const auto& voci = p.getVoci();
    for (const auto& vocePtr : voci) {
        if (!vocePtr) continue;

        // Commento da studente:
        // Evito dynamic_cast (RTTI) sfruttando il polimorfismo:
        // ogni voce concreta sa dire “che tipo è” tramite tipoVoce().
        out << vocePtr->tipoVoce() << ";"
            << vocePtr->getNome() << ";"
            << vocePtr->getUnitaMisura() << ";"
            << vocePtr->getQuantita() << ";"
            << vocePtr->getPrezzoUnitario() << ";"
            << vocePtr->getCoefficiente() << ";"
            << vocePtr->subtotale() << "\n";
    }

    constexpr double IVA = 0.22;
    const double imponibile = p.totale();

    out << "TotaleImponibile;;;;;" << imponibile << "\n";
    out << "IVA(22%);;;;;" << imponibile * IVA << "\n";
    out << "TotaleComplessivo;;;;;" << imponibile * (1.0 + IVA) << "\n";
}
