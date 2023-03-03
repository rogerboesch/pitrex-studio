
#pragma once

#include <string>
#include <thread>

class PitrexProject {
public:
    PitrexProject();
    
public:
    std::string& GetTemporaryPath();
    std::string& GetPath();
    std::string& GetCurrentFile();
    bool AddNewFile();
    bool SaveCurrentFile(std::string content);
    bool LoadCurrentFile(std::string& content);
    bool PreviousFile();
    bool NextFile();

    bool Run();
    bool Stop();
    void RunInternal();
    bool ProgramIsRunning();
    
    bool RunOnPitrex();

private:
    void BuildFilename();
    bool SendCommand(std::string command, int number);
    bool SendCurrentFile();
    
private:
    std::string m_temporaryPath;
    std::string m_path;
    std::string m_filename;
    std::string m_fullPath;
    
    int m_currentNumber;
    
    std::thread* m_thread;
    bool m_running = false;

    std::string m_server;
    int m_port;
};
