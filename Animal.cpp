#include "Animal.h"
#include "CommonFunctions.h"
#include <random>
#include <iostream>
using namespace std;

void Animal::random_walk() {
    // Generate weights for directional movement
    int direction_weights[4] = {
        up_preference,    // Moving up (-1, 0)
        right_preference, // Moving right (0, +1)
        down_preference,  // Moving down (+1, 0)
        left_preference   // Moving left (0, -1)
    };

    // Calculate the total weight
    int total_weight = 0;
    for (int weight : direction_weights) {
        total_weight += weight;
    }

    // Generate a random number within the total weight
    int roll = random_int_gen(1, total_weight);

    // Determine direction based on weighted probabilities
    int x_move = 0, y_move = 0; // Default no movement
    if (roll <= direction_weights[0]) { // Move up
        x_move = -1;
    } else if (roll <= direction_weights[0] + direction_weights[1]) { // Move right
        y_move = 1;
    } else if (roll <= direction_weights[0] + direction_weights[1] + direction_weights[2]) { // Move down
        x_move = 1;
    } else { // Move left
        y_move = -1;
    }

    // Calculate new position
    int new_x = xpos + x_move;
    int new_y = ypos + y_move;

    // Clamp the new position to ensure it stays within bounds
    xpos = std::clamp(new_x, 0, grid_width - 1);
    ypos = std::clamp(new_y, 0, grid_height - 1);

    // Update the previous direction
    prev_xdir = x_move;
    prev_ydir = y_move;

    // Decrease the animal's food by one
    food--;
}

void Animal::walk_toward_gradient() {
    float max_density = -1.0f;
    int best_x = xpos;
    int best_y = ypos;

    // Iterate over all positions within the sensing radius
    for (int dx = -sensing_radius; dx <= sensing_radius; ++dx) {
        for (int dy = -sensing_radius; dy <= sensing_radius; ++dy) {
            int new_x = xpos + dx;
            int new_y = ypos + dy;

            // Skip the current position and ensure bounds
            if ((dx != 0 || dy != 0) && new_x >= 0 && new_x < grid_width && new_y >= 0 && new_y < grid_height) {
                // Avoid moving back to the previous position
                if (new_x == prev_xpos && new_y == prev_ypos) {
                    continue;
                }

                // If the next position is food, that's the best position to move to
                if (matrix_map[new_y][new_x] == 1){
                    best_x = new_x;
                    best_y = new_y;
                } else {
                    // Calculate the density for this position
                    float density = CalculateDensity(matrix_map, new_x, new_y, sensing_radius);

                    // Keep track of the position with the highest density
                    if (density > max_density) {
                        max_density = density;
                        best_x = new_x;
                        best_y = new_y;

                    // Randomly choose between equal values
                    } else if (density == max_density) {
                        if (random_int_gen(0, 1) == 1) {
                            best_x = new_x;
                            best_y = new_y;
                        }
                    }
                }

                
            }
        }
    }

    // Move to the position with the highest density
    prev_xpos = xpos;
    prev_ypos = ypos;

    xpos = best_x;
    ypos = best_y;

    // Clamp the position
    xpos = std::clamp(xpos, 1, grid_width - 2);
    ypos = std::clamp(ypos, 1, grid_height - 2);

    // Update the previous direction
    prev_xdir = best_x - xpos;
    prev_ydir = best_y - ypos;

    // Decrease the animal's food by one
    food--;

}

void Animal::walk_to_food() {
    std::vector<std::pair<int, int>> neighbors;
    int num_rows = matrix_map.size();
    int num_cols = matrix_map[0].size();

    // Offsets for neighbors
    int offsets[8][2] = {
        {-1, -1}, {-1, 0}, {-1, +1}, // Top-left, Top, Top-right
        { 0, -1},          { 0, +1}, // Left,        , Right
        {+1, -1}, {+1, 0}, {+1, +1}  // Bottom-left, Bottom, Bottom-right
    };

    // Store the direction of the closest food
    int closest_x_offset = 0;
    int closest_y_offset = 0;
    double closest_distance = std::numeric_limits<double>::max();

    // Search for food in neighboring cells
    for (const auto& offset : offsets) {
        int neighbor_x = xpos + offset[0];
        int neighbor_y = ypos + offset[1];

        if (neighbor_x >= 0 && neighbor_x < num_cols && neighbor_y >= 0 && neighbor_y < num_rows) {
            if (matrix_map[neighbor_y][neighbor_x] == 1) {
                // Calculate distance to the food
                double distance = std::sqrt(offset[0] * offset[0] + offset[1] * offset[1]);
                if (distance < closest_distance) {
                    closest_distance = distance;
                    closest_x_offset = offset[0];
                    closest_y_offset = offset[1];
                }
            }
        }
    }

    // If food is found, move one step toward it
    if (closest_distance < std::numeric_limits<double>::max()) {
        prev_xpos = xpos;
        prev_ypos = ypos;

        xpos += closest_x_offset; // Take one step in the x-direction
        ypos += closest_y_offset; // Take one step in the y-direction

        // Clamp the position
        xpos = std::clamp(xpos, 0, grid_width - 1);
        ypos = std::clamp(ypos, 0, grid_height - 1);

        // Update direction
        prev_xdir = closest_x_offset;
        prev_ydir = closest_y_offset;

        // Decrease the animal's food by one
        food--;

        return;
    }
}

bool Animal::evaluate_sensed_gradient() {
    float density = CalculateDensity(matrix_map, xpos, ypos, sensing_radius);
    cout << "sensed density " << density << endl;
    return density > 0.1;
}

int Animal::mutate(int parent_value, int min_value, int max_value) {
    static std::random_device rd;          // Seed
    static std::mt19937 gen(rd());         // Random number generator

    int mutation_roll = random_int_gen(1, 100); // Assuming random_int_gen is using gen internally
    if (mutation_roll > 95) {
        std::normal_distribution<float> mutation(parent_value, 0.1 * (max_value - min_value)); // Small variance
        int mutated_value = static_cast<int>(mutation(gen));
        return std::clamp(mutated_value, min_value, max_value);
    } else {
        return std::clamp(parent_value, min_value, max_value);
    }
}

float Animal::calculate_offspring_similarity(const Animal& child) {
    int total_attributes = 9; // Total number of attributes to compare
    int unchanged_attributes = 0;

    if (max_lifetime == child.max_lifetime) unchanged_attributes++;
    if (group_preference == child.group_preference) unchanged_attributes++;
    if (movement_chance == child.movement_chance) unchanged_attributes++;
    if (wander_chance == child.wander_chance) unchanged_attributes++;
    if (sensing_radius == child.sensing_radius) unchanged_attributes++;
    if (maturation == child.maturation) unchanged_attributes++;
    if (food_per_unit_eaten == child.food_per_unit_eaten) unchanged_attributes++;
    if (spawning_threshold == child.spawning_threshold) unchanged_attributes++;
    if (spawn_delay == child.spawn_delay) unchanged_attributes++;

    return static_cast<float>(unchanged_attributes) / total_attributes; // Returns a value between 0 and 1
}