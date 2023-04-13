#include <iostream>
#include <Windows.h>

int main() {
    // Stoppa il servizio "Spooler"
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!scm) {
        std::cerr << "Impossibile aprire il gestore dei servizi\n";
        return 1;
    }
    SC_HANDLE service = OpenService(scm, "Spooler", SERVICE_ALL_ACCESS);
    if (!service) {
        std::cerr << "Impossibile aprire il servizio Spooler\n";
        CloseServiceHandle(scm);
        return 1;
    }
    SERVICE_STATUS serviceStatus;
    if (!ControlService(service, SERVICE_CONTROL_STOP, &serviceStatus)) {
        std::cerr << "Impossibile fermare il servizio Spooler\n";
        CloseServiceHandle(service);
        CloseServiceHandle(scm);
        return 1;
    }
    std::cout << "Servizio Spooler fermato con successo\n";
    CloseServiceHandle(service);
    CloseServiceHandle(scm);

    // Cancella i file all'interno della cartella specificata
    const std::wstring folderPath = L"C:\\Path\\To\\Folder";
    std::wstring searchPath = folderPath + L"\\*.*";
    WIN32_FIND_DATA findData;
    HANDLE findHandle = FindFirstFile(searchPath.c_str(), &findData);
    if (findHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Impossibile trovare i file nella cartella specificata\n";
        return 1;
    }
    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            std::wstring filePath = folderPath + L"\\" + findData.cFileName;
            if (!DeleteFile(filePath.c_str())) {
                std::cerr << "Impossibile eliminare il file " << filePath << '\n';
            } else {
                std::wcout << L"File eliminato: " << filePath << '\n';
            }
        }
    } while (FindNextFile(findHandle, &findData));
    FindClose(findHandle);

    // Riavvia il servizio "Spooler"
    scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!scm) {
        std::cerr << "Impossibile aprire il gestore dei servizi\n";
        return 1;
    }
    service = OpenService(scm, "Spooler", SERVICE_ALL_ACCESS);
    if (!service) {
        std::cerr << "Impossibile aprire il servizio Spooler\n";
        CloseServiceHandle(scm);
        return 1;
    }
    if (!StartService(service, 0, NULL)) {
        std::cerr << "Impossibile avviare il servizio Spooler\n";
        CloseServiceHandle(service);
        CloseServiceHandle(scm);
        return 1;
    }
    std::cout << "Servizio Spooler avviato con successo\n";
    CloseServiceHandle(service);
    CloseServiceHandle(scm);

    return 0;
}
