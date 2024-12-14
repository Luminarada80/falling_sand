#include "Animal.h"
#include "CommonFunctions.h"

void Animal::random_walk() {
    // Generate relative movement in the range [-1, 1]
    int x_move = random_int_gen(-1, 1);
    int y_move = random_int_gen(-1, 1);

    // Calculate new position
    int new_x = xpos + x_move;
    int new_y = ypos + y_move;

    // Clamp the new position to ensure it stays within bounds
    xpos = std::clamp(new_x, 0, grid_width - 1);
    ypos = std::clamp(new_y, 0, grid_height - 1);

    // Update the previous direction
    prev_xdir = x_move;
    prev_ydir = y_move;

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

                if (matrix_map[new_y][new_x] == 1){
                    best_x = new_x;
                    best_y = new_y;
                } else {
                    // Calculate the density for this position
                    float density = CalculateDensity(matrix_map, new_x, new_y);

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

    for (const auto& offset : offsets) {
        int neighbor_x = xpos + offset[0];
        int neighbor_y = ypos + offset[1];

        if (neighbor_x >= 0 && neighbor_x < num_cols && neighbor_y >= 0 && neighbor_y < num_rows) {
            if (matrix_map[neighbor_y][neighbor_x] == 1) {
                // Avoid moving back to the previous position
                if (neighbor_x == prev_xpos && neighbor_y == prev_ypos) {
                    continue;
                }

                // Move to the food
                prev_xpos = xpos;
                prev_ypos = ypos;
                xpos = neighbor_x;
                ypos = neighbor_y;

                // Clamp the position
                xpos = std::clamp(xpos, 0, grid_width - 1);
                ypos = std::clamp(ypos, 0, grid_height - 1);

                prev_xdir = offset[0];
                prev_ydir = offset[1];
                return;
            }
        }
    }
}

// Method definitions
bool Animal::evaluate_sensed_gradient() {
    float density = CalculateDensity(matrix_map, xpos, ypos);
    return density > 0.1;
}