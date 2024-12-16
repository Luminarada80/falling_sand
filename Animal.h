#ifndef ANIMAL_H
#define ANIMAL_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "CommonFunctions.h"

using std::vector;

class Animal {
    public:
        // POSITIONING
        int xpos;
        int ypos;
        
        int prev_xdir;
        int prev_ydir;

        int prev_xpos;
        int prev_ypos;

        // VISUAL INFORMATION
        int red = random_int_gen(100, 255);
        int green = random_int_gen(100, 255);
        int blue = random_int_gen(100, 255);

        sf::Color animalColor = sf::Color(red, green, blue, 255);

        // MAP INFORMATION
        const vector<vector<int>>& matrix_map;
        int grid_height = matrix_map.size();
        int grid_width = matrix_map[0].size();   

        // SURVIVAL
        int max_lifetime;                       // How long the animal lives before dyng
        int remaining_lifetime;                 // How much of the animals lifetime it has remaining
        int food = 25;                          // The amount of food the animal currently has
        int group_preference = 50;              // How much the animal prefers to stay near other animals of its species
        int movement_chance = 50;               // How likely the animal is to move at any given time

        // MOVEMENT
        int up_preference = 25;                 // Let animals evolve to prefer to move in certain directions
        int right_preference = 25;
        int down_preference = 25;
        int left_preference = 25;

        // GATHERING FOOD
        int wander_chance = 25;                 // The chance to randomly change direction
        int sensing_radius = 10;                // The radius the animal can sense density

        // MAKING BABIES
        int generation = 1;
        int maturation = 50;                    // The percent of the animals life before it creates babies
        int food_per_unit_eaten = 2;            // The amount of food gained per unit eaten
        int food_per_baby = 10;                 // The amount of food it costs to create a baby
        int spawning_threshold = 50;            // The amound of food needed to start making babies
        int spawn_delay = 50;                   // Delay before another baby can be created
        int remaining_spawn_delay = 0;

        // Constructor
        Animal(const vector<vector<int>>& matrix_map, int x, int y)
        : matrix_map(matrix_map), xpos(x), ypos(y),
        grid_height(matrix_map.size()),
        grid_width(matrix_map[0].size()) {
            prev_xdir = random_int_gen(-1, 1);      // Randomly set the previous direction
            prev_ydir = random_int_gen(-1, 1);      
            prev_xpos = x;                          // Set the previous x and y positions to the spawn point
            prev_ypos = y;                          
            remaining_lifetime = max_lifetime;      // Initialize the remaining lifetime as the max
        }

        void random_walk();
        void walk_toward_gradient();
        void walk_to_food();
        bool evaluate_sensed_gradient();
        int mutate(int parent_value, int min_value, int max_value);
        float calculate_offspring_similarity(const Animal& child);



};

#endif //ANIMAL_H