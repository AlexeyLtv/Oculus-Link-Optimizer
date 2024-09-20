#include "linkpriority.h"
#include "ui_linkpriority.h"
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <cstring>
#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QTextStream>

int globalIndex = 0;
QString globalPath = "C:\\Program Files\\Oculus\\Support\\oculus-client\\OculusClient.exe";

LinkPriority::LinkPriority(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LinkPriority)
{
    ui->setupUi(this);

    // Read File

    QFile file("path_config.txt");
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        globalPath = in.readLine();  // Read path
        file.close();
    } else {
        //Use Default path
        globalPath = "C:\\Program Files\\Oculus\\Support\\oculus-client\\OculusClient.exe";
    }

    qDebug() << "Program started with Path: " << globalPath;
}

LinkPriority::~LinkPriority()
{
    delete ui;
}

//Functions
bool DetectRunning(const char* processName) {
    DWORD processes[1024];
    DWORD cbNeeded;

    if (!EnumProcesses(processes, sizeof(processes), &cbNeeded)) {
        return false;
    }

    DWORD processCount = cbNeeded / sizeof(DWORD);
    for (DWORD i = 0; i < processCount; i++) {
        if (processes[i] == 0) continue;

        TCHAR processFileName[MAX_PATH];
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
        if (hProcess) {
            HMODULE hMod;
            DWORD cbNeededModule;
            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeededModule)) {
                GetModuleBaseName(hProcess, hMod, processFileName, sizeof(processFileName) / sizeof(TCHAR));

                QString processNameQString = QString::fromWCharArray(processFileName);
                QString searchProcessName = QString::fromUtf8(processName);

                if (QString::compare(searchProcessName, processNameQString, Qt::CaseInsensitive) == 0) {
                    CloseHandle(hProcess);
                    return true;
                }
            }
            CloseHandle(hProcess);
        }
    }

    return false;
}
DWORD GetProcessID(const char* processName) {
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    QString searchProcessName = QString::fromUtf8(processName);

    if (Process32First(hSnapshot, &pe)) {
        do {
            QString processNameQString = QString::fromWCharArray(pe.szExeFile);

            if (QString::compare(searchProcessName, processNameQString, Qt::CaseInsensitive) == 0) {
                CloseHandle(hSnapshot);
                return pe.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return 0;
}
void SetProcessPriority(DWORD &pid) {
    QMessageBox msgBox;
    if (pid == 0) {
        msgBox.setText("Process not found.\n");
        return;
    }

    HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, pid);
    if (hProcess == NULL) {
        msgBox.setText("Failed to open process.\n");
        return;
    }

    switch(globalIndex){
    case 0: if (SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS)) {
            qDebug() << "Priority set to: Realtime";
        } else {
            msgBox.setText("Failed to set priority.\n");
        }
        break;
    case 1: if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS)) {
            qDebug() << "Priority set to: High";
        } else {
            msgBox.setText("Failed to set priority.\n");
        }
        break;
    case 2: if (SetPriorityClass(hProcess, ABOVE_NORMAL_PRIORITY_CLASS)) {
            qDebug() << "Priority set to: Above_Normal";
        } else {
            msgBox.setText("Failed to set priority.\n");
        }
        break;
    case 3: if (SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS)) {
            qDebug() << "Priority set to: Normal";
        } else {
            msgBox.setText("Failed to set priority.\n");
        }
        break;
    default: msgBox.setText("Failed to set priority.\n");
    }

    CloseHandle(hProcess);
}
void StartProgram(const QString& program) {
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    std::wstring programWStr = program.toStdWString();
    LPWSTR programWStrC = const_cast<LPWSTR>(programWStr.c_str());

    // Process Create
    if (CreateProcess(NULL, programWStrC, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        // Process Wait
        WaitForSingleObject(pi.hProcess, 2000);  // Wait 2 seconds or adjust as needed

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        QMessageBox msgBox;
        msgBox.setText("Failed to create process.\nError code: " + QString::number(GetLastError()));
        msgBox.exec();
    }
}
void CloseProgram(const char* program) {
    QMessageBox msgBox;

    QString programName = QString::fromUtf8(program);

    // Get Window Name
    HWND hwnd = FindWindow(NULL, programName.toStdWString().c_str());
    if (hwnd) {
        // Request to Close
        SendMessage(hwnd, WM_CLOSE, 0, 0);
    } else {
        msgBox.setText("Window not found\n");
        msgBox.exec();
    }
}

//Buttons
void LinkPriority::on_pushButton_clicked()
{
    const char* processName = "OculusClient.exe";

    if (DetectRunning(processName)) {
        DWORD pid = GetProcessID("OVRServer_x64.exe");
        SetProcessPriority(pid);
        CloseProgram("Meta Quest Link");
        StartProgram(globalPath);  // Pass QString directly
    } else {
        StartProgram(globalPath);  // Pass QString directly
        DWORD pid = GetProcessID("OVRServer_x64.exe");
        SetProcessPriority(pid);
        CloseProgram("Meta Quest Link");
        StartProgram(globalPath);  // Pass QString directly
    }
}
void LinkPriority::on_pushButton_3_clicked()
{
    globalIndex = 0;
}
void LinkPriority::on_pushButton_4_clicked()
{
    globalIndex = 1;
}
void LinkPriority::on_pushButton_5_clicked()
{
    globalIndex = 2;
}
void LinkPriority::on_pushButton_6_clicked()
{
    globalIndex = 3;
}
void LinkPriority::on_Save_clicked()
{
    QString path = ui->Path->text();
    QString Add = "\\Support\\oculus-client\\OculusClient.exe";

    if (path != "C:\\Program Files\\Oculus\\Support\\oculus-client" &&
        path != "D:\\Program Files\\Oculus\\Support\\oculus-client" &&
        path != "E:\\Program Files\\Oculus\\Support\\oculus-client")
    {
        path += Add;
    }

    globalPath = path;

    // Save path
    QFile file("path_config.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << path;
        file.close();
    } else {
        qDebug() << "Failed to save the path to file";
    }

    qDebug() << "Clicked Save, Path: " << globalPath;

}
/*
    0 - realtime
    1 - high
    2 - above normal
    3 - normal
    */
