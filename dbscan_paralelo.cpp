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
    //linea para ver que funcione bien, borrar al terminar
    //cout << point_root[0] << "," << point_target[0] << "," << point_root[1] << "," << point_target[1] << "," << distance << "\n";

    return distance;
}

void add_2d_cores(float** points,long long int size,float epsilon){
    float distance = 0;
    long long int i=0;
    long long int j=0;

    #pragma omp parallel for shared(points, size, epsilon) private(i,j)
    for (i=0; i<size; i++){
        if (points[i][2]!=2){
            for (j=0; j<size; j++){
                if (points[j][2]==2){
                    distance = calculate_distance(points[j],points[i]);
                    if (distance <= epsilon){
                        points[i][2]=1;
                    }
                }
            }
        }
    }
}

void no_core_to_core(float** points,long long int size){
    long long int i = 0;
    
    #pragma omp parallel for shared(points,size) private(i)
    for (i=0; i<size; i++){
        if (points[i][2]==1){
            points[i][2]=2;
        }
    }
}

long int region_query(float** points, long long int point, float epsilon,long long int size,long long int chunk_size){
    long int numero_vecinos = 0;
    float distancia = 0;
    long long int i = 0;

    #pragma omp parallel shared(points,epsilon,numero_vecinos,chunk_size) private(i,distancia)
    {
        #pragma omp for schedule(static,chunk_size) reduction(+:numero_vecinos)
        for (i=0; i<size;i++){
            distancia = calculate_distance(points[i],points[point]);
            if (distancia <= epsilon) {
                numero_vecinos++;
            }
        }
    }
    return numero_vecinos;
}

void noise_detection(float** points, float epsilon, int min_samples, long long int size,long long int chunk_size) {
    int i=0;
    //#pragma omp parallel for shared(points,epsilon,min_samples,chunk_size,size) private(i) 
    for (i=0; i < size; i++){
        long int num_vecinos = region_query(points,i,epsilon,size,chunk_size);
        if(num_vecinos>=min_samples){
            //número 2 significa nodo core, los nodos que no sean core de grado uno quedarán en 0
            points[i][2] = 2;
        }
    }
    add_2d_cores(points,size,epsilon);
    no_core_to_core(points,size);
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
    //parametrizar programa
    //parámetros para paralelizar
    int num_hilos = 8;
    //const int num_hilos = atoi(argv[2]);
    const float epsilon = 0.03;
    const int min_samples = 10;
    const long long int size = atol(argv[1]);
    //const long long int size = 1000;
    const string input_file_name = to_string(size)+"_data.csv";
    const string output_file_name = to_string(size)+"_results_paralelo.csv";    
    float** points = new float*[size];
    double start = 0;
    double end = 0;
    long long int chunk_size = size/num_hilos;

    for(long long int i = 0; i < size; i++) {
        points[i] = new float[3]{0.0, 0.0, 0.0}; 
        // index 0: position x
        // index 1: position y 
        // index 2: 0 for noise point, 1 for core point
    }

    load_CSV(input_file_name, points, size);

    omp_set_num_threads(num_hilos);

    start = omp_get_wtime();

    noise_detection(points, epsilon, min_samples, size, chunk_size);

    end = omp_get_wtime();

    save_to_CSV(output_file_name, points, size);
    
    //Imprimir resultados
    cout << "(" << end - start << "," << size << "," << num_hilos << ")" << "\n";

    for(long long int i = 0; i < size; i++) {
        delete[] points[i];
    }
    delete[] points;
    return 0;
}