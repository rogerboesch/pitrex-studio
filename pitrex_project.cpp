
#include "pitrex_project.hpp"
#include "rbbasic/basic/rb_bas_interpreter.h"
#include "rbbase/http.hh"
#include <filesystem>
#include <fstream>
#include <streambuf>
#include "rbbase/base64.h"

static PitrexProject* INSTANCE = nullptr;

int MAX_FILES = 4;

PitrexProject::PitrexProject() {
    m_currentNumber = 3;
    BuildFilename();
    
    m_path = "/Users/roger/";
    m_temporaryPath = "~";
    m_server = "192.168.0.129";
    m_port = 33333;
    
    INSTANCE = this;
}

// MARK: - File handling

void PitrexProject::BuildFilename() {
    m_filename = "PROGRAM-" + std::to_string(m_currentNumber) + ".BAS";
}

std::string& PitrexProject::GetTemporaryPath() {
    return m_temporaryPath;
}

std::string& PitrexProject::GetPath() {
    return m_path;
}

std::string& PitrexProject::GetCurrentFile() {
    m_fullPath = m_path + m_filename;
    return m_fullPath;
}

bool PitrexProject::AddNewFile() {
    m_filename = "UNTITLED.BAS";
    return true;
}

bool PitrexProject::SaveCurrentFile(std::string content) {
    std::ofstream outfile;

    std::string path = m_path + m_filename;
    outfile.open(path, std::ios_base::trunc);
    outfile << content;

    return true;
}

bool PitrexProject::LoadCurrentFile(std::string& content) {
    std::string path = m_path + m_filename;
    std::ifstream t(path);
    
    if (!t.good())
        return false;
    
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    content = str;

    return true;
}

bool PitrexProject::PreviousFile() {
    if (m_currentNumber == 1)
        m_currentNumber = MAX_FILES;
    else
        m_currentNumber--;
    
    BuildFilename();
    
    return true;
}

bool PitrexProject::NextFile() {
    if (m_currentNumber == MAX_FILES)
        m_currentNumber = 1;
    else
        m_currentNumber++;
    
    BuildFilename();
    
    return true;
}

// MARK: - Local BASIC integration

void onThreadStarted() {
    INSTANCE->RunInternal();
}

void PitrexProject::RunInternal() {
    m_running = true;

    std::string msg = m_filename + " started.";
    platform_print_console((char *)msg.c_str());

    basic_load((char *)m_filename.c_str(), (char *)m_path.c_str());
    basic_run();
    
    m_running = false;
    
    platform_print_console((char *)"Program finished.");
}

bool PitrexProject::ProgramIsRunning() {
    return m_running;
}

bool PitrexProject::Run() {
    basic_init(48*1024, 512); // memory size, stack size
    m_thread = new std::thread(onThreadStarted);
}

bool PitrexProject::Stop() {
    if (!ProgramIsRunning())
        return;
    
    basic_stop();
}

// MARK: - PiTrex Remote integration

bool PitrexProject::SendCurrentFile() {
    http_client_t client(m_server.c_str(), m_port);

    if (client.connect() < 0) {
        platform_print_console((char *)"Error update BASIC file on PiTrex (1)");
        return false;
    }

    std::string filename = m_path + m_filename;
    std::ifstream t(filename);
    
    if (!t.good()) {
        platform_print_console((char *)"Error update BASIC file on PiTrex (2)");
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    std::string encoded = base64_encode(content);
    
    std::string header;
    header = "GET /file?" + encoded + " HTTP/1.1\r\n";
    header += "Host: ";
    header += m_server;
    header += "\r\n";
    header += "Accept: application/text\r\n";
    header += "Connection: close";
    header += "\r\n";
    header += "\r\n";

    if (client.write_all(header.c_str(), header.size()) < 0) {
        platform_print_console((char *)"Error update BASIC file on PiTrex (3)");
        return false;
    }

    char buf_[4096];
    int size_read;

    if ((size_read = client.read_all(buf_, sizeof(buf_))) < 0) {
        platform_print_console((char *)"Error update BASIC file on PiTrex (4)");
        return false;
    }

    std::string buf(buf_, size_read);
    std::cout << buf.c_str();
    std::string str_body = http_get_body(buf.c_str());

    client.close();
    return true;
}

bool PitrexProject::SendCommand(std::string command, int number) {
    http_client_t client(m_server.c_str(), m_port);

    if (client.connect() < 0) {
        platform_print_console((char *)"Error run BASIC file on PiTrex (1)");
        return false;
    }

    std::string header;
    header = "GET /" + command + "?" + std::to_string(number) + " HTTP/1.1\r\n";
    header += "Host: ";
    header += m_server;
    header += "\r\n";
    header += "Accept: application/text\r\n";
    header += "Connection: close";
    header += "\r\n";
    header += "\r\n";

    if (client.write_all(header.c_str(), header.size()) < 0) {
        platform_print_console((char *)"Error run BASIC file on PiTrex (2)");
        return false;
    }

    char buf_[4096];
    int size_read;

    if ((size_read = client.read_all(buf_, sizeof(buf_))) < 0) {
        platform_print_console((char *)"Error run BASIC file on PiTrex (3)");
        return false;
    }

    std::string buf(buf_, size_read);
    std::cout << buf.c_str();
    std::string str_body = http_get_body(buf.c_str());

    client.close();
    
    return true;
}

bool PitrexProject::RunOnPitrex() {
    platform_print_console((char *)"Run BASIC code on PiTrex");

    if (!SendCurrentFile())
        return false;
    
    if (!SendCommand("run", 0))
        return false;
    
    platform_print_console((char *)"Done.");
    return true;
}
