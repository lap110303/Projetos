#include "Object.hpp"

#include<iostream>
#include<iomanip>

Object::Object(std::string name, std::vector<double> featureVector)
    : _name(name), _featureVector(featureVector) {}

void Object::print_info() {
    std::cout << "Object: " << _name << "   FeatureVector: [";
    for (size_t i = 0; i < _featureVector.size(); ++i) {
        std::cout << std::fixed << std::setprecision(2) << _featureVector[i];
        if (i < _featureVector.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

std::string Object::get_name() {
    return this->_name;
}

std::vector<double> Object::get_featureVector() {
    return this->_featureVector;
}
