#ifndef REACTION_CLASSIFIER_H
#define REACTION_CLASSIFIER_H

#include "ChemicalCompound.h"
#include <string>
#include <vector>

enum class ReactionType {
    SYNTHESIS,              // A + B → AB
    DECOMPOSITION,          // AB → A + B
    SINGLE_REPLACEMENT,     // A + BC → AC + B
    DOUBLE_REPLACEMENT,     // AB + CD → AD + CB
    COMBUSTION,             // CxHy + O2 → CO2 + H2O
    ACID_BASE,             // Acid + Base → Salt + H2O
    REDOX,                 // Involving electron transfer
    PRECIPITATION,         // Formation of insoluble product
    GAS_FORMING,          // Formation of gas product
    UNKNOWN
};

struct ReactionInfo {
    ReactionType type;
    std::string name;
    std::string description;
    std::string generalForm;
    std::vector<std::string> characteristics;
    std::vector<std::string> examples;
};

class ReactionClassifier {
private:
    bool containsElement(const ChemicalCompound& compound, const std::string& element);
    bool isOxygen(const ChemicalCompound& compound);
    bool isCO2(const ChemicalCompound& compound);
    bool isH2O(const ChemicalCompound& compound);
    bool isAcid(const ChemicalCompound& compound);
    bool isBase(const ChemicalCompound& compound);
    bool isSalt(const ChemicalCompound& compound);
    bool isHydrocarbon(const ChemicalCompound& compound);
    bool isGas(const ChemicalCompound& compound);
    bool hasMetalAndNonMetal(const ChemicalCompound& compound);
    
    bool isCombustionPattern(const ChemicalEquation& equation);
    bool isSynthesisPattern(const ChemicalEquation& equation);
    bool isDecompositionPattern(const ChemicalEquation& equation);
    bool isSingleReplacementPattern(const ChemicalEquation& equation);
    bool isDoubleReplacementPattern(const ChemicalEquation& equation);
    bool isAcidBasePattern(const ChemicalEquation& equation);
    bool isPrecipitationPattern(const ChemicalEquation& equation);
    bool isGasFormingPattern(const ChemicalEquation& equation);
    
public:
    ReactionType classify(const ChemicalEquation& equation);
    ReactionInfo getReactionInfo(ReactionType type);
    std::string getReactionName(ReactionType type);
    std::string getReactionDescription(ReactionType type);
    
    std::vector<ReactionType> getAllPossibleTypes(const ChemicalEquation& equation);
    
    // Specific type checkers (public for testing/analysis)
    bool isCombustion(const ChemicalEquation& equation);
    bool isAcidBase(const ChemicalEquation& equation);
    bool isRedox(const ChemicalEquation& equation);
    bool isPrecipitation(const ChemicalEquation& equation);
    
    // Analysis methods
    std::string analyzeReaction(const ChemicalEquation& equation);
    std::vector<std::string> getReactionCharacteristics(const ChemicalEquation& equation);
};

#endif // REACTION_CLASSIFIER_H