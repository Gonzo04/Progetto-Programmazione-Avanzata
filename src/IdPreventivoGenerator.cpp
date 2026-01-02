#include "IdPreventivoGenerator.h"

#include <atomic>
#include <ctime>
#include <iomanip>
#include <sstream>

// Contatore progressivo: atomico per sicurezza (anche se oggi il main è single-thread)
static std::atomic<int> g_contatorePreventivi(1);

std::string generaIdPreventivo() {
    std::time_t now = std::time(nullptr);

    // Rendiamo la conversione in tm "più robusta":
    // - su Windows: localtime_s
    // - su Linux/macOS: localtime_r
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif

    char data[16];
    std::strftime(data, sizeof(data), "%Y%m%d", &tm);

    int progressivo = g_contatorePreventivi.fetch_add(1);

    std::ostringstream oss;
    oss << "P" << data << "-"
        << std::setw(3) << std::setfill('0') << progressivo;

    return oss.str();
}
