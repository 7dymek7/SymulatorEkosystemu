#ifndef POZYCJA_H
#define POZYCJA_H

class Pozycja {
private:
    int x;
    int y;

public:
    Pozycja(); // Konstruktor domyślny
    Pozycja(int startX, int startY); // Konstruktor parametryczny

    int getX();
    int getY();
    void setX(int noweX);
    void setY(int noweY);
};

#endif