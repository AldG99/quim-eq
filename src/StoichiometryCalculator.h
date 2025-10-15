#ifndef STOICHIOMETRY_CALCULATOR_H
#define STOICHIOMETRY_CALCULATOR_H

#include "ChemicalCompound.h"
#include <map>
#include <string>
#include <vector>

struct MolarRatio {
    std::map<std::string, int> ratios; // compound formula -> molar ratio
    std::string description;
};

struct LimitingReagentResult {
    std::string limitingReagent;
    double limitingAmount;
    std::map<std::string, double> excessAmounts;
    std::map<std::string, double> theoreticalYields;
    std::string explanation;
};

struct StoichiometricRelation {
    std::string compound1;
    std::string compound2;
    double ratio; // moles of compound2 per mole of compound1
    double massRatio; // grams of compound2 per gram of compound1
};

class StoichiometryCalculator {
private:
    
public:
    // Basic calculations
    double calculateMolarMass(const ChemicalCompound& compound);
    double molesToGrams(double moles, const ChemicalCompound& compound);
    double gramsToMoles(double grams, const ChemicalCompound& compound);
    double molesToMolecules(double moles);
    double moleculesToMoles(double molecules);
    
    // Equation-based calculations
    MolarRatio calculateMolarRatios(const ChemicalEquation& equation);
    std::vector<StoichiometricRelation> calculateAllRelations(const ChemicalEquation& equation);
    
    // Limiting reagent analysis
    LimitingReagentResult findLimitingReagent(
        const ChemicalEquation& equation,
        const std::map<std::string, double>& availableMasses);
    
    LimitingReagentResult findLimitingReagentFromMoles(
        const ChemicalEquation& equation,
        const std::map<std::string, double>& availableMoles);
    
    // Theoretical yield calculations
    double calculateTheoreticalYield(
        const ChemicalEquation& equation,
        const std::string& product,
        const std::string& limitingReagent,
        double limitingReagentAmount);
    
    std::map<std::string, double> calculateAllYields(
        const ChemicalEquation& equation,
        const std::string& limitingReagent,
        double limitingReagentAmount);
    
    // Percent composition
    std::map<std::string, double> calculatePercentComposition(const ChemicalCompound& compound);
    
    // Unit conversions
    double gramsToKilograms(double grams);
    double kilogramsToGrams(double kilograms);
    double molesToLitersAtSTP(double moles); // For gases at STP
    double litersToMolesAtSTP(double liters); // For gases at STP
    
    // Constants
    static constexpr double AVOGADRO_NUMBER = 6.02214076e23;
    static constexpr double MOLAR_VOLUME_STP = 22.414; // L/mol at STP
    
    // Helper methods for display
    std::string formatMolarRatio(const MolarRatio& ratio);
    std::string formatLimitingReagentResult(const LimitingReagentResult& result);
    
    // Helper method for atomic masses
    double getAtomicMass(const std::string& element);
};

#endif // STOICHIOMETRY_CALCULATOR_H