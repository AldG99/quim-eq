#include "EquationBalancer.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <iomanip>

std::vector<std::vector<double>> EquationBalancer::buildStoichiometricMatrix(const ChemicalEquation& equation) {
    auto elements = equation.getAllElements();
    auto reactants = equation.getReactants();
    auto products = equation.getProducts();
    
    int numElements = elements.size();
    int numCompounds = reactants.size() + products.size();
    
    std::vector<std::vector<double>> matrix(numElements, std::vector<double>(numCompounds, 0.0));
    
    addBalancingStep("Building stoichiometric matrix for elements: " + 
                    [&elements]() {
                        std::string result;
                        for (size_t i = 0; i < elements.size(); ++i) {
                            if (i > 0) result += ", ";
                            result += elements[i];
                        }
                        return result;
                    }());
    
    // Fill matrix for reactants (positive coefficients in matrix)
    for (size_t compIndex = 0; compIndex < reactants.size(); ++compIndex) {
        auto elementCount = reactants[compIndex].first.getElementCount();
        
        for (size_t elemIndex = 0; elemIndex < elements.size(); ++elemIndex) {
            auto it = elementCount.find(elements[elemIndex]);
            if (it != elementCount.end()) {
                matrix[elemIndex][compIndex] = it->second;
            }
        }
    }
    
    // Fill matrix for products (negative coefficients in matrix)
    for (size_t compIndex = 0; compIndex < products.size(); ++compIndex) {
        auto elementCount = products[compIndex].first.getElementCount();
        
        for (size_t elemIndex = 0; elemIndex < elements.size(); ++elemIndex) {
            auto it = elementCount.find(elements[elemIndex]);
            if (it != elementCount.end()) {
                matrix[elemIndex][reactants.size() + compIndex] = -it->second;
            }
        }
    }
    
    addBalancingStep("Matrix constructed with " + std::to_string(numElements) + 
                    " equations and " + std::to_string(numCompounds) + " unknowns");
    
    return matrix;
}

void EquationBalancer::addBalancingStep(const std::string& step) {
    balancingSteps_.push_back(step);
}

std::string EquationBalancer::formatMatrix(const std::vector<std::vector<double>>& matrix, 
                                         const std::vector<std::string>& elements, 
                                         const ChemicalEquation& equation) {
    std::stringstream ss;
    
    // Header with compound formulas
    ss << "        ";
    auto reactants = equation.getReactants();
    auto products = equation.getProducts();
    
    for (const auto& reactant : reactants) {
        ss << std::setw(8) << reactant.first.getFormula();
    }
    for (const auto& product : products) {
        ss << std::setw(8) << product.first.getFormula();
    }
    ss << "\n";
    
    // Matrix rows with element labels
    for (size_t row = 0; row < matrix.size(); ++row) {
        ss << std::setw(4) << elements[row] << " │";
        for (size_t col = 0; col < matrix[row].size(); ++col) {
            ss << std::setw(7) << std::fixed << std::setprecision(0) << matrix[row][col] << " ";
        }
        ss << "│ = 0\n";
    }
    
    return ss.str();
}

BalanceInfo EquationBalancer::balance(ChemicalEquation& equation) {
    balancingSteps_.clear();
    solver_.clearSteps();
    
    BalanceInfo info;
    info.result = BalanceResult::SUCCESS;
    info.conservationVerified = false;
    
    addBalancingStep("Starting equation balancing process");
    addBalancingStep("Original equation: " + equation.toString());
    
    // Check if already balanced
    if (equation.isBalanced()) {
        info.result = BalanceResult::ALREADY_BALANCED;
        info.message = "Equation is already balanced";
        
        // Extract current coefficients
        for (const auto& reactant : equation.getReactants()) {
            info.coefficients.push_back(reactant.second);
        }
        for (const auto& product : equation.getProducts()) {
            info.coefficients.push_back(product.second);
        }
        
        info.conservationVerified = true;
        info.atomBalance = getAtomBalance(equation);
        return info;
    }
    
    try {
        // Build stoichiometric matrix
        auto matrix = buildStoichiometricMatrix(equation);
        auto elements = equation.getAllElements();
        
        addBalancingStep("Stoichiometric matrix:");
        addBalancingStep(formatMatrix(matrix, elements, equation));
        
        // Solve using Gaussian elimination
        addBalancingStep("Solving system of linear equations using Gaussian elimination");
        
        auto solution = solver_.gaussianElimination(matrix);
        
        if (solution.empty()) {
            info.result = BalanceResult::NO_SOLUTION;
            info.message = "No solution exists for this equation";
            return info;
        }
        
        addBalancingStep("Raw solution found: " + 
                        [&solution]() {
                            std::stringstream ss;
                            for (size_t i = 0; i < solution.size(); ++i) {
                                if (i > 0) ss << ", ";
                                ss << std::fixed << std::setprecision(3) << solution[i];
                            }
                            return ss.str();
                        }());
        
        // Convert to integers
        auto integerCoeffs = solver_.reduceToIntegers(solution);
        
        addBalancingStep("Converting to smallest integer coefficients: " +
                        [&integerCoeffs]() {
                            std::stringstream ss;
                            for (size_t i = 0; i < integerCoeffs.size(); ++i) {
                                if (i > 0) ss << ", ";
                                ss << integerCoeffs[i];
                            }
                            return ss.str();
                        }());
        
        // Check for valid coefficients
        for (int coeff : integerCoeffs) {
            if (coeff <= 0) {
                info.result = BalanceResult::NO_SOLUTION;
                info.message = "Invalid coefficients found (zero or negative)";
                return info;
            }
        }
        
        // Apply coefficients to equation
        equation.setCoefficients(integerCoeffs);
        info.coefficients = integerCoeffs;
        
        // Verify balance
        info.conservationVerified = validateAtomConservation(equation);
        info.atomBalance = getAtomBalance(equation);
        
        if (info.conservationVerified) {
            info.message = "Equation balanced successfully";
            addBalancingStep("Final balanced equation: " + equation.toString());
            addBalancingStep("Atom conservation verified ✓");
        } else {
            info.result = BalanceResult::INVALID_EQUATION;
            info.message = "Balancing failed - atom conservation violated";
            addBalancingStep("ERROR: Atom conservation failed ✗");
        }
        
    } catch (const std::exception& e) {
        info.result = BalanceResult::PARSING_ERROR;
        info.message = "Error during balancing: " + std::string(e.what());
        addBalancingStep("ERROR: " + info.message);
    }
    
    return info;
}

std::vector<std::string> EquationBalancer::getBalancingSteps() const {
    return balancingSteps_;
}

std::vector<SolutionStep> EquationBalancer::getMathematicalSteps() const {
    return solver_.getSteps();
}

void EquationBalancer::clearSteps() {
    balancingSteps_.clear();
    solver_.clearSteps();
}

bool EquationBalancer::validateAtomConservation(const ChemicalEquation& equation) {
    auto atomBalance = getAtomBalance(equation);
    
    for (const auto& element : atomBalance) {
        if (element.second != 0) {
            return false;
        }
    }
    
    return true;
}

std::map<std::string, int> EquationBalancer::getAtomBalance(const ChemicalEquation& equation) {
    std::map<std::string, int> balance;
    
    // Count atoms from reactants (positive)
    for (const auto& reactant : equation.getReactants()) {
        auto elements = reactant.first.getElementCount();
        for (const auto& element : elements) {
            balance[element.first] += element.second * reactant.second;
        }
    }
    
    // Count atoms from products (negative)
    for (const auto& product : equation.getProducts()) {
        auto elements = product.first.getElementCount();
        for (const auto& element : elements) {
            balance[element.first] -= element.second * product.second;
        }
    }
    
    return balance;
}

ChemicalEquation EquationBalancer::parseEquationString(const std::string& equationStr) {
    ChemicalEquation equation;
    
    // Split by arrow
    std::regex arrowRegex(R"(\s*(?:->|→)\s*)");
    std::vector<std::string> sides;
    
    std::sregex_token_iterator iter(equationStr.begin(), equationStr.end(), arrowRegex, -1);
    std::sregex_token_iterator end;
    
    for (; iter != end; ++iter) {
        std::string side = *iter;
        if (!side.empty()) {
            sides.push_back(side);
        }
    }
    
    if (sides.size() != 2) {
        throw std::invalid_argument("Invalid equation format - must have reactants -> products");
    }
    
    // Parse reactants
    auto reactants = splitCompounds(sides[0]);
    for (const std::string& reactantStr : reactants) {
        auto [formula, coeff] = parseCompoundWithCoefficient(reactantStr);
        ChemicalCompound compound(formula);
        if (!compound.isValid()) {
            throw std::invalid_argument("Invalid reactant: " + formula);
        }
        equation.addReactant(compound, coeff);
    }
    
    // Parse products
    auto products = splitCompounds(sides[1]);
    for (const std::string& productStr : products) {
        auto [formula, coeff] = parseCompoundWithCoefficient(productStr);
        ChemicalCompound compound(formula);
        if (!compound.isValid()) {
            throw std::invalid_argument("Invalid product: " + formula);
        }
        equation.addProduct(compound, coeff);
    }
    
    return equation;
}

std::vector<std::string> EquationBalancer::splitCompounds(const std::string& side) {
    std::vector<std::string> compounds;
    
    std::regex plusRegex(R"(\s*\+\s*)");
    std::sregex_token_iterator iter(side.begin(), side.end(), plusRegex, -1);
    std::sregex_token_iterator end;
    
    for (; iter != end; ++iter) {
        std::string compound = *iter;
        if (!compound.empty()) {
            compounds.push_back(compound);
        }
    }
    
    return compounds;
}

std::pair<std::string, int> EquationBalancer::parseCompoundWithCoefficient(const std::string& compoundStr) {
    std::regex coeffRegex(R"(^\s*(\d+)?\s*([A-Za-z0-9()]+)\s*$)");
    std::smatch match;
    
    if (std::regex_match(compoundStr, match, coeffRegex)) {
        int coefficient = match[1].str().empty() ? 1 : std::stoi(match[1].str());
        std::string formula = match[2].str();
        return {formula, coefficient};
    }
    
    throw std::invalid_argument("Invalid compound format: " + compoundStr);
}