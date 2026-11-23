#include "Pozycja.h"

Pozycja::Pozycja() : x(0), y(0) {}
Pozycja::Pozycja(int startX, int startY) : x(startX), y(startY) {}

int Pozycja::getX() const { return x; }
int Pozycja::getY() const { return y; }
void Pozycja::setX(int noweX) { x = noweX; }
void Pozycja::setY(int noweY) { y = noweY; }