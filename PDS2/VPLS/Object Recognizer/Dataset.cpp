#include "Dataset.hpp"

#include<iostream>

Dataset::Dataset(std::string category) : _category(category) {}

void Dataset::add_object(std::string name, std::vector<double> featureVector) {
    Object object(name, featureVector);
    _objects.insert(std::make_pair(name, object));
}

void Dataset::remove_object(std::string name) {
    auto it = _objects.find(name);
    if (it != _objects.end()) {
        _objects.erase(it);
    } else {
        std::cout << "Invalid object: " << name << "!" << std::endl;
    }
}

void Dataset::print_summary() {
    if (_objects.empty()) {
        std::cout << "Empty dataset: " << _category << "!" << std::endl;
    } else {
        std::cout << "Dataset: " << _category << std::endl;
        for (auto& pair : _objects) {
            pair.second.print_info();
        }
    }
}

std::string Dataset::get_category() {
    return this->_category;
}

std::map<std::string, Object> Dataset::get_objects() {
    return this->_objects;
}
