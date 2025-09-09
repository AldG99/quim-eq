#include "ChemicalCompound.h"
#include "CompoundDatabase.h"
#include <regex>
#include <iostream>
#include <sstream>

ChemicalCompound::ChemicalCompound(const std::string& formula) 
    : molarMass_(0.0), formula_(formula), valid_(false) {
    try {
        parseFormula(formula);
        calculateMolarMass();
        valid_ = true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing formula " << formula << ": " << e.what() << std::endl;
        valid_ = false;
    }
}

std::string ChemicalCompound::parseFormula(const std::string& formula) {
    elements_.clear();
    
    // Clean the formula (remove spaces and states like (s), (l), (g), (aq))
    std::string cleanFormula = formula;
    std::regex stateRegex(R"(\s*\([slgaq]\)\s*)");
    cleanFormula = std::regex_replace(cleanFormula, stateRegex, "");
    
    // Remove extra spaces
    std::regex spaceRegex(R"(\s+)");
    cleanFormula = std::regex_replace(cleanFormula, spaceRegex, "");
    
    // Parse the formula recursively
    parseGroup(cleanFormula, 0, cleanFormula.length(), 1);
    
    return cleanFormula;
}

void ChemicalCompound::parseGroup(const std::string& formula, size_t start, size_t end, int multiplier) {
    size_t i = start;
    
    while (i < end) {
        if (std::isupper(formula[i])) {
            // Element symbol
            std::string element;
            element += formula[i++];
            
            // Check for lowercase letter (part of element symbol)
            while (i < end && std::islower(formula[i])) {
                element += formula[i++];
            }
            
            // Get number following element
            int count = 1;
            if (i < end && std::isdigit(formula[i])) {
                count = 0;
                while (i < end && std::isdigit(formula[i])) {
                    count = count * 10 + (formula[i++] - '0');
                }
            }
            
            // Add to elements map
            elements_[element] += count * multiplier;
            
        } else if (formula[i] == '(') {
            // Find matching closing parenthesis
            int depth = 1;
            size_t groupStart = i + 1;
            i++;
            
            while (i < end && depth > 0) {
                if (formula[i] == '(') depth++;
                else if (formula[i] == ')') depth--;
                i++;
            }
            
            size_t groupEnd = i - 1;
            
            // Get multiplier after closing parenthesis
            int groupMultiplier = 1;
            if (i < end && std::isdigit(formula[i])) {
                groupMultiplier = 0;
                while (i < end && std::isdigit(formula[i])) {
                    groupMultiplier = groupMultiplier * 10 + (formula[i++] - '0');
                }
            }
            
            // Parse the group recursively
            parseGroup(formula, groupStart, groupEnd, multiplier * groupMultiplier);
            
        } else {
            throw std::invalid_argument("Invalid character in formula: " + std::string(1, formula[i]));
        }
    }
}

void ChemicalCompound::calculateMolarMass() {
    molarMass_ = 0.0;
    CompoundDatabase& db = CompoundDatabase::getInstance();
    
    for (const auto& element : elements_) {
        double atomicMass = db.getAtomicMass(element.first);
        molarMass_ += atomicMass * element.second;
    }
}

std::map<std::string, int> ChemicalCompound::getElementCount() const {
    return elements_;
}

double ChemicalCompound::getMolarMass() const {
    return molarMass_;
}

bool ChemicalCompound::isValid() const {
    return valid_;
}

std::string ChemicalCompound::getFormula() const {
    return formula_;
}

std::string ChemicalCompound::getDisplayFormula() const {
    std::string display = formula_;
    
    // Convert numbers to subscripts (simple string replacement)
    std::map<std::string, std::string> subscripts = {
        {"0", "₀"}, {"1", "₁"}, {"2", "₂"}, {"3", "₃"}, {"4", "₄"},
        {"5", "₅"}, {"6", "₆"}, {"7", "₇"}, {"8", "₈"}, {"9", "₉"}
    };
    
    std::string result = display;
    for (const auto& sub : subscripts) {
        size_t pos = 0;
        while ((pos = result.find(sub.first, pos)) != std::string::npos) {
            // Check if it's a number that should be subscripted (not at beginning)
            if (pos > 0 && std::isalpha(result[pos-1])) {
                result.replace(pos, sub.first.length(), sub.second);
                pos += sub.second.length();
            } else {
                pos += sub.first.length();
            }
        }
    }
    
    return result;
}

bool ChemicalCompound::operator==(const ChemicalCompound& other) const {
    return formula_ == other.formula_;
}

bool ChemicalCompound::operator<(const ChemicalCompound& other) const {
    return formula_ < other.formula_;
}

// ChemicalEquation implementation

ChemicalEquation::ChemicalEquation() : balanced_(false) {}

void ChemicalEquation::addReactant(const ChemicalCompound& compound, int coefficient) {
    reactants_.emplace_back(compound, coefficient);
    balanced_ = false;
}

void ChemicalEquation::addProduct(const ChemicalCompound& compound, int coefficient) {
    products_.emplace_back(compound, coefficient);
    balanced_ = false;
}

void ChemicalEquation::setCoefficients(const std::vector<int>& coefficients) {
    if (coefficients.size() != getTotalCompounds()) {
        throw std::invalid_argument("Number of coefficients doesn't match number of compounds");
    }
    
    size_t index = 0;
    for (auto& reactant : reactants_) {
        reactant.second = coefficients[index++];
    }
    for (auto& product : products_) {
        product.second = coefficients[index++];
    }
    
    checkBalance();
}

bool ChemicalEquation::isBalanced() const {
    return balanced_;
}

void ChemicalEquation::checkBalance() {
    std::map<std::string, int> elementCount;
    
    // Add atoms from reactants
    for (const auto& reactant : reactants_) {
        auto elements = reactant.first.getElementCount();
        for (const auto& element : elements) {
            elementCount[element.first] += element.second * reactant.second;
        }
    }
    
    // Subtract atoms from products
    for (const auto& product : products_) {
        auto elements = product.first.getElementCount();
        for (const auto& element : elements) {
            elementCount[element.first] -= element.second * product.second;
        }
    }
    
    // Check if all elements balance
    balanced_ = true;
    for (const auto& element : elementCount) {
        if (element.second != 0) {
            balanced_ = false;
            break;
        }
    }
}

std::string ChemicalEquation::toString() const {
    std::stringstream ss;
    
    // Reactants
    for (size_t i = 0; i < reactants_.size(); ++i) {
        if (i > 0) ss << " + ";
        if (reactants_[i].second > 1) {
            ss << reactants_[i].second;
        }
        ss << reactants_[i].first.getFormula();
    }
    
    ss << " → ";
    
    // Products
    for (size_t i = 0; i < products_.size(); ++i) {
        if (i > 0) ss << " + ";
        if (products_[i].second > 1) {
            ss << products_[i].second;
        }
        ss << products_[i].first.getFormula();
    }
    
    return ss.str();
}

std::string ChemicalEquation::toDisplayString() const {
    std::stringstream ss;
    
    // Reactants
    for (size_t i = 0; i < reactants_.size(); ++i) {
        if (i > 0) ss << " + ";
        if (reactants_[i].second > 1) {
            ss << reactants_[i].second;
        }
        ss << reactants_[i].first.getDisplayFormula();
    }
    
    ss << " → ";
    
    // Products
    for (size_t i = 0; i < products_.size(); ++i) {
        if (i > 0) ss << " + ";
        if (products_[i].second > 1) {
            ss << products_[i].second;
        }
        ss << products_[i].first.getDisplayFormula();
    }
    
    return ss.str();
}

size_t ChemicalEquation::getTotalCompounds() const {
    return reactants_.size() + products_.size();
}

std::vector<std::string> ChemicalEquation::getAllElements() const {
    std::set<std::string> elementSet;
    
    for (const auto& reactant : reactants_) {
        auto elements = reactant.first.getElementCount();
        for (const auto& element : elements) {
            elementSet.insert(element.first);
        }
    }
    
    for (const auto& product : products_) {
        auto elements = product.first.getElementCount();
        for (const auto& element : elements) {
            elementSet.insert(element.first);
        }
    }
    
    return std::vector<std::string>(elementSet.begin(), elementSet.end());
}

void ChemicalEquation::clear() {
    reactants_.clear();
    products_.clear();
    balanced_ = false;
}