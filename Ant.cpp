#include "Ant.h"
#include "CommonFunctions.h"
#include "Animal.h"

#include <iostream>
#include <vector>

using std::vector;

Ant Ant::make_babies(){
    // Set the position for the baby in the Ant's previous position
    int baby_xpos = std::clamp(prev_xpos, 1, grid_width - 2);
    int baby_ypos = std::clamp(prev_ypos, 1, grid_height - 2);

    // Create a new Ant object
    Ant antObj(matrix_map, baby_xpos, baby_ypos, sf::Color(0, 255, 0, 255));

    // Reset the food
    food = 0;

    return antObj;
}

void Ant::walk()
{   
    if (FindDirectNeighbors(matrix_map, xpos, ypos) > 0) {
        walk_to_food();

    } else if (evaluate_sensed_gradient()) {
        int walk_toward_food_chance = random_int_gen(0, 100);
        if (walk_toward_food_chance > wander_chance){
            walk_toward_gradient();
            

        } else {
            xpos += prev_xdir;
            ypos += prev_ydir;
        }

    } else {
        
        // Randomly walk
        int change_dir_roll = random_int_gen(0, 100);
        if ((change_dir_roll < wander_chance) || (prev_xdir + prev_ydir == 0)){
            random_walk();

        // Continue in the same direction
        } else {
            // Calculate the new position
            int new_x = xpos + prev_xdir;
            int new_y = ypos + prev_ydir;

            // Clamp the new position to ensure it stays within bounds
            xpos = std::clamp(new_x, 0, grid_width - 1);
            ypos = std::clamp(new_y, 0, grid_height - 1);
        }
    }
}