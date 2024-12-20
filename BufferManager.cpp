#include "BufferManager.h"
#include <iostream>
#include <limits>

BufferManager::BufferManager(int poolSize, const std::string& replacementMethod) 
    : miss(0), hits(0), solicitudes(0), clockHand(0), methodReplace(replacementMethod) {
    bufferPool.resize(poolSize);
    for (int i = 0; i < poolSize; i++) {
        bufferPool[i] = new Frame(i);
    }
}

BufferManager::~BufferManager() {
    for (auto& frame : bufferPool) {
        delete frame;
    }
}

void BufferManager::pinPage(int pageId, Page* page, bool accion) {
    solicitudes++;
    if (pageTable.count(pageId)) {
        hits++;
        int frameIndex = pageTable[pageId];
        Frame* frame = bufferPool[frameIndex];
        frame->incrementPinCount();
        frame->setDirty(accion);
        frame->setLastUsed(solicitudes);
    } else {
        miss++;
        Frame* frameToUse = nullptr;
        for (auto& frame : bufferPool) {
            if (!frame->getPagina()) {
                frameToUse = frame;
                break;
            }
        }

        if (!frameToUse) {
            if (methodReplace == "LRU") frameToUse = replaceFrameLRU();
            else if (methodReplace == "CLOCK") frameToUse = replaceFrameCLOCK();
            else frameToUse = replaceFrameMRU();
        }

        if (frameToUse->getPagina()) {
            pageTable.erase(frameToUse->getPagina()->id);
            frameToUse->saveChanges();
        }

        frameToUse->resetFrame();
        frameToUse->setPage(page);
        frameToUse->setLastUsed(solicitudes);
        frameToUse->setDirty(accion);
        frameToUse->incrementPinCount();
        pageTable[pageId] = frameToUse->getId();
    }
}

void BufferManager::unpinPage(int pageId) {
    if (pageTable.count(pageId)) {
        int frameIndex = pageTable[pageId];
        bufferPool[frameIndex]->decrementPinCount();
    }
}

float BufferManager::hitRate() {
    return (float)hits / solicitudes * 100;
}

void BufferManager::printFrame() {
    for (auto& frame : bufferPool) {
        std::cout << "Frame " << frame->getId() << " - ";
        if (frame->getPagina()) {
            std::cout << "Página: " << frame->getPagina()->id
                      << ", PinCount: " << frame->getPinCount()
                      << ", Dirty: " << frame->getDirty()
                      << ", LastUsed: " << frame->getLastUsed() << "\n";
        } else {
            std::cout << "Vacío\n";
        }
    }
}

// Reemplazo LRU
Frame* BufferManager::replaceFrameLRU() {
    Frame* lruFrame = nullptr;
    int minUsed = std::numeric_limits<int>::max();
    for (auto& frame : bufferPool) {
        if (frame->getPinCount() == 0 && frame->getLastUsed() < minUsed) {
            minUsed = frame->getLastUsed();
            lruFrame = frame;
        }
    }
    return lruFrame;
}

// Reemplazo CLOCK
Frame* BufferManager::replaceFrameCLOCK() {
    while (true) {
        Frame* frame = bufferPool[clockHand];
        if (frame->getPinCount() == 0) {
            if (!frame->getRefBit()) {
                return frame;
            } else {
                frame->setRefBit(false);
            }
        }
        clockHand = (clockHand + 1) % bufferPool.size();
    }
}

// Reemplazo MRU
Frame* BufferManager::replaceFrameMRU() {
    Frame* mruFrame = nullptr;
    int maxUsed = 0;
    for (auto& frame : bufferPool) {
        if (frame->getPinCount() == 0 && frame->getLastUsed() > maxUsed) {
            maxUsed = frame->getLastUsed();
            mruFrame = frame;
        }
    }
    return mruFrame;
}
