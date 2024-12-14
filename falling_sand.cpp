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

std::list<Ant> create_ants(int num_ants, std::vector<std::vector<int>>& matrix_map, int num_cols, int num_rows) {
    std::list<Ant> ant_list;
    std::set<std::pair<int, int>> used_positions;

    for (int i = 0; i < num_ants; i++) {
        int randx, randy;
        do {
            randx = random_int_gen(0, num_cols - 1);
            randy = random_int_gen(0, num_rows - 1);
        } while (used_positions.find({randx, randy}) != used_positions.end());

        used_positions.insert({randx, randy});

        Ant antObj(matrix_map, randx, randy, sf::Color(0, 255, 0, 255));
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
    bool show_density = false;
    bool conways_game_of_life = false;
    bool make_ants = false;
    bool make_generators = false;
    bool show_objects = true;

    // Initialize the matrix
    vector<vector<int>> matrix_map = createMatrix(50, 50);

    int num_rows = matrix_map.size();
    int num_cols = matrix_map[0].size();

    // Matrix dimensions
    const int cellSize = 25; // Size of each cell in pixels

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

    if (make_ants) {
        ant_list = create_ants(num_ants, matrix_map, num_cols, num_rows);
    }
    if (make_generators) {
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
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                gamePause = !gamePause;
            }
            
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

            // Make ants if hit 4
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num4) {
                make_ants = !make_ants;
                if (make_ants) {
                    ant_list = create_ants(num_ants, matrix_map, num_cols, num_rows);
                }

            }

            // Make generators if hit 5
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num5) {
                make_generators = !make_generators;
                if (make_generators) {
                    generator_list = create_generators(num_generators, matrix_map, num_cols, num_rows);
                }

            }

            // Toggle hiding the objects if hit 6
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num6) {
                show_objects = !show_objects;
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
                                next_matrix_map[row][col] = 1;
                            } else {
                                next_matrix_map[row][col] = 0;
                            }
                        } else { // Cell is currently dead
                            if (num_neighbors == 3) {
                                next_matrix_map[row][col] = 1;
                            } else {
                                next_matrix_map[row][col] = 0;
                            }
                        
                        }
                    }
                }


                if (matrix_map[row][col] == 1 && show_objects) {
                    cellColor = sf::Color(255, 255, 255, 255);
                
                } else if (show_density) {
                    float density = CalculateDensity(matrix_map, col, row);

                    int transparency = std::min(static_cast<int>(15 + density), 255);
                    cellColor = sf::Color(0, 0, 255, transparency);
                }

                // Iterate over the list of ants using an iterator
                if (ant_list.size() > 0) {
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

            // Iterate over the list of ants using an iterator
            if (ant_list.size() > 0) {

                // Populate initial positions
                for (const Ant& ant : ant_list) {
                    occupied_positions.insert({ant.xpos, ant.ypos});
                }
                for (const Generator& generator : generator_list) {
                    occupied_positions.insert({generator.xpos, generator.ypos});
                }

                for (auto it = ant_list.begin(); it != ant_list.end(); ) {
                    Ant& ant = *it;

                    if (!make_ants) {
                        it = ant_list.erase(it);
                    } else {
                        // Remove the current position from the occupied set
                        occupied_positions.erase({ant.xpos, ant.ypos});

                        // Make the ant walk
                        ant.walk();

                        // Collision detection
                        if (occupied_positions.find({ant.xpos, ant.ypos}) != occupied_positions.end()) {
                            ant.xpos = ant.prev_xpos;
                            ant.ypos = ant.prev_ypos;
                        } else {
                            occupied_positions.insert({ant.xpos, ant.ypos});
                        }

                        // Reproduction logic
                        if (ant.food >= 25) {
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

                        // Decrease lifetime and check removal
                        ant.lifetime -= 1;
                        if (ant.lifetime <= 0) {
                            it = ant_list.erase(it);
                        } else {
                            ++it;
                        }
                    }

                        // if (previous_pop != ant_list.size()) {
                        //     // cout << "Population: " << ant_list.size() << endl;
                        //     previous_pop = ant_list.size();
                        // }
                }
            }

            if (generator_list.size() > 0){
                for (auto it = generator_list.begin(); it != generator_list.end(); ) {
                    Generator& gen = *it;

                    if (!make_generators) {
                        it = generator_list.erase(it);
                    } else {
                        occupied_positions.erase({gen.xpos, gen.ypos});

                        gen.walk();

                        if (occupied_positions.find({gen.xpos, gen.ypos}) != occupied_positions.end()) {
                            gen.xpos = gen.prev_xpos;
                            gen.ypos = gen.prev_ypos;
                        } else {
                            occupied_positions.insert({gen.xpos, gen.ypos});
                        }

                        ++it;
                    }
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
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    return 0;
}


