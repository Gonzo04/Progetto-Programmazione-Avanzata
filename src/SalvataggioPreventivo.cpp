#include "SalvataggioPreventivo.h"
#include "Preventivo.h"

#include <fstream>
#include <iomanip>
#include <stdexcept>


#include <thread>
#include <mutex>
#include <list>
#include <atomic>
#include <chrono>
#include <iostream>

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

    // Mantengo il tuo formato (non perfettamente “7 campi”), ma coerente con il tuo CSV attuale
    out << "TotaleImponibile;;;;;" << imponibile << "\n";
    out << "IVA(22%);;;;;" << imponibile * IVA << "\n";
    out << "TotaleComplessivo;;;;;" << imponibile * (1.0 + IVA) << "\n";
}

bool salvaPreventivoConcorrente(const Preventivo& p, const std::string& baseName) {
    std::cout << "\nSalvataggio in corso";

    std::mutex consoleMutex;
    std::list<std::thread> threadPool;

    std::atomic<int> taskFiniti(0);
    std::atomic<bool> salvataggioOk(true);
    constexpr int NUM_TASK = 2;

    // Task 1: TXT
    threadPool.emplace_back([&]() {
        try {
            std::this_thread::sleep_for(std::chrono::seconds(2)); // simulazione "operazione lunga"
            salvaPreventivoSuTxt(p, baseName + ".txt");
        } catch (const std::exception& e) {
            salvataggioOk = false;
            std::lock_guard<std::mutex> lock(consoleMutex);
            std::cerr << "\nErrore salvataggio TXT: " << e.what() << "\n";
        }
        ++taskFiniti;
    });

    // Task 2: CSV
    threadPool.emplace_back([&]() {
        try {
            std::this_thread::sleep_for(std::chrono::seconds(2)); // simulazione "operazione lunga"
            salvaPreventivoSuCsv(p, baseName + ".csv");
        } catch (const std::exception& e) {
            salvataggioOk = false;
            std::lock_guard<std::mutex> lock(consoleMutex);
            std::cerr << "\nErrore salvataggio CSV: " << e.what() << "\n";
        }
        ++taskFiniti;
    });

    // Puntini finché non finiscono entrambi i task
    while (taskFiniti.load() < NUM_TASK) {
        {
            std::lock_guard<std::mutex> lock(consoleMutex);
            std::cout << "." << std::flush;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Join dei thread
    for (std::thread& t : threadPool) {
        if (t.joinable()) t.join();
    }

    return salvataggioOk.load();
}
