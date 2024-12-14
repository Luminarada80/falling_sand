#include "Generator.h"
#include "CommonFunctions.h"

void Generator::walk(){
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

void Generator::random_walk() {
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

void Generator::create_life(std::vector<std::vector<int>>& matrix_map) {
    int num_rows = matrix_map.size();
    int num_cols = matrix_map[0].size();

    // Offsets for neighbors
    int offsets[8][2] = {
        {-1, -1}, {-1, 0}, {-1, +1}, // Top-left, Top, Top-right
        { 0, -1},          { 0, +1}, // Left,        , Right
        {+1, -1}, {+1, 0}, {+1, +1}  // Bottom-left, Bottom, Bottom-right
    };

    // First loop: Check for neighboring cells with value 2
    for (const auto& offset : offsets) {
        int neighbor_x = xpos + offset[0];
        int neighbor_y = ypos + offset[1];

        // Ensure the neighbor is within bounds
        if (neighbor_x >= 0 && neighbor_x < num_cols && neighbor_y >= 0 && neighbor_y < num_rows) {
            if (matrix_map[neighbor_y][neighbor_x] == 2) {
                // Abort the function if a neighbor with value 2 is found
                return;
            }
        }
    }

    // Second loop: Set neighbors to 1 if they are 0
    for (const auto& offset : offsets) {
        int neighbor_x = xpos + offset[0];
        int neighbor_y = ypos + offset[1];

        // Ensure the neighbor is within bounds
        if (neighbor_x >= 0 && neighbor_x < num_cols && neighbor_y >= 0 && neighbor_y < num_rows) {
            if (matrix_map[neighbor_y][neighbor_x] == 0) {
                matrix_map[neighbor_y][neighbor_x] = 1;
            }
        }
    }
}