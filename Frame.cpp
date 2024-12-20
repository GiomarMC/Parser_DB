#include "Frame.h"
#include <iostream>

Frame::Frame(int id) : dirtyFlag(false), pinCount(0), lastUsed(0), refBit(false) {
    this->id = id;
    pagina = nullptr;
}

Frame::~Frame() {
    delete pagina;
}

int Frame::getId() {
    return id;
}

int Frame::getPinCount() {
    return pinCount;
}

void Frame::resetFrame() {
    if (pagina) {
        saveChanges();
        delete pagina;
    }
    pagina = nullptr;
    dirtyFlag = false;
    pinCount = 0;
    lastUsed = 0;
}

Page* Frame::getPagina() {
    return pagina;
}

void Frame::setPage(Page* pag) {
    this->pagina = pag;
}

void Frame::incrementPinCount() {
    pinCount++;
}

void Frame::decrementPinCount() {
    if (pinCount > 0) pinCount--;
}

void Frame::setLastUsed(int lused) {
    this->lastUsed = lused;
}

bool Frame::getDirty() {
    return dirtyFlag;
}

void Frame::setDirty(bool accion) {
    dirtyFlag = accion;
}

void Frame::saveChanges() {
    if (dirtyFlag && pagina) {
        pagina->save();
        dirtyFlag = false;
    }
}

void Frame::incrementLastUsed() {
    lastUsed++;
}

int Frame::getLastUsed() {
    return lastUsed;
}

bool Frame::getRefBit() {
    return refBit;
}

void Frame::setRefBit(bool state) {
    this->refBit = state;
}
