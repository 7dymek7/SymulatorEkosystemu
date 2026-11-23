#ifndef POZYCJA_H
#define POZYCJA_H

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
};

#endif