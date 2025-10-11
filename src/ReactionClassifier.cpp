#include "ReactionClassifier.h"
#include <algorithm>
#include <sstream>

bool ReactionClassifier::containsElement(const ChemicalCompound& compound, const std::string& element) {
    auto elements = compound.getElementCount();
    return elements.find(element) != elements.end();
}

bool ReactionClassifier::isOxygen(const ChemicalCompound& compound) {
    return compound.getFormula() == "O2";
}

bool ReactionClassifier::isCO2(const ChemicalCompound& compound) {
    return compound.getFormula() == "CO2";
}

bool ReactionClassifier::isH2O(const ChemicalCompound& compound) {
    return compound.getFormula() == "H2O";
}

bool ReactionClassifier::isAcid(const ChemicalCompound& compound) {
    std::string formula = compound.getFormula();
    return formula.substr(0, 1) == "H" && 
           (formula.find("Cl") != std::string::npos ||
            formula.find("SO4") != std::string::npos ||
            formula.find("NO3") != std::string::npos ||
            formula.find("PO4") != std::string::npos);
}

bool ReactionClassifier::isBase(const ChemicalCompound& compound) {
    std::string formula = compound.getFormula();
    return formula.find("OH") != std::string::npos ||
           formula == "NH3" ||
           formula.find("CO3") != std::string::npos;
}

bool ReactionClassifier::isSalt(const ChemicalCompound& compound) {
    auto elements = compound.getElementCount();
    bool hasMetal = false;
    bool hasNonMetal = false;
    
    // Simple heuristic: contains a metal and a non-metal (but not OH or H)
    std::vector<std::string> metals = {"Na", "K", "Ca", "Mg", "Al", "Fe", "Cu", "Zn", "Pb", "Ag"};
    std::vector<std::string> nonMetals = {"Cl", "Br", "I", "S", "P", "N"};
    
    for (const auto& element : elements) {
        if (std::find(metals.begin(), metals.end(), element.first) != metals.end()) {
            hasMetal = true;
        }
        if (std::find(nonMetals.begin(), nonMetals.end(), element.first) != nonMetals.end()) {
            hasNonMetal = true;
        }
    }
    
    return hasMetal && hasNonMetal && !isAcid(compound) && !isBase(compound);
}

bool ReactionClassifier::isHydrocarbon(const ChemicalCompound& compound) {
    auto elements = compound.getElementCount();
    return elements.size() <= 2 && 
           containsElement(compound, "C") && 
           containsElement(compound, "H");
}

bool ReactionClassifier::isGas(const ChemicalCompound& compound) {
    std::string formula = compound.getFormula();
    std::vector<std::string> commonGases = {
        "H2", "O2", "N2", "CO2", "CO", "NH3", "HCl", "SO2", "NO", "NO2", "H2S", "CH4"
    };
    
    return std::find(commonGases.begin(), commonGases.end(), formula) != commonGases.end();
}

bool ReactionClassifier::hasMetalAndNonMetal(const ChemicalCompound& compound) {
    auto elements = compound.getElementCount();
    bool hasMetal = false;
    bool hasNonMetal = false;
    
    std::vector<std::string> metals = {"Na", "K", "Ca", "Mg", "Al", "Fe", "Cu", "Zn", "Pb", "Ag"};
    
    for (const auto& element : elements) {
        if (std::find(metals.begin(), metals.end(), element.first) != metals.end()) {
            hasMetal = true;
        } else {
            hasNonMetal = true;
        }
    }
    
    return hasMetal && hasNonMetal;
}

bool ReactionClassifier::isCombustionPattern(const ChemicalEquation& equation) {
    auto reactants = equation.getReactants();
    auto products = equation.getProducts();
    
    // Look for hydrocarbon + O2 → CO2 + H2O pattern
    bool hasHydrocarbon = false;
    bool hasOxygen = false;
    bool hasCO2 = false;
    bool hasH2O = false;
    
    for (const auto& reactant : reactants) {
        if (isHydrocarbon(reactant.first)) hasHydrocarbon = true;
        if (isOxygen(reactant.first)) hasOxygen = true;
    }
    
    for (const auto& product : products) {
        if (isCO2(product.first)) hasCO2 = true;
        if (isH2O(product.first)) hasH2O = true;
    }
    
    return hasHydrocarbon && hasOxygen && hasCO2 && hasH2O;
}

bool ReactionClassifier::isSynthesisPattern(const ChemicalEquation& equation) {
    return equation.getReactants().size() >= 2 && equation.getProducts().size() == 1;
}

bool ReactionClassifier::isDecompositionPattern(const ChemicalEquation& equation) {
    return equation.getReactants().size() == 1 && equation.getProducts().size() >= 2;
}

bool ReactionClassifier::isSingleReplacementPattern(const ChemicalEquation& equation) {
    return equation.getReactants().size() == 2 && equation.getProducts().size() == 2;
}

bool ReactionClassifier::isDoubleReplacementPattern(const ChemicalEquation& equation) {
    if (equation.getReactants().size() != 2 || equation.getProducts().size() != 2) {
        return false;
    }
    
    // Check if all compounds are ionic (have metal and non-metal)
    bool allIonic = true;
    for (const auto& reactant : equation.getReactants()) {
        if (!hasMetalAndNonMetal(reactant.first)) {
            allIonic = false;
            break;
        }
    }
    
    if (allIonic) {
        for (const auto& product : equation.getProducts()) {
            if (!hasMetalAndNonMetal(product.first) && !isH2O(product.first)) {
                allIonic = false;
                break;
            }
        }
    }
    
    return allIonic;
}

bool ReactionClassifier::isAcidBasePattern(const ChemicalEquation& equation) {
    auto reactants = equation.getReactants();
    auto products = equation.getProducts();
    
    if (reactants.size() != 2 || products.size() != 2) {
        return false;
    }
    
    bool hasAcid = false;
    bool hasBase = false;
    bool hasSalt = false;
    bool hasWater = false;
    
    for (const auto& reactant : reactants) {
        if (isAcid(reactant.first)) hasAcid = true;
        if (isBase(reactant.first)) hasBase = true;
    }
    
    for (const auto& product : products) {
        if (isSalt(product.first)) hasSalt = true;
        if (isH2O(product.first)) hasWater = true;
    }
    
    return hasAcid && hasBase && hasSalt && hasWater;
}

bool ReactionClassifier::isPrecipitationPattern(const ChemicalEquation& equation) {
    // Look for insoluble salt formation
    auto products = equation.getProducts();
    
    for (const auto& product : products) {
        if (isSalt(product.first)) {
            // Simple check for common insoluble salts
            std::string formula = product.first.getFormula();
            if (formula.find("AgCl") != std::string::npos ||
                formula.find("BaSO4") != std::string::npos ||
                formula.find("PbI2") != std::string::npos) {
                return true;
            }
        }
    }
    
    return false;
}

bool ReactionClassifier::isGasFormingPattern(const ChemicalEquation& equation) {
    auto products = equation.getProducts();
    
    for (const auto& product : products) {
        if (isGas(product.first)) {
            return true;
        }
    }
    
    return false;
}

ReactionType ReactionClassifier::classify(const ChemicalEquation& equation) {
    // Check in order of specificity
    
    if (isCombustionPattern(equation)) {
        return ReactionType::COMBUSTION;
    }
    
    if (isAcidBasePattern(equation)) {
        return ReactionType::ACID_BASE;
    }
    
    if (isPrecipitationPattern(equation)) {
        return ReactionType::PRECIPITATION;
    }
    
    if (isGasFormingPattern(equation)) {
        return ReactionType::GAS_FORMING;
    }
    
    if (isSynthesisPattern(equation)) {
        return ReactionType::SYNTHESIS;
    }
    
    if (isDecompositionPattern(equation)) {
        return ReactionType::DECOMPOSITION;
    }
    
    if (isDoubleReplacementPattern(equation)) {
        return ReactionType::DOUBLE_REPLACEMENT;
    }
    
    if (isSingleReplacementPattern(equation)) {
        return ReactionType::SINGLE_REPLACEMENT;
    }
    
    return ReactionType::UNKNOWN;
}

ReactionInfo ReactionClassifier::getReactionInfo(ReactionType type) {
    ReactionInfo info;
    info.type = type;
    
    switch (type) {
        case ReactionType::SYNTHESIS:
            info.name = "Synthesis (Combination)";
            info.description = "Two or more reactants combine to form a single product";
            info.generalForm = "A + B → AB";
            info.characteristics = {"Multiple reactants", "Single product", "Energy usually released"};
            info.examples = {"2H2 + O2 → 2H2O", "N2 + 3H2 → 2NH3"};
            break;
            
        case ReactionType::DECOMPOSITION:
            info.name = "Decomposition";
            info.description = "A single reactant breaks down into two or more products";
            info.generalForm = "AB → A + B";
            info.characteristics = {"Single reactant", "Multiple products", "Energy usually required"};
            info.examples = {"2H2O → 2H2 + O2", "CaCO3 → CaO + CO2"};
            break;
            
        case ReactionType::SINGLE_REPLACEMENT:
            info.name = "Single Replacement";
            info.description = "One element replaces another in a compound";
            info.generalForm = "A + BC → AC + B";
            info.characteristics = {"Element + compound", "New compound + element"};
            info.examples = {"Zn + 2HCl → ZnCl2 + H2", "Cu + 2AgNO3 → Cu(NO3)2 + 2Ag"};
            break;
            
        case ReactionType::DOUBLE_REPLACEMENT:
            info.name = "Double Replacement";
            info.description = "Two compounds exchange ions";
            info.generalForm = "AB + CD → AD + CB";
            info.characteristics = {"Two ionic compounds", "Ion exchange", "Often forms precipitate or gas"};
            info.examples = {"AgNO3 + NaCl → AgCl + NaNO3", "BaCl2 + Na2SO4 → BaSO4 + 2NaCl"};
            break;
            
        case ReactionType::COMBUSTION:
            info.name = "Combustion";
            info.description = "A substance combines with oxygen, usually producing CO2 and H2O";
            info.generalForm = "CxHy + O2 → CO2 + H2O";
            info.characteristics = {"Reaction with oxygen", "Produces CO2 and H2O", "Releases energy (exothermic)"};
            info.examples = {"CH4 + 2O2 → CO2 + 2H2O", "C6H12O6 + 6O2 → 6CO2 + 6H2O"};
            break;
            
        case ReactionType::ACID_BASE:
            info.name = "Acid-Base Neutralization";
            info.description = "An acid reacts with a base to produce a salt and water";
            info.generalForm = "Acid + Base → Salt + H2O";
            info.characteristics = {"Acid and base reactants", "Forms salt and water", "Neutralization"};
            info.examples = {"HCl + NaOH → NaCl + H2O", "H2SO4 + 2KOH → K2SO4 + 2H2O"};
            break;
            
        case ReactionType::PRECIPITATION:
            info.name = "Precipitation";
            info.description = "Formation of an insoluble solid from soluble reactants";
            info.generalForm = "AB(aq) + CD(aq) → AD(s) + CB(aq)";
            info.characteristics = {"Insoluble product formed", "Solid precipitates from solution"};
            info.examples = {"AgNO3 + NaCl → AgCl(s) + NaNO3", "Ba(NO3)2 + Na2SO4 → BaSO4(s) + 2NaNO3"};
            break;
            
        case ReactionType::GAS_FORMING:
            info.name = "Gas-Forming";
            info.description = "Reaction produces a gaseous product";
            info.generalForm = "Reactants → Products + Gas";
            info.characteristics = {"Gas evolution", "Often accompanied by bubbling"};
            info.examples = {"Zn + 2HCl → ZnCl2 + H2(g)", "CaCO3 + 2HCl → CaCl2 + CO2(g) + H2O"};
            break;
            
        case ReactionType::REDOX:
            info.name = "Oxidation-Reduction";
            info.description = "Electron transfer between species";
            info.generalForm = "Oxidant + Reductant → Products";
            info.characteristics = {"Electron transfer", "Change in oxidation states"};
            info.examples = {"2Na + Cl2 → 2NaCl", "Fe2O3 + 3CO → 2Fe + 3CO2"};
            break;
            
        default:
            info.name = "Unknown";
            info.description = "Reaction type could not be determined";
            info.generalForm = "Unknown";
            info.characteristics = {"Classification unclear"};
            info.examples = {};
            break;
    }
    
    return info;
}

std::string ReactionClassifier::getReactionName(ReactionType type) {
    return getReactionInfo(type).name;
}

std::string ReactionClassifier::getReactionDescription(ReactionType type) {
    return getReactionInfo(type).description;
}

std::vector<ReactionType> ReactionClassifier::getAllPossibleTypes(const ChemicalEquation& equation) {
    std::vector<ReactionType> types;
    
    if (isCombustionPattern(equation)) types.push_back(ReactionType::COMBUSTION);
    if (isAcidBasePattern(equation)) types.push_back(ReactionType::ACID_BASE);
    if (isPrecipitationPattern(equation)) types.push_back(ReactionType::PRECIPITATION);
    if (isGasFormingPattern(equation)) types.push_back(ReactionType::GAS_FORMING);
    if (isSynthesisPattern(equation)) types.push_back(ReactionType::SYNTHESIS);
    if (isDecompositionPattern(equation)) types.push_back(ReactionType::DECOMPOSITION);
    if (isDoubleReplacementPattern(equation)) types.push_back(ReactionType::DOUBLE_REPLACEMENT);
    if (isSingleReplacementPattern(equation)) types.push_back(ReactionType::SINGLE_REPLACEMENT);
    
    if (types.empty()) {
        types.push_back(ReactionType::UNKNOWN);
    }
    
    return types;
}

bool ReactionClassifier::isCombustion(const ChemicalEquation& equation) {
    return isCombustionPattern(equation);
}

bool ReactionClassifier::isAcidBase(const ChemicalEquation& equation) {
    return isAcidBasePattern(equation);
}

bool ReactionClassifier::isRedox(const ChemicalEquation& equation) {
    // Simplified redox detection - would need oxidation state analysis for full implementation
    auto reactants = equation.getReactants();
    auto products = equation.getProducts();
    
    // Check for common redox patterns
    for (const auto& reactant : reactants) {
        if (containsElement(reactant.first, "O") && reactant.first.getFormula() == "O2") {
            return true; // Likely oxidation
        }
    }
    
    return false;
}

bool ReactionClassifier::isPrecipitation(const ChemicalEquation& equation) {
    return isPrecipitationPattern(equation);
}

std::string ReactionClassifier::analyzeReaction(const ChemicalEquation& equation) {
    std::stringstream analysis;
    
    ReactionType primary = classify(equation);
    auto allTypes = getAllPossibleTypes(equation);
    
    analysis << "Primary classification: " << getReactionName(primary) << "\n";
    analysis << getReactionDescription(primary) << "\n\n";
    
    if (allTypes.size() > 1) {
        analysis << "Additional classifications:\n";
        for (auto type : allTypes) {
            if (type != primary) {
                analysis << "- " << getReactionName(type) << "\n";
            }
        }
        analysis << "\n";
    }
    
    auto characteristics = getReactionCharacteristics(equation);
    if (!characteristics.empty()) {
        analysis << "Key characteristics:\n";
        for (const auto& characteristic : characteristics) {
            analysis << "- " << characteristic << "\n";
        }
    }
    
    return analysis.str();
}

std::vector<std::string> ReactionClassifier::getReactionCharacteristics(const ChemicalEquation& equation) {
    std::vector<std::string> characteristics;
    
    auto reactants = equation.getReactants();
    auto products = equation.getProducts();
    
    characteristics.push_back(std::to_string(reactants.size()) + " reactant(s), " + 
                            std::to_string(products.size()) + " product(s)");
    
    // Check for gas formation
    for (const auto& product : products) {
        if (isGas(product.first)) {
            characteristics.push_back("Gas formation: " + product.first.getFormula());
            break;
        }
    }
    
    // Check for water formation
    for (const auto& product : products) {
        if (isH2O(product.first)) {
            characteristics.push_back("Water formation");
            break;
        }
    }
    
    // Check for energy considerations
    if (isCombustionPattern(equation)) {
        characteristics.push_back("Exothermic (releases energy)");
    }
    
    if (isDecompositionPattern(equation)) {
        characteristics.push_back("Usually endothermic (requires energy input)");
    }
    
    return characteristics;
}