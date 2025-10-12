#include "StoichiometryCalculator.h"
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <map>

double StoichiometryCalculator::calculateMolarMass(const ChemicalCompound& compound) {
    return compound.getMolarMass();
}

double StoichiometryCalculator::molesToGrams(double moles, const ChemicalCompound& compound) {
    return moles * compound.getMolarMass();
}

double StoichiometryCalculator::gramsToMoles(double grams, const ChemicalCompound& compound) {
    if (compound.getMolarMass() == 0.0) return 0.0;
    return grams / compound.getMolarMass();
}

double StoichiometryCalculator::molesToMolecules(double moles) {
    return moles * AVOGADRO_NUMBER;
}

double StoichiometryCalculator::moleculesToMoles(double molecules) {
    return molecules / AVOGADRO_NUMBER;
}

MolarRatio StoichiometryCalculator::calculateMolarRatios(const ChemicalEquation& equation) {
    MolarRatio ratio;
    
    // Add reactants
    for (const auto& reactant : equation.getReactants()) {
        ratio.ratios[reactant.first.getFormula()] = reactant.second;
    }
    
    // Add products
    for (const auto& product : equation.getProducts()) {
        ratio.ratios[product.first.getFormula()] = product.second;
    }
    
    // Create description
    std::stringstream ss;
    bool first = true;
    for (const auto& compound : ratio.ratios) {
        if (!first) ss << " : ";
        ss << compound.second << " mol " << compound.first;
        first = false;
    }
    ratio.description = ss.str();
    
    return ratio;
}

std::vector<StoichiometricRelation> StoichiometryCalculator::calculateAllRelations(const ChemicalEquation& equation) {
    std::vector<StoichiometricRelation> relations;
    
    std::vector<std::pair<ChemicalCompound, int>> allCompounds;
    
    // Collect all compounds
    for (const auto& reactant : equation.getReactants()) {
        allCompounds.push_back(reactant);
    }
    for (const auto& product : equation.getProducts()) {
        allCompounds.push_back(product);
    }
    
    // Generate all pairwise relations
    for (size_t i = 0; i < allCompounds.size(); ++i) {
        for (size_t j = i + 1; j < allCompounds.size(); ++j) {
            const auto& comp1 = allCompounds[i];
            const auto& comp2 = allCompounds[j];
            
            StoichiometricRelation relation;
            relation.compound1 = comp1.first.getFormula();
            relation.compound2 = comp2.first.getFormula();
            relation.ratio = static_cast<double>(comp2.second) / comp1.second;
            
            double mass1 = comp1.first.getMolarMass();
            double mass2 = comp2.first.getMolarMass();
            relation.massRatio = (mass2 * comp2.second) / (mass1 * comp1.second);
            
            relations.push_back(relation);
        }
    }
    
    return relations;
}

LimitingReagentResult StoichiometryCalculator::findLimitingReagent(
    const ChemicalEquation& equation,
    const std::map<std::string, double>& availableMasses) {
    
    // Convert masses to moles
    std::map<std::string, double> availableMoles;
    
    for (const auto& reactant : equation.getReactants()) {
        std::string formula = reactant.first.getFormula();
        auto it = availableMasses.find(formula);
        if (it != availableMasses.end()) {
            availableMoles[formula] = gramsToMoles(it->second, reactant.first);
        }
    }
    
    return findLimitingReagentFromMoles(equation, availableMoles);
}

LimitingReagentResult StoichiometryCalculator::findLimitingReagentFromMoles(
    const ChemicalEquation& equation,
    const std::map<std::string, double>& availableMoles) {
    
    LimitingReagentResult result;
    
    double minRatio = std::numeric_limits<double>::max();
    std::string limitingFormula;
    
    // Find limiting reagent
    for (const auto& reactant : equation.getReactants()) {
        std::string formula = reactant.first.getFormula();
        auto it = availableMoles.find(formula);
        
        if (it != availableMoles.end()) {
            double ratio = it->second / reactant.second; // available moles / required stoichiometric coefficient
            if (ratio < minRatio) {
                minRatio = ratio;
                limitingFormula = formula;
            }
        }
    }
    
    result.limitingReagent = limitingFormula;
    result.limitingAmount = minRatio;
    
    // Calculate excess amounts
    for (const auto& reactant : equation.getReactants()) {
        std::string formula = reactant.first.getFormula();
        auto it = availableMoles.find(formula);
        
        if (it != availableMoles.end() && formula != limitingFormula) {
            double consumed = minRatio * reactant.second;
            double excess = it->second - consumed;
            result.excessAmounts[formula] = excess;
        }
    }
    
    // Calculate theoretical yields
    result.theoreticalYields = calculateAllYields(equation, limitingFormula, minRatio);
    
    // Create explanation
    std::stringstream ss;
    ss << "Limiting reagent: " << limitingFormula << " (" 
       << std::fixed << std::setprecision(3) << minRatio << " mol available)\n";
    ss << "This limits the reaction to " << minRatio << " mol of reaction extent.";
    result.explanation = ss.str();
    
    return result;
}

double StoichiometryCalculator::calculateTheoreticalYield(
    const ChemicalEquation& equation,
    const std::string& product,
    const std::string& limitingReagent,
    double limitingReagentAmount) {
    
    // Find coefficients
    int limitingCoeff = 0;
    int productCoeff = 0;
    ChemicalCompound productCompound("");
    
    for (const auto& reactant : equation.getReactants()) {
        if (reactant.first.getFormula() == limitingReagent) {
            limitingCoeff = reactant.second;
            break;
        }
    }
    
    for (const auto& prod : equation.getProducts()) {
        if (prod.first.getFormula() == product) {
            productCoeff = prod.second;
            productCompound = prod.first;
            break;
        }
    }
    
    if (limitingCoeff == 0 || productCoeff == 0) {
        return 0.0;
    }
    
    // Calculate moles of product
    double productMoles = limitingReagentAmount * productCoeff / limitingCoeff;
    
    // Convert to grams
    return molesToGrams(productMoles, productCompound);
}

std::map<std::string, double> StoichiometryCalculator::calculateAllYields(
    const ChemicalEquation& equation,
    const std::string& limitingReagent,
    double limitingReagentAmount) {
    
    std::map<std::string, double> yields;
    
    for (const auto& product : equation.getProducts()) {
        std::string formula = product.first.getFormula();
        double yield = calculateTheoreticalYield(equation, formula, limitingReagent, limitingReagentAmount);
        yields[formula] = yield;
    }
    
    return yields;
}

std::map<std::string, double> StoichiometryCalculator::calculatePercentComposition(const ChemicalCompound& compound) {
    std::map<std::string, double> composition;
    auto elements = compound.getElementCount();
    double totalMass = compound.getMolarMass();
    
    if (totalMass == 0.0) return composition;
    
    // Calculate percent composition for each element
    for (const auto& element : elements) {
        // Get atomic mass from database (simplified - using hardcoded values for now)
        double atomicMass = getAtomicMass(element.first);
        double elementMass = atomicMass * element.second;
        double percentage = (elementMass / totalMass) * 100.0;
        composition[element.first] = percentage;
    }
    
    return composition;
}

double StoichiometryCalculator::gramsToKilograms(double grams) {
    return grams / 1000.0;
}

double StoichiometryCalculator::kilogramsToGrams(double kilograms) {
    return kilograms * 1000.0;
}

double StoichiometryCalculator::molesToLitersAtSTP(double moles) {
    return moles * MOLAR_VOLUME_STP;
}

double StoichiometryCalculator::litersToMolesAtSTP(double liters) {
    return liters / MOLAR_VOLUME_STP;
}

std::string StoichiometryCalculator::formatMolarRatio(const MolarRatio& ratio) {
    return ratio.description;
}

std::string StoichiometryCalculator::formatLimitingReagentResult(const LimitingReagentResult& result) {
    std::stringstream ss;
    
    ss << "Limiting Reagent Analysis:\n";
    ss << "Limiting reagent: " << result.limitingReagent << "\n";
    ss << "Available amount: " << std::fixed << std::setprecision(3) << result.limitingAmount << " mol\n";
    
    if (!result.excessAmounts.empty()) {
        ss << "\nExcess reagents:\n";
        for (const auto& excess : result.excessAmounts) {
            ss << "  " << excess.first << ": " << excess.second << " mol excess\n";
        }
    }
    
    if (!result.theoreticalYields.empty()) {
        ss << "\nTheoretical yields:\n";
        for (const auto& yield : result.theoreticalYields) {
            ss << "  " << yield.first << ": " << yield.second << " g\n";
        }
    }
    
    return ss.str();
}

// Helper function to get atomic mass (simplified version)
double StoichiometryCalculator::getAtomicMass(const std::string& element) {
    // This is a simplified version - in the full implementation, 
    // this would use the CompoundDatabase
    static std::map<std::string, double> masses = {
        {"H", 1.008}, {"C", 12.011}, {"N", 14.007}, {"O", 15.999},
        {"Na", 22.990}, {"Mg", 24.305}, {"Al", 26.982}, {"Si", 28.086},
        {"P", 30.974}, {"S", 32.066}, {"Cl", 35.453}, {"K", 39.098},
        {"Ca", 40.078}, {"Fe", 55.845}, {"Cu", 63.546}, {"Zn", 65.38}
    };
    
    auto it = masses.find(element);
    return (it != masses.end()) ? it->second : 0.0;
}