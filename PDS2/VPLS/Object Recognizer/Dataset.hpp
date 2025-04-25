#ifndef DATASET_H
#define DATASET_H

#include<string>
#include<vector>
#include<map>
#include "Object.hpp"

class Dataset {
private:
    std::string _category;
    std::map<std::string, Object> _objects;

public:
    Dataset(std::string category);
    void add_object(std::string name, std::vector<double> featureVector);
    void remove_object(std::string name);
    void print_summary();
    std::string get_category();
    std::map<std::string, Object> get_objects();
};

#endif