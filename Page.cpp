#include "Page.h"
#include <iostream>

Page::Page(int id) : id(id) {}

void Page::save() {
    std::cout << "Guardando página " << id << " en disco.\n";
}
