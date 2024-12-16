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
#include <fstream>


using namespace std;

struct AnimalLog {
    int time_step;
    int generation;
    int max_lifetime;
    int food;
    int group_preference;
    int movement_chance;
    int wander_chance;
    int sensing_radius;
    int maturation; 
    int food_per_unit_eaten;
    int food_per_baby;
    int spawning_threshold;
    int spawn_delay;
    int population;
};

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

        Ant antObj(matrix_map, randx, randy);
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

void logAnimalAttributes(const Animal& animal, int time_step, std::vector<AnimalLog>& animalLogs, int population) {
    animalLogs.push_back({
        time_step,
        animal.generation,
        animal.max_lifetime,
        animal.food,
        animal.group_preference,
        animal.movement_chance,
        animal.wander_chance,
        animal.sensing_radius,
        animal.maturation,
        animal.food_per_unit_eaten,
        animal.food_per_baby,
        animal.spawning_threshold,
        animal.spawn_delay,
        population
    });
};

void saveLogsToCSV(const std::vector<AnimalLog>& animalLogs, const std::string& filename) {
    std::ofstream file(filename);

    // Write the header
    file << "time_step,generation,max_lifetime,food,group_preference,movement_chance,"
        << "wander_chance,sensing_radius,maturation,food_per_unit_eaten,"
        << "food_per_baby,spawning_threshold,spawn_delay,population\n";

    // Write each log
    for (const AnimalLog& log : animalLogs) {
        file << log.time_step << ","
            << log.generation << ","
            << log.max_lifetime << ","
            << log.food << ","
            << log.group_preference << ","
            << log.movement_chance << ","
            << log.wander_chance << ","
            << log.sensing_radius << ","
            << log.maturation << ","
            << log.food_per_unit_eaten << ","
            << log.food_per_baby << ","
            << log.spawning_threshold << ","
            << log.spawn_delay << ","
            << log.population << "\n";
    }

    file.close();
}


int main() {
    // ================ SETTINGS =================
    // User settings
    bool gamePause = false;             // Space
    bool make_ones_fall = false;        // Key_1
    bool conways_game_of_life = false;  // Key_2
    bool show_density = false;          // Key 3
    bool make_ants = false;             // Key 4
    bool make_generators = false;       // Key 5
    bool show_objects = true;           // Key 6

    int num_ants_to_save = 50;

    int matrix_height = 100;
    int matrix_width = 100;
    const int cellSize = 10; // Size of each cell in pixels

    // Initialize the matrix
    vector<vector<int>> matrix_map = createMatrix(matrix_height, matrix_width);

    int num_rows = matrix_map.size();
    int num_cols = matrix_map[0].size();    

    int click_delay = 0;
    int save_attribute_delay = 0;

    // Create the SFML window
    sf::RenderWindow window(sf::VideoMode(num_cols * cellSize, num_rows * cellSize), "Falling Sand");

    // Colors for rendering
    sf::Color cellColor = sf::Color(0, 255, 255, 255);
    sf::Color backgroundColor = sf::Color(0,0,0,255);

    // Handle creating the Ants and Generators lists
    int num_ants = 10;
    int num_generators = 10;
    std::list<Ant> ant_list = create_ants(0, matrix_map, num_cols, num_rows);
    std::list<Generator> generator_list = create_generators(0, matrix_map, num_cols, num_rows);

    if (make_ants) {
        ant_list = create_ants(num_ants, matrix_map, num_cols, num_rows);
    }
    if (make_generators) {
        generator_list = create_generators(num_generators, matrix_map, num_cols, num_rows);
    }

    std::vector<AnimalLog> animalLogs;
    int time_step = 0;


    // ============= MAIN GAME LOOP =================
    while (window.isOpen()) {    
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {

            // Handle closing the window
            if (event.type == sf::Event::Closed)
                window.close();
            
            // Pause the simulation if hit space
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

                // Make sure to show objects if the density is turned off, otherwise nothing
                // represents 1's on the map.
                if (show_objects == false && show_density == false){
                    show_objects = !show_objects;
                }
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

                // Make sure to show density if the objects are not being shown, otherwise nothing 
                // represents 1's on the map
                if (show_objects == false && show_density == false){
                    show_density = !show_density;
                }
            }
        }

        // Set cell to 1 if left mouse button is clicked
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            // Get mouse position relative to the window
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            // Convert to grid coordinates
            int col = mousePos.x / cellSize;
            int row = mousePos.y / cellSize;

            // Check bounds and update the grid
            if (row >= 0 && row < num_rows && col >= 0 && col < num_cols) {
                matrix_map[row][col] = 1;
            }



        
        // Set cell to 0 if right mouse button is clicked
        } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            // Get mouse position relative to the window
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            // Convert to grid coordinates
            int col = mousePos.x / cellSize;
            int row = mousePos.y / cellSize;

            // Check bounds and update the grid
            if (row >= 0 && row < num_rows && col >= 0 && col < num_cols) {
                matrix_map[row][col] = 0;
            }
        }

        // Clear the window
        window.clear(backgroundColor);
        std::vector<std::vector<int>> next_matrix_map = matrix_map; // Temporary grid for next state

        int num_ants_saved = 0;

        // Draw and update each cell of the matrix
        for (int row = 0; row < num_rows; row++) {
            for (int col = 0; col < num_cols; col++) {

                // Set up the cell shape as a rectangle with size cellSize * cellSize
                sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));

                // Set the cell position based on the matrix position and cellSize
                cell.setPosition(col * cellSize, row * cellSize);

                // Default: Dead cell (black)
                sf::Color cellColor = sf::Color::Black; 

                // =========== CONWAYS GAME OF LIFE CODE ==============
                if (conways_game_of_life) {
                    if (!gamePause) { 
                        // Count neighbors
                        int num_neighbors = FindDirectNeighbors(matrix_map, col, row);

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

                // Handles setting the cell to 1 (overwritten by other code)
                if (matrix_map[row][col] == 1 && show_objects) {
                    cellColor = sf::Color(255, 255, 255, 255);
                
                // Show the density of dead cells
                } else if (show_density) {
                    float density = CalculateDensity(matrix_map, col, row, 20);

                    // Clamp transparency between 15 and 255
                    int transparency = std::min(static_cast<int>(density), 255);

                    // Clamp green component between 0 and 165
                    int green_component = std::max(0, static_cast<int>(130 - density));

                    // Set the color from orange (255, 165, 0) to red (255, 0, 0)
                    cellColor = sf::Color(0, 0, 255, transparency);
                }

                // Iterate over the list of ants using an iterator
                if (ant_list.size() > 0) {
                    for (Ant& ant : ant_list) {

                        if (save_attribute_delay == 0 && num_ants_saved < num_ants_to_save){
                            int population = ant_list.size();
                            logAnimalAttributes(ant, time_step, animalLogs, population);
                            num_ants_saved ++;
                        }
                        
                        if ((col == ant.xpos) && (row == ant.ypos)){
                            cellColor = ant.animalColor;
                            if (matrix_map[row][col] == 1){
                                ant.food += ant.food_per_unit_eaten;
                            }
                            matrix_map[row][col] == 2;


                            next_matrix_map[row][col] = 0;
                            
                        } 
                    }
                }

                for (Generator& gen : generator_list) {
                    if ((col == gen.xpos) && (row == gen.ypos)){
                        cellColor = gen.GeneratorColor;
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
        // Track occupied positions
        std::set<std::pair<int, int>> occupied_positions;

        if (!gamePause) {

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
                        // cout << "Food = " << ant.food << " Spawning threshold = " << ant.spawning_threshold << endl;
                        if (ant.food >= ant.spawning_threshold) {
                            // cout << "I have enough food to make a baby!\n";

                            // If the animal has not created a baby recently, make a baby
                            if (ant.remaining_spawn_delay <= 0){
                                // cout << "\tI can make a baby!\n";
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
                                    ant.remaining_spawn_delay = ant.spawn_delay;
                                    continue;
                                }

                            // Otherwise, decrease the spawn delay
                            } else {
                                // cout << "\tI need to wait " << ant.spawn_delay << " to reproduce\n";
                                ant.remaining_spawn_delay--;
                            }
                        }

                        // Decrease lifetime and check removal
                        ant.remaining_lifetime -= 1;
                        if (ant.remaining_lifetime <= 0 || ant.food <= 0) {
                            it = ant_list.erase(it);
                        } else {
                            ++it;
                        }
                    }
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

                        gen.prev_xpos = gen.xpos;
                        gen.prev_ypos = gen.ypos;

                        if (occupied_positions.find({gen.xpos, gen.ypos}) != occupied_positions.end()) {
                            gen.xpos = gen.prev_xpos;
                            gen.ypos = gen.prev_ypos;
                        } else {
                            occupied_positions.insert({gen.xpos, gen.ypos});
                        }

                        gen.create_life(matrix_map, occupied_positions);

                        ++it;
                    }
                }
            }

            // Make each cell with sand fall one row
            if (make_ones_fall) {
                matrix_map = ones_fall(matrix_map);
            }

            if (ant_list.size() > 0){
                
                if (save_attribute_delay > 0){
                    save_attribute_delay--;
                } else {
                    cout << "Saving population time " << time_step << " (pop size " << ant_list.size() << ")" << endl;
                    save_attribute_delay = 75;
                    time_step++;
                }
            }
            
        }

        // Set cell to 1 if left mouse button is clicked
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && click_delay == 0) {

            // Get mouse position relative to the window
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            // Convert to grid coordinates
            int col = mousePos.x / cellSize;
            int row = mousePos.y / cellSize;

            for (const Animal& ant : ant_list) {
                // Convert ant position to screen space (assuming xpos, ypos are in grid coordinates)
                int ant_x_pixel = ant.xpos * cellSize;
                int ant_y_pixel = ant.ypos * cellSize;

                // Check if the click falls within the ant's bounding box
                if (mousePos.x >= ant_x_pixel &&
                    mousePos.x < ant_x_pixel + cellSize && // Adjust for ant size if needed
                    mousePos.y >= ant_y_pixel &&
                    mousePos.y < ant_y_pixel + cellSize) {

                    cout << "\nGeneration = " << ant.generation << endl;
                    cout << "max_lifetime = " << ant.max_lifetime << endl;
                    cout << "group_preference = " << ant.group_preference << endl;
                    cout << "movement_chance = " << ant.movement_chance << endl;
                    cout << "wander_chance = " << ant.wander_chance << endl;
                    cout << "sensing_radius = " << ant.sensing_radius << endl;
                    cout << "maturation = " << ant.maturation << endl;
                    cout << "food_per_unit_eaten = " << ant.food_per_unit_eaten << endl;
                    cout << "spawning_threshold = " << ant.spawning_threshold << endl;
                    cout << "spawn_delay = " << ant.spawn_delay << endl;
                    cout << "up preference = " << ant.up_preference << endl;
                    cout << "right preference = " << ant.right_preference << endl;
                    cout << "down preference = " << ant.down_preference << endl;
                    cout << "left preference = " << ant.left_preference << endl;

                    click_delay = 15;
                }
            }
        
        }

        if (click_delay > 0){
            click_delay--;
        }

        

        
        


        // Display the updated window
        window.display();

        

        // Pause for animation
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    saveLogsToCSV(animalLogs, "animal_logs.csv");
    return 0;
}


