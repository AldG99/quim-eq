#ifndef EQUATION_BALANCER_H
#define EQUATION_BALANCER_H

#include "ChemicalCompound.h"
#include "MatrixSolver.h"
#include <vector>
#include <string>
#include <map>

enum class BalanceResult {
    SUCCESS,
    ALREADY_BALANCED,
    NO_SOLUTION,
    INFINITE_SOLUTIONS,
    INVALID_EQUATION,
    PARSING_ERROR
};

struct BalanceInfo {
    BalanceResult result;
    std::vector<int> coefficients;
    std::string message;
    std::map<std::string, int> atomBalance;
    bool conservationVerified;
};

class EquationBalancer {
private:
    MatrixSolver solver_;
    std::vector<std::string> balancingSteps_;
    
    std::vector<std::vector<double>> buildStoichiometricMatrix(const ChemicalEquation& equation);
    void addBalancingStep(const std::string& step);
    std::string formatMatrix(const std::vector<std::vector<double>>& matrix, const std::vector<std::string>& elements, const ChemicalEquation& equation);
    
public:
    BalanceInfo balance(ChemicalEquation& equation);
    std::vector<std::string> getBalancingSteps() const;
    std::vector<SolutionStep> getMathematicalSteps() const;
    
    void clearSteps();
    
    // Validation methods
    bool validateAtomConservation(const ChemicalEquation& equation);
    std::map<std::string, int> getAtomBalance(const ChemicalEquation& equation);
    
    // Static helper methods
    static ChemicalEquation parseEquationString(const std::string& equationStr);
    static std::vector<std::string> splitCompounds(const std::string& side);
    static std::pair<std::string, int> parseCompoundWithCoefficient(const std::string& compoundStr);
};

#endif // EQUATION_BALANCER_H