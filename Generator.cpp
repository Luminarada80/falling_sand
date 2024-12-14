#include "Generator.h"
#include "CommonFunctions.h"
#include <set>

void Generator::walk(){
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

void Generator::create_life(std::vector<std::vector<int>>& matrix_map, std::set<std::pair<int, int>>& occupied_positions) {
    int num_rows = matrix_map.size();
    int num_cols = matrix_map[0].size();

    // Offsets for neighbors
    int offsets[8][2] = {
        {-1, -1}, {-1, 0}, {-1, +1}, // Top-left, Top, Top-right
        { 0, -1},          { 0, +1}, // Left,        , Right
        {+1, -1}, {+1, 0}, {+1, +1}  // Bottom-left, Bottom, Bottom-right
    };

    // Decrement the stop_generating counter
    if (stop_generating > 0) {
        stop_generating--; // Countdown frames
        return; // Skip the generation logic
    }

    // Check if the current position or any neighbor is occupied
    bool is_occupied = false;
    for (const auto& offset : offsets) {
        int neighbor_x = xpos + offset[0];
        int neighbor_y = ypos + offset[1];

        // Check if the neighbor is within bounds
        if (neighbor_x >= 0 && neighbor_x < num_cols && neighbor_y >= 0 && neighbor_y < num_rows) {
            std::pair<int, int> position = {neighbor_x, neighbor_y};
            if (occupied_positions.find(position) != occupied_positions.end()) {
                is_occupied = true;
                break; // Stop checking further if occupied
            }
        }
    }

    // If occupied, set stop_generating to 10 frames and return
    if (is_occupied) {
        stop_generating = 10;
        return;
    }

    // Process neighbors and update the matrix
    for (const auto& offset : offsets) {
        int neighbor_x = xpos + offset[0];
        int neighbor_y = ypos + offset[1];

        // Ensure the neighbor is within bounds
        if (neighbor_x >= 0 && neighbor_x < num_cols && neighbor_y >= 0 && neighbor_y < num_rows) {
            // If the cell is 0 and not in occupied_positions, set it to 1
            std::pair<int, int> position = {neighbor_x, neighbor_y};
            if (matrix_map[neighbor_y][neighbor_x] == 0 && occupied_positions.find(position) == occupied_positions.end()) {
                matrix_map[neighbor_y][neighbor_x] = 1;
            }
        }
    }
}

