#ifndef POZYCJA_H
#define POZYCJA_H

#include <iostream>

using namespace std;

class Pozycja {
private:
    int x;
    int y;

public:
    Pozycja();
    Pozycja(int startX, int startY);

    int getX() const;
    int getY() const;
    void setX(int noweX);
    void setY(int noweY);

    bool operator==(const Pozycja& inna) const;
    friend ostream& operator<<(ostream& os, const Pozycja& poz);
};

#endif