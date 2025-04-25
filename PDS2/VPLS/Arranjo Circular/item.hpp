#ifndef ITEM_H
#define ITEM_H

#include <iostream>

class Item {
private:
    int _key;
    int _val;

public:
    Item(){
        _key = 0;
        _val = 0;
    }

    Item(int key, int val){
        _key = key;
        _val = val;
    }

    friend std::ostream& operator<<(std::ostream& out, const Item& i) {
        out << "(" << i._key << "," << i._val << ")";
        return out;
    }

    friend bool operator>(const Item& i1, const Item& i2) {
        if (i1._val > i2._val){
            return true;
        } else{
            return false;
        }
    }
};

#endif
