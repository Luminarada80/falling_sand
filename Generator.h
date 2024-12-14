#ifndef GENERATOR_H

#include <SFML/Graphics.hpp>
#include <vector>

using std::vector;

class Generator {
    public:
        int xpos;
        int ypos;
        sf::Color GeneratorColor;
        int prev_xdir = 0;
        int prev_ydir = 0;

        int prev_xpos = 0;
        int prev_ypos = 0;
        const vector<vector<int>>& matrix_map;

        int grid_height = matrix_map.size();
        int grid_width = matrix_map[0].size();   

        int wander_chance = 25;

        Generator(const vector<vector<int>>& matrix_map, int x, int y, sf::Color color) 
        : matrix_map(matrix_map), xpos(x), ypos(y), GeneratorColor(color),
        grid_height(matrix_map.size()), grid_width(matrix_map[0].size()) {}

        void walk();
        void random_walk();
        void create_life(std::vector<std::vector<int>>& matrix_map);
        
};

#endif //GENERATOR_H