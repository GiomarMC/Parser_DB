#ifndef FRAME_H
#define FRAME_H

#include "Page.h"

class Frame {
private:
    int id;
    Page* pagina;
    bool dirtyFlag;
    int pinCount;
    int lastUsed;
    bool refBit;

public:
    Frame(int id);
    ~Frame();

    int getId();
    int getPinCount();
    void resetFrame();
    Page* getPagina();
    void setPage(Page* pag);
    void incrementPinCount();
    void decrementPinCount();
    void setLastUsed(int lused);
    bool getDirty();
    void setDirty(bool accion);
    void saveChanges();
    void incrementLastUsed();
    int getLastUsed();
    bool getRefBit();
    void setRefBit(bool state);
};

#endif
