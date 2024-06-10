/* Librerias a utilizar
  ACT. INTEGRADORA 1: Conceptos básicos ya lgoritmos fundamentales
  Kevin Alejandro Ramírez Luna      A01711063 
  Francisco Rafael Arreola Corona A01706242
  Damian Reza Ortiz                 A01644088*/
//---------------------------------------------------------------------
//Este código intenta usar el uso del mergueshort() para ordenar un arreglo de objetos de la clase "Bitacora". En las posteriores etapas de este código se implementará el uso de la búsqueda binaria para encontrar la fecha de inicio y fin de algún posible elemento de nuestra base de datos "bitacora.txt"
//Notas importantes a considerar: El programa tienen un tiempo de ejecución muy grande por todas las busquedas que debe realizar
//---------------------------------------------------------------------
//Librerias a utilizar para nuestro código
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <iomanip>

class LogEntry {
private:
    std::string ip;
    std::tm date;
    std::string reason;

public:
    LogEntry(const std::string& date_str, const std::string& time_str, const std::string& ip, const std::string& reason)
        : ip(ip), reason(reason) {
        std::istringstream ss(date_str + " " + time_str);
        ss >> std::get_time(&date, "%b %d %H:%M:%S");
        if (ss.fail()) {
            throw std::runtime_error("Error al analizar la fecha: " + date_str + " " + time_str);
        }
    }

    std::tm get_date() const {
        return date;
    }

    void set_date(const std::tm& new_date) {
        date = new_date;
    }

    friend bool operator<(const LogEntry& a, const LogEntry& b) {
        return std::mktime(const_cast<std::tm*>(&a.date)) < std::mktime(const_cast<std::tm*>(&b.date));
    }

    friend std::ostream& operator<<(std::ostream& os, const LogEntry& entry) {
        std::stringstream ss;
        ss << std::put_time(const_cast<std::tm*>(&entry.date), "%b %d %H:%M:%S");
        os << ss.str() << " " << entry.ip << " " << entry.reason;
        return os;
    }
};

std::vector<LogEntry> read_log_entries(const std::string& filename) {
    std::vector<LogEntry> log_entries;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Error al abrir el archivo");
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string month, day, time_str, ip, reason;

        ss >> month >> day >> time_str >> ip;
        std::getline(ss, reason);

        std::string date_str = month + " " + day;

        try {
            log_entries.emplace_back(date_str, time_str, ip, reason);
        } catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    return log_entries;
}

void quick_sort(std::vector<LogEntry>& log_entries, int low, int high) {
    if (low < high) {
        LogEntry pivot = log_entries[high];
        int i = low - 1;
        for (int j = low; j <= high - 1; j++) {
            if (log_entries[j] < pivot) {
                i++;
                std::swap(log_entries[i], log_entries[j]);
            }
        }
        std::swap(log_entries[i + 1], log_entries[high]);
        int pivot_index = i + 1;

        quick_sort(log_entries, low, pivot_index - 1);
        quick_sort(log_entries, pivot_index + 1, high);
    }
}

std::pair<std::vector<LogEntry>::iterator, std::vector<LogEntry>::iterator> search_log_entries(std::vector<LogEntry>& log_entries, const std::tm& start_date, const std::tm& end_date) {
    std::tm start_date_copy = start_date;
    std::tm end_date_copy = end_date;

    std::time_t start_time = std::mktime(&start_date_copy);
    std::time_t end_time = std::mktime(&end_date_copy);

    auto it_start = std::lower_bound(log_entries.begin(), log_entries.end(), start_time, [](const LogEntry& entry, const std::time_t& time) {
        std::tm entry_date_copy = entry.get_date();
        return std::mktime(&entry_date_copy) < time;
    });

    auto it_end = std::upper_bound(log_entries.begin(), log_entries.end(), end_time, [](const std::time_t& time, const LogEntry& entry) {
        std::tm entry_date_copy = entry.get_date();
        return time < std::mktime(&entry_date_copy);
    });

    return std::make_pair(it_start, it_end);
}

void print_log_entries(const std::vector<LogEntry>& log_entries) {
    for (const auto& entry : log_entries) {
        std::cout << entry << "\n";
    }
}

void save_log_entries(const std::string& filename, const std::vector<LogEntry>& log_entries) {
    std::ofstream outfile(filename);
    if (outfile.is_open()) {
        for (const auto& entry : log_entries) {
            outfile << entry << "\n";
        }
        outfile.close();
    } else {
        std::cerr << "No se pudo abrir el archivo para guardar la bitácora ordenada.\n";
    }
}

int main() {
    try {
        std::vector<LogEntry> log_entries;
        std::string filename = "bitacora.txt";

        // Lectura de la bitácora
        log_entries = read_log_entries(filename);

        // Ordenamiento de la bitácora con quicksort
        quick_sort(log_entries, 0, log_entries.size() - 1);

        // Interfaz con el usuario para ingresar fechas de búsqueda
        std::cout << "Ingrese la fecha de inicio (mes día hora:minuto:segundo): ";
        std::tm start_date = {};
        std::cin >> std::get_time(&start_date, "%b %d %H:%M:%S");

        std::cout << "Ingrese la fecha de fin (mes día hora:minuto:segundo): ";
        std::tm end_date = {};
        std::cin >> std::get_time(&end_date, "%b %d %H:%M:%S");

        // Búsqueda de registros en la bitácora
        auto range = search_log_entries(log_entries, start_date, end_date);

        // Verificación de las fechas
        if (range.first == log_entries.end() || range.second == log_entries.end()) {
            std::cout << "Una de las fechas no se encuentra en la bitácora.\n";
        } else {
            // Despliegue de los registros correspondientes al rango de fechas
            std::vector<LogEntry> filtered_entries(range.first, range.second);
            std::cout << "Registros encontrados en el rango de fechas:\n";
            print_log_entries(filtered_entries);
            std::cout << "Total de registros encontrados: " << filtered_entries.size() << "\n";

            // Almacenamiento del resultado del ordenamiento en un archivo "bitacora_ordenada.txt"
            save_log_entries("bitacora_ordenada.txt", filtered_entries);
            std::cout << "¡Bitácora ordenada guardada exitosamente!\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
