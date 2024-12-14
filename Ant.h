#ifndef ANT_H
#define ANT_H

#include <vector>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include "Animal.h"


using std::vector;

// Declare the variables and functions here
class Ant : public Animal {
    public:
        // Constructor for Ant, forwarding parameters to Animal's constructor
        Ant(const std::vector<std::vector<int>>& matrix_map, int x, int y, sf::Color color)
            : Animal(matrix_map, x, y, color) {
            lifetime = 200; // Set lifetime for Ant
            }

        Ant make_babies();
        void walk();
        
};

#endif // ANT_H