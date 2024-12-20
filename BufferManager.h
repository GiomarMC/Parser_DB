#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "Frame.h"
#include <map>
#include <vector>
#include <string>

class BufferManager {
private:
    std::map<int, int> pageTable;
    std::vector<Frame*> bufferPool;
    std::string methodReplace;
    int clockHand;
    int miss;
    int hits;
    int solicitudes;

    Frame* replaceFrameLRU();
    Frame* replaceFrameMRU();
    Frame* replaceFrameCLOCK();

public:
    BufferManager(int poolSize, const std::string& replacementMethod = "CLOCK");
    ~BufferManager();

    void pinPage(int pageId, Page* page, bool accion);
    void unpinPage(int pageId);
    float hitRate();
    void printFrame();
};

#endif
