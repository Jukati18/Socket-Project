#ifndef CLAMAV_AGENT_H
#define CLAMAV_AGENT_H
#pragma once
#include <string>

class ClamAVAgent {
public:
    ClamAVAgent();
    ~ClamAVAgent();
    void run();

private:
    std::string scanWithClamAV(const std::string& filePath);
    void showProgressBar_realTime();
};

#endif
