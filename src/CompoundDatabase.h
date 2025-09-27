#ifndef COMPOUND_DATABASE_H
#define COMPOUND_DATABASE_H

#include <string>
#include <map>
#include <vector>

struct ElementData {
    std::string name;
    std::string symbol;
    double atomicMass;
    int atomicNumber;
    std::string category;
};

struct CompoundData {
    std::string formula;
    std::string name;
    std::string commonName;
    double molarMass;
    std::string state;
    std::string properties;
    bool toxic;
};

class CompoundDatabase {
private:
    std::map<std::string, ElementData> elements_;
    std::map<std::string, CompoundData> compounds_;
    static CompoundDatabase* instance_;
    
    CompoundDatabase();
    void loadElements();
    void loadCompounds();
    
public:
    static CompoundDatabase& getInstance();
    
    double getAtomicMass(const std::string& element) const;
    ElementData getElementData(const std::string& element) const;
    CompoundData getCompoundData(const std::string& formula) const;
    
    bool isValidElement(const std::string& element) const;
    bool isKnownCompound(const std::string& formula) const;
    
    std::vector<std::string> getAllElements() const;
    std::vector<std::string> getAllCompounds() const;
    
    std::string getElementName(const std::string& symbol) const;
    std::string getCompoundName(const std::string& formula) const;
};

#endif // COMPOUND_DATABASE_H