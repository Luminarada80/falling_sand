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
#include <cstdlib>
#include <random>
#include <set>
#include "CommonFunctions.h"
#include "Ant.h"
#include "Generator.h"


using namespace std;

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

std::vector<std::vector<int>> createMatrix(int rows, int cols) {
    // Initialize a matrix of size rows x cols with all values set to 0
    std::vector<std::vector<int>> matrix(rows, std::vector<int>(cols, 0));

    return matrix;
}

std::list<Ant> create_ants(int num_ants, vector<vector<int>>& matrix_map, int num_cols, int num_rows){
    // Create multiple ants
    std::list<Ant> ant_list;

    for (int i = 0; i < num_ants; i++) { // Correct initialization of the loop
        int randx = random_int_gen(0, num_cols - 1); // Ensure the range is valid
        int randy = random_int_gen(0, num_rows - 1);

        // Create a new Ant object
        Ant antObj(matrix_map, randx, randy, sf::Color(0, 255, 0, 255));

        // Add the Ant object to the list
        ant_list.push_back(antObj);
    }
    return ant_list;
}

std::list<Generator> create_generators(int num_generators, vector<vector<int>>& matrix_map, int num_cols, int num_rows){
    // Create multiple Generators
    std::list<Generator> generator_list;

    for (int i = 0; i < num_generators; i++) { // Correct initialization of the loop
        int randx = random_int_gen(0, num_cols - 1); // Ensure the range is valid
        int randy = random_int_gen(0, num_rows - 1);

        // Create a new Ant object
        Generator genObj(matrix_map, randx, randy, sf::Color(255, 0, 255, 255));

        // Add the Ant object to the list
        generator_list.push_back(genObj);
    }

    return generator_list;
}

int main() {
    // User settings
    bool make_ones_fall = false;
    bool create_animals = false;
    bool show_density = false;
    bool conways_game_of_life = false;

    // Initialize the matrix
    vector<vector<int>> matrix_map = createMatrix(50, 50);

    int num_rows = matrix_map.size();
    int num_cols = matrix_map[0].size();

    // Matrix dimensions
    const int cellSize = 15; // Size of each cell in pixels

    int previous_pop = 0;

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

    // Handle creating the animal lists
    int num_ants = 10;
    int num_generators = 5;

    std::list<Ant> ant_list = create_ants(0, matrix_map, num_cols, num_rows);
    std::list<Generator> generator_list = create_generators(0, matrix_map, num_cols, num_rows);

    if (create_animals) {
        ant_list = create_ants(num_ants, matrix_map, num_cols, num_rows);
        generator_list = create_generators(num_generators, matrix_map, num_cols, num_rows);
    }

    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {

            // Handle closing the window
            if (event.type == sf::Event::Closed)
                window.close();
            
            // Make ones fall if hit 1
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num1) {
                make_ones_fall = !make_ones_fall;
            }

            // Show Conways game of life if hit 2
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num2) {
                conways_game_of_life = !conways_game_of_life;
            }

            // Show density if hit 3
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num3) {
                show_density = !show_density;
            }

            // Make animals if hit 4
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num4) {
                create_animals = !create_animals;
                if (create_animals) {
                    ant_list = create_ants(num_ants, matrix_map, num_cols, num_rows);
                    generator_list = create_generators(num_generators, matrix_map, num_cols, num_rows);
                }

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

                if (conways_game_of_life) {
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
                }

                if (matrix_map[row][col] == 1) {
                    cellColor = sf::Color(255, 255, 255, 255);
                
                } else if (show_density) {
                    float density = CalculateDensity(matrix_map, col, row);

                    int transparency = std::min(static_cast<int>(15 + density), 255);
                    cellColor = sf::Color(0, 0, 255, transparency);
                }

                for (Ant& ant : ant_list) {
                    if ((col == ant.xpos) && (row == ant.ypos)){
                        cellColor = ant.animalColor;
                        if (matrix_map[row][col] == 1){
                            ant.food += 1;
                        }
                        matrix_map[row][col] == 2;


                        next_matrix_map[row][col] = 0;
                        
                    } 
                }

                for (Generator& gen : generator_list) {
                    if ((col == gen.xpos) && (row == gen.ypos)){
                        cellColor = gen.GeneratorColor;
                        gen.create_life(matrix_map);
                    } 
                }

                // Set cell color
                cell.setFillColor(cellColor);

                cell.setOutlineThickness(1);
                cell.setOutlineColor(sf::Color(255,255,255,25));

                // Draw the cell on the window
                window.draw(cell);
            

            }
        }


        matrix_map = next_matrix_map;

        if (!gamePause) {
            // Track occupied positions
            std::set<std::pair<int, int>> occupied_positions;

            // Populate initial positions
            for (const Ant& ant : ant_list) {
                occupied_positions.insert({ant.xpos, ant.ypos});
            }

            // Iterate over the list of ants using an iterator
            for (auto it = ant_list.begin(); it != ant_list.end(); ) {
                Ant& ant = *it;

                // Remove the current position from the occupied set
                occupied_positions.erase({ant.xpos, ant.ypos});

                // Make the ant walk
                ant.walk();

                // Check if the new position is already occupied
                if (occupied_positions.find({ant.xpos, ant.ypos}) != occupied_positions.end()) {
                    // Revert to previous position to prevent overlap
                    ant.xpos = ant.prev_xpos;
                    ant.ypos = ant.prev_ypos;
                } else {
                    // Update the occupied positions set with the new position
                    occupied_positions.insert({ant.xpos, ant.ypos});
                }

                // Check if the ant has enough food to create a new ant
                if (ant.food >= 20) {
                    // Find a free position for the baby ant
                    std::pair<int, int> baby_position;
                    bool position_found = false;
                    for (int dx = -1; dx <= 1 && !position_found; ++dx) {
                        for (int dy = -1; dy <= 1 && !position_found; ++dy) {
                            int new_x = ant.xpos + dx;
                            int new_y = ant.ypos + dy;
                            if (occupied_positions.find({new_x, new_y}) == occupied_positions.end()) {
                                baby_position = {new_x, new_y};
                                position_found = true;
                            }
                        }
                    }

                    if (position_found) {
                        Ant babyAnt = ant.make_babies();
                        babyAnt.xpos = baby_position.first;
                        babyAnt.ypos = baby_position.second;
                        ant_list.push_back(babyAnt);
                        occupied_positions.insert(baby_position);
                    }
                }

                // Decrease the ant's lifetime
                ant.lifetime -= 1;

                if (!create_animals){
                    it = ant_list.erase(it);
                } else {
                    ++it;
                }

                // Remove the ant if its lifetime has expired
                if (ant.lifetime <= 0) {
                    it = ant_list.erase(it); // Safely remove the ant and update the iterator
                } else {
                    ++it; // Move to the next ant
                }

                if (previous_pop != ant_list.size()) {
                    // cout << "Population: " << ant_list.size() << endl;
                    previous_pop = ant_list.size();
                }
            
            }
            // Iterate over the list of ants using an iterator
            for (auto it = generator_list.begin(); it != generator_list.end(); ) {
                Generator& gen = *it;
                gen.walk();

                if (!create_animals){
                    it = generator_list.erase(it);
                } else {
                    ++it;
                }
            }

            // Make each cell with sand fall one row
            if (make_ones_fall) {
                matrix_map = ones_fall(matrix_map);
            }
            
        }

        // Display the updated window
        window.display();

        

        // Pause for animation
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}


