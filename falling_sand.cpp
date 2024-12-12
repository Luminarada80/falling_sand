#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <cmath>
#include <math.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <thread> // For sleep_for
#include <chrono> // For time durations
#include <algorithm>
#include <tgmath.h> 


using namespace std;


std::vector<std::vector<int>> createMatrix(int rows, int cols) {
    // Initialize a matrix of size rows x cols with all values set to 0
    std::vector<std::vector<int>> matrix(rows, std::vector<int>(cols, 0));

    return matrix;
}

void print_map(vector<vector<int>> matrix_map) {
    int num_rows = matrix_map.size();
    int num_cols = matrix_map[0].size();

    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_cols; col++) {
            cout << matrix_map[row][col];
            };
        cout << endl;
    };
};

vector<vector<int>> ones_fall(vector<vector<int>> matrix_map) {
    int num_rows = matrix_map.size();
    int num_cols = matrix_map[0].size();

    // Start at the last row and check for 1's moving up
    for (int row = num_rows - 1; row >= 0; row--) {

        // Iterate across the rows
        for (int col = 0; col < num_cols; col++) {

            // If a 1 is encountered
            if (matrix_map[row][col] == 1) {
                
                // Move the 1 to the next row unless its the last row and if there is space below
                if (row != num_rows - 1 && matrix_map[row + 1][col] == 0) {
                    matrix_map[row][col] = 0;
                    matrix_map[row + 1][col] = 1;
                };
                
            };

        };
        
    };
    return matrix_map;
};

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


int main() {

    // Initialize the matrix
    vector<vector<int>> matrix_map = createMatrix(75, 150);

    int num_rows = matrix_map.size();
    int num_cols = matrix_map[0].size();

    // Matrix dimensions
    const int cellSize = 15; // Size of each cell in pixels

    // Create the SFML window
    sf::RenderWindow window(sf::VideoMode(num_cols * cellSize, num_rows * cellSize), "Falling Sand");

    // Colors for rendering
    sf::Color cellColor = sf::Color(0, 255, 255, 255);
    sf::Color backgroundColor = sf::Color(0,0,0,255);

    // Initialize a grid to store the last interaction time for each cell
    std::vector<std::vector<std::chrono::time_point<std::chrono::steady_clock>>> lastInteractionTime(
        num_rows, std::vector<std::chrono::time_point<std::chrono::steady_clock>>(num_cols, std::chrono::steady_clock::now())
    );
    bool gamePause = false;

    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {

            // Handle closing the window
            if (event.type == sf::Event::Closed)
                window.close();
            
            // Toggle gamePause with Space key
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                gamePause = !gamePause;
            }

        }


        // Handle mouse click events
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            // Get mouse position relative to the window
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            // Convert to grid coordinates
            int col = mousePos.x / cellSize;
            int row = mousePos.y / cellSize;

            // Check bounds and update the grid
            if (row >= 0 && row < num_rows && col >= 0 && col < num_cols) {
                // Get the current time
                auto now = std::chrono::steady_clock::now();

                // Check if the lockout period has passed
                auto lockoutDuration = std::chrono::milliseconds(500); // 500ms lockout
                if (now - lastInteractionTime[row][col] >= lockoutDuration) {
                    // Toggle the cell's state
                    matrix_map[row][col] = 1;

                    // Update the last interaction time for the cell
                    lastInteractionTime[row][col] = now;
                }
            }

        } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            // Get mouse position relative to the window
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            // Convert to grid coordinates
            int col = mousePos.x / cellSize;
            int row = mousePos.y / cellSize;

            // Check bounds and update the grid
            if (row >= 0 && row < num_rows && col >= 0 && col < num_cols) {
                // Get the current time
                auto now = std::chrono::steady_clock::now();

                // Check if the lockout period has passed
                auto lockoutDuration = std::chrono::milliseconds(500); // 500ms lockout
                if (now - lastInteractionTime[row][col] >= lockoutDuration) {
                    // Toggle the cell's state
                    matrix_map[row][col] = 0;

                    // Update the last interaction time for the cell
                    lastInteractionTime[row][col] = now;
                }
            }
        }
        
        // Clear the window
        window.clear(backgroundColor);
        std::vector<std::vector<int>> next_matrix_map = matrix_map; // Temporary grid for next state

        // Draw and update the matrix
        for (int row = 0; row < num_rows; row++) {
            for (int col = 0; col < num_cols; col++) {
                sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
                cell.setPosition(col * cellSize, row * cellSize);

                sf::Color cellColor = sf::Color::Black; // Default: Dead cell (black)

                int num_neighbors = 0;
                int max_distance = 1; // Typical Game of Life uses direct neighbors only

                if (!gamePause) { 
                    // Count neighbors
                    num_neighbors = FindDirectNeighbors(matrix_map, col, row);

                    // Conway's game of life
                    // Determine cell state based on number of neighbors
                    if (matrix_map[row][col] == 1) { // Cell is currently alive
                        if (num_neighbors == 2 || num_neighbors == 3) {
                            cellColor = sf::Color(255, 255, 255, 255); // Stay alive
                            next_matrix_map[row][col] = 1;
                        } else {
                            cellColor = sf::Color(0, 0, 0, 255); // Death by isolation or overcrowding
                            next_matrix_map[row][col] = 0;
                        }
                    } else { // Cell is currently dead
                        if (num_neighbors == 3) {
                            cellColor = sf::Color(255, 255, 255, 255); // Birth
                            next_matrix_map[row][col] = 1;
                        } else {
                            cellColor = sf::Color(0, 0, 0, 255); // Remain dead
                            next_matrix_map[row][col] = 0;
                        }
                    }
                }

                if (matrix_map[row][col] == 1) {
                    cellColor = sf::Color(255, 255, 255, 255);
                }
                // } else {
                //     float density = CalculateDensity(matrix_map, col, row);

                //     int transparency = std::min(static_cast<int>(15 + density), 255);
                //     cellColor = sf::Color(0, 0, 255, transparency);
                // }

                // Set cell color
                cell.setFillColor(cellColor);

                cell.setOutlineThickness(1);
                cell.setOutlineColor(sf::Color(255,255,255,25));

                // Draw the cell on the window
                window.draw(cell);
            

            }
        }

        // Update matrix for the next generation
        matrix_map = next_matrix_map;

        // Display the updated window
        window.display();


        // // Make each cell with sand fall one row
        // matrix_map = ones_fall(matrix_map);

        // Pause for animation
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}


