#include "CommonFunctions.h"
#include <random>
#include <cmath>

int FindDirectNeighbors(const vector<vector<int>>& matrix_map, int sample_point_x, int sample_point_y) {
    std::vector<std::pair<int, int>> neighbors;
    int num_neighbors = 0;

    int num_rows = matrix_map.size();
    int num_cols = matrix_map[0].size();

    // Offsets for neighbors
    int offsets[8][2] = {
        {-1, -1}, {-1, 0}, {-1, +1}, // Top-left, Top, Top-right
        { 0, -1},          { 0, +1}, // Left,        , Right
        {+1, -1}, {+1, 0}, {+1, +1}  // Bottom-left, Bottom, Bottom-right
    };

    // Check all neighbors
    for (const auto& offset : offsets) {
        int neighbor_x = sample_point_x + offset[0];
        int neighbor_y = sample_point_y + offset[1];

        // Check if neighbor is within bounds
        if (neighbor_x >= 0 && neighbor_x < num_cols && neighbor_y >= 0 && neighbor_y < num_rows) {
            // Add neighbor coordinates to the list
            neighbors.emplace_back(neighbor_x, neighbor_y);
            if (matrix_map[neighbor_y][neighbor_x] == 1) {
                num_neighbors++;
            }
        }
    }

    return num_neighbors;
}

float CalculateDistance(int sample_point_x, int sample_point_y, int cell_x, int cell_y)
{
    float dx = cell_x - sample_point_x;
    float dy = cell_y - sample_point_y;
    float dst = sqrt(dx * dx + dy * dy);

    return dst;
}

static float SmoothingKernel(float radius, float dst)
{
    float volume = M_PI * pow(radius, 8) / 4;
    float value = std::max(0.0f, radius * radius - dst * dst);
    return value * value * value / volume;
}

float CalculateDensity(const vector<vector<int>>& matrix_map, int col, int row) {
    float density = 0.0f;
    const float mass = 1000.0f;
    int smoothing_radius = 10;

    int num_rows = matrix_map.size();
    int num_cols = matrix_map[0].size();

    float influence = 0.0f;

    // Calculate bounds for the smoothing radius
    int min_row = std::max(0, row - smoothing_radius);
    int max_row = std::min(num_rows - 1, row + smoothing_radius);
    int min_col = std::max(0, col - smoothing_radius);
    int max_col = std::min(num_cols - 1, col + smoothing_radius);

    // Iterate only within the smoothing radius
    for (int test_row = min_row; test_row <= max_row; test_row++) {
        for (int test_col = min_col; test_col <= max_col; test_col++) {
            if (matrix_map[test_row][test_col] == 1) {
                float distance = CalculateDistance(test_col, test_row, col, row);

                // Only consider points within the smoothing radius
                if (distance <= smoothing_radius) {
                    influence += SmoothingKernel(smoothing_radius, distance);
                }
            }
        }
    }

    density = mass * influence;

    return density;
}

int random_int_gen(int min, int max) {
    static std::random_device rd;         // Seed
    static std::mt19937 gen(rd());        // Random number generator
    std::uniform_int_distribution<int> dist(min, max); // Distribution
    return dist(gen);
}