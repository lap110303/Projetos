#include "Recognizer.hpp"
#include<iostream>
#include<cmath>
#include<iomanip>
#include<limits>

void Recognizer::add_dataset(std::string category) {
    _datasets.insert(std::make_pair(category, Dataset(category)));
}

void Recognizer::add_object(std::string category, std::string name, std::vector<double> featureVector) {
    auto it = _datasets.find(category);
    if (it == _datasets.end()) {
        Dataset dataset(category);
        it = _datasets.insert({category, dataset}).first;
    }
    it->second.add_object(name, featureVector);
}

void Recognizer::remove_object(std::string category, std::string name) {
    auto it = _datasets.find(category);
    if (it != _datasets.end()) {
        it->second.remove_object(name);
    } else {
        std::cout << "Invalid dataset: " << category << "!" << std::endl;
    }
}

void Recognizer::print_dataset_summary(std::string category) {
    auto it = _datasets.find(category);
    if (_datasets.empty()) {
        std::cout << "There are no datasets!" << std::endl;
    }
    else{
        if (it != _datasets.end()) {
            it->second.print_summary();
        } else {
            std::cout << "Invalid dataset: " << category << "!" << std::endl;
        }
    }
}

double Recognizer::calc_vector_distance(std::vector<double> v1, std::vector<double> v2) {
    double distance = 0.0;
    for (size_t i = 0; i < v1.size(); ++i) {
        double diff = v1[i] - v2[i];
        distance += diff * diff;
    }
    return sqrt(distance);
}

void Recognizer::recognize_object(std::string category, std::vector<double> inputFeature) {
    auto it = _datasets.find(category);
    if (it != _datasets.end()) {
        Dataset& dataset = it->second;
        double minDistance = std::numeric_limits<double>::max();
        std::string probableObject;
        
        std::cout << "Matching object ";
        for (size_t i = 0; i < inputFeature.size(); ++i) {
            std::cout << "[" << std::fixed << std::setprecision(2) << inputFeature[i] << "]";
            if (i < inputFeature.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << " in dataset " << category << std::endl;
        
        for (auto& pair : dataset.get_objects()) {
            Object& obj = pair.second;
            double distance = calc_vector_distance(obj.get_featureVector(), inputFeature);
            std::cout << "Object: " << obj.get_name() << "    Distance: " << std::fixed << std::setprecision(2) << distance << std::endl;
            if (distance < minDistance) {
                minDistance = distance;
                probableObject = obj.get_name();
            }
        }
        
        std::cout << "The input object is probably a " << probableObject << "." << std::endl;
    } else {
        std::cout << "Invalid dataset: " << category << "!" << std::endl;
    }
}
