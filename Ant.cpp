#include "Ant.h"
#include "CommonFunctions.h"
#include "Animal.h"

#include <iostream>
#include <vector>

using std::vector;

using namespace std;

Ant Ant::make_babies(){
    // Set the position for the baby in the Ant's previous position
    int baby_xpos = std::clamp(prev_xpos, 1, grid_width - 2);
    int baby_ypos = std::clamp(prev_ypos, 1, grid_height - 2);

    // Create a new Ant object
    Ant antObj(matrix_map, baby_xpos, baby_ypos);

    // Check to see if the child's values will be different than the parents
    antObj.max_lifetime = mutate(max_lifetime, 100, 500);
    antObj.group_preference = mutate(group_preference, 1, 100);
    antObj.movement_chance = mutate(movement_chance, 1, 100);
    antObj.wander_chance = mutate(wander_chance, 1, 100);
    antObj.sensing_radius = mutate(sensing_radius, 1, 20);
    antObj.maturation = mutate(maturation, 1, 100);
    antObj.food_per_unit_eaten = mutate(food_per_unit_eaten, 1, 3);
    antObj.spawning_threshold = mutate(spawning_threshold, 25, 150);
    antObj.spawn_delay = mutate(spawn_delay, 50, 200);

    // cout << "\n\nCreated Baby:\n";
    // cout << "\tmax_lifetime = " << antObj.max_lifetime << endl;
    // cout << "\tgroup_preference = " << antObj.group_preference << endl;
    // cout << "\tmovement_chance = " << antObj.movement_chance << endl;
    // cout << "\twander_chance = " << antObj.wander_chance << endl;
    // cout << "\tsensing_radius = " << antObj.sensing_radius << endl;
    // cout << "\tmaturation = " << antObj.maturation << endl;
    // cout << "\tfood_per_unit_eaten = " << antObj.food_per_unit_eaten << endl;
    // cout << "\tspawning_threshold = " << antObj.spawning_threshold << endl;
    // cout << "\tspawn_delay = " << antObj.spawn_delay << endl;

    antObj.red = red;       // Inherit parent's red component
    antObj.green = green;   // Inherit parent's green component
    antObj.blue = blue;     // Inherit parent's blue component


    antObj.animalColor = sf::Color(red, green, blue, 255);

    // Decrease the food
    food -= food_per_baby;

    return antObj;
}

void Ant::walk()
{   
    // Set it up so that there is a chance that the animal does not move
    int rand_movement_roll = random_int_gen(1, 100);
    if (rand_movement_roll > movement_chance) {

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

                if (xpos == 0 || xpos == grid_width - 1) prev_xdir = -prev_xdir;
                if (ypos == 0 || ypos == grid_height - 1) prev_ydir = -prev_ydir;

                xpos = std::clamp(new_x, 0, grid_width - 1);
                ypos = std::clamp(new_y, 0, grid_height - 1);
            }
        }
    }
}

void evolve() {
    return;
}