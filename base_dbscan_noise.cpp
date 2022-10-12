#include <omp.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>

using namespace std;


void noise_detection(float** points, float epsilon, int min_samples, long long int size) {
    cout << "Step 0" << "\n"; 
    for (long long int i=0; i < size; i++) {
        points[i][2] = rand() % 2;
    }      
    cout << "Complete" << "\n"; 
}

float calculate_distance(float* point_root,float* point_target){
    float distance = 0.0;

    distance = pow(point_root[0] - point_target[0],2) + pow(point_root[1] - point_target[1],2);
    distance = sqrtf(distance);

    return distance;
}

//idea erni:
// que esto esté dentro del 1er loop. 

// los puntos evaluados con la función no deberían ser core
void add_new_cores(float** core_points, long long int core_size, float** noise_points, int noise_size,float epsilon) {
    //la 1ra pasada identifica los puntos que castean radio
    // esta pasada ve si está dentro de la distancia. Por el punto dado, calc la dist con esos core_points
    for(long long int i=0; i < core_size; i++){
        for(long long int j=0; j < noise_size; j++){
            float dist = calculate_distance(noise_points[j], core_points[i]);
            if(dist > epsilon){
                noise_points[j][2] = 1.0;
            }
            else {
                noise_points[j][2] = 0;
            }

        }
        
    }

}

void load_CSV(string file_name, float** points, long long int size) {
    ifstream in(file_name);
    if (!in) {
        cerr << "Couldn't read file: " << file_name << "\n";
    }
    long long int point_number = 0; 
    while (!in.eof() && (point_number < size)) {
        char* line = new char[12];
        streamsize row_size = 12;
        in.read(line, row_size);
        string row = line;
        //cout << stof(row.substr(0, 5)) << " - " << stof(row.substr(6, 5)) << "\n";
        points[point_number][0] = stof(row.substr(0, 5));
        points[point_number][1] = stof(row.substr(6, 5));
        point_number++;
    }
}

void save_to_CSV(string file_name, float** points, long long int size) {
    fstream fout;
    fout.open(file_name, ios::out);
    for (long long int i = 0; i < size; i++) {
        fout << points[i][0] << ","
             << points[i][1] << ","
             << points[i][2] << "\n";
    }
}

int main(int argc, char** argv) {

    const float epsilon = 0.03;
    const int min_samples = 10;
    const long long int core_size = 150;
    const long long int not_core_size = 150;
    const string input_file_name_0 = "300_test_data_0.csv";
    const string input_file_name_1 = "300_test_data_1.csv";
    const string output_file_name = "test_results.csv";

    float** core_points = new float*[core_size];
    float** not_core_points = new float*[not_core_size];

    for(long long int i = 0; i < core_size; i++) {
        core_points[i] = new float[3]{0.0, 0.0, 0.0}; 
        // index 0: position x
        // index 1: position y 
        // index 2: 0 for noise point, 1 for core point
    }
    for(long long int i = 0; i < not_core_size; i++) {
        not_core_points[i] = new float[3]{0.0, 0.0, 0.0}; 
        // index 0: position x
        // index 1: position y 
        // index 2: 0 for noise point, 1 for core point
    }    

    load_CSV(input_file_name_0, not_core_points, not_core_size);
    load_CSV(input_file_name_1, core_points, core_size);
    

    add_new_cores(core_points, core_size, not_core_points, not_core_size, epsilon);

    save_to_CSV(output_file_name, not_core_points, not_core_size);

    // termina prueba


    for(long long int i = 0; i < not_core_size; i++) {
        delete[] not_core_points[i];
    }
    delete[] not_core_points;
    return 0;

    for(long long int i = 0; i < core_size; i++) {
        delete[] core_points[i];
    }
    delete[] core_points;
    return 0;
}