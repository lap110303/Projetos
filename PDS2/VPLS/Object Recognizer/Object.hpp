#ifndef OBJECT_H
#define OBJECT_H

#include<string>
#include<vector>

class Object {
private:
    std::string _name;
    std::vector<double> _featureVector;

public:
    Object(std::string name, std::vector<double> featureVector);
    void print_info();
    std::string get_name();
    std::vector<double> get_featureVector();
};

#endif