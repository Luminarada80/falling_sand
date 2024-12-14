#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include <vector>

using std::vector;

float CalculateDistance(int sample_point_x, int sample_point_y, int cell_x, int cell_y);
static float SmoothingKernel(float radius, float dst);
float CalculateDensity(const vector<vector<int>>& matrix_map, int col, int row);
int random_int_gen(int min, int max);
int FindDirectNeighbors(const vector<vector<int>>& matrix_map, int sample_point_x, int sample_point_y);

#endif //COMMON_FUNCTIONS_H