#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include<map>
#include<vector>
#include<string>
#include "Dataset.hpp"

struct Recognizer {
private:
    std::map<std::string, Dataset> _datasets;

public:
    void add_dataset(std::string category);
    void add_object(std::string category, std::string name, std::vector<double> featureVector);
    void remove_object(std::string category, std::string name);
    void print_dataset_summary(std::string category);
    double calc_vector_distance(const std::vector<double> v1, std::vector<double> v2);
    void recognize_object(std::string category, std::vector<double> inputFeature);
};

#endif