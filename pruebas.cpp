#include <omp.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>

using namespace std;

float calculate_distance(float* point_root,float* point_target){
    float distance = 0.0;

    distance = pow(point_root[0] - point_target[0],2) + pow(point_root[1] - point_target[1],2);
    distance = sqrtf(distance);

    cout << point_root[0] << "," << point_target[0] << "," << point_root[1] << "," << point_target[1] << "," << distance << "\n";

    return distance;
}

long int region_query(float** points, long long int point, float epsilon,long long int size){
    long int numero_vecinos = 0;
    float distancia = 0;

    for (long long int i=0; i<size; i++){
        distancia = calculate_distance(points[i],points[point]);
        if (distancia <= epsilon){
            numero_vecinos ++;
        }
    }
    return numero_vecinos-1;
}

void noise_detection(float** points, float epsilon, int min_samples, long long int size) {
    cout << "Step 0" << "\n"; 
    for (long long int i=0; i < size; i++) {
        long int num_vecinos = region_query(points,i,epsilon,size);
        if(num_vecinos>=min_samples){
            //número 2 significa nodo core, los nodos que no sean core de grado uno quedarán en 0
            points[i][2] = 2;
        }
    }
    cout << "Complete" << "\n"; 
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
    const int min_samples = 2;
    const long long int size = 100;
    const string input_file_name = to_string(size)+"_data.csv";
    const string output_file_name = to_string(size)+"_results.csv";    
    float** points = new float*[size];

    for(long long int i = 0; i < size; i++) {
        points[i] = new float[3]{0.0, 0.0, 0.0}; 
        // index 0: position x
        // index 1: position y 
        // index 2: 0 for noise point, 1 for core point
    }

    load_CSV(input_file_name, points, size);

    noise_detection(points, epsilon, min_samples, size);

    for(long long int i = 0; i < size; i++) {
        cout << points[i][0] << "," << points[i][1] << "," << points[i][2] << "\n";
    }

    for(long long int i = 0; i < size; i++) {
        delete[] points[i];
    }
    delete[] points;
    return 0;
}