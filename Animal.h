#ifndef ANIMAL_H
#define ANIMAL_H

#include <vector>
#include <SFML/Graphics.hpp>

using std::vector;

class Animal {
    public:
        int xpos;
        int ypos;
        sf::Color animalColor;
        int prev_xdir;
        int prev_ydir;

        int prev_xpos;
        int prev_ypos;
        const vector<vector<int>>& matrix_map;

        int grid_height = matrix_map.size();
        int grid_width = matrix_map[0].size();   

        int wander_chance = 25;
        int sensing_radius = 10;

        int lifetime;
        int food = 0;

        // Constructor
        Animal(const vector<vector<int>>& matrix_map, int x, int y, sf::Color color)
        : matrix_map(matrix_map), xpos(x), ypos(y), animalColor(color),
        grid_height(matrix_map.size()),
        grid_width(matrix_map[0].size()) {}

        void random_walk();
        void walk_toward_gradient();
        void walk_to_food();
        bool evaluate_sensed_gradient();
};

#endif //ANIMAL_H