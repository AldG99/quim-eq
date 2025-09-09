#ifndef CHEMICAL_COMPOUND_H
#define CHEMICAL_COMPOUND_H

#include <string>
#include <map>
#include <vector>
#include <set>

class ChemicalCompound {
private:
    std::map<std::string, int> elements_;
    double molarMass_;
    std::string formula_;
    bool valid_;
    std::string parseFormula(const std::string& formula);
    void parseGroup(const std::string& formula, size_t start, size_t end, int multiplier);
    void calculateMolarMass();
    
public:
    ChemicalCompound(const std::string& formula);
    ChemicalCompound(const ChemicalCompound& other) = default;
    ChemicalCompound& operator=(const ChemicalCompound& other) = default;
    
    std::map<std::string, int> getElementCount() const;
    double getMolarMass() const;
    bool isValid() const;
    std::string getFormula() const;
    std::string getDisplayFormula() const; // With subscripts for display
    
    bool operator==(const ChemicalCompound& other) const;
    bool operator<(const ChemicalCompound& other) const;
};

class ChemicalEquation {
private:
    std::vector<std::pair<ChemicalCompound, int>> reactants_;
    std::vector<std::pair<ChemicalCompound, int>> products_;
    bool balanced_;
    
public:
    ChemicalEquation();
    
    void addReactant(const ChemicalCompound& compound, int coefficient = 1);
    void addProduct(const ChemicalCompound& compound, int coefficient = 1);
    void setCoefficients(const std::vector<int>& coefficients);
    
    bool isBalanced() const;
    void checkBalance();
    
    std::string toString() const;
    std::string toDisplayString() const; // With subscripts for display
    
    std::vector<std::pair<ChemicalCompound, int>>& getReactants() { return reactants_; }
    std::vector<std::pair<ChemicalCompound, int>>& getProducts() { return products_; }
    
    const std::vector<std::pair<ChemicalCompound, int>>& getReactants() const { return reactants_; }
    const std::vector<std::pair<ChemicalCompound, int>>& getProducts() const { return products_; }
    
    size_t getTotalCompounds() const;
    std::vector<std::string> getAllElements() const;
    
    void clear();
};

#endif // CHEMICAL_COMPOUND_H