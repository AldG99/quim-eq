#include <iostream>
#include <string>
#include <vector>
#include "ChemicalCompound.h"
#include "EquationBalancer.h"
#include "StoichiometryCalculator.h"
#include "ReactionClassifier.h"
#include "CompoundDatabase.h"

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [equation] or [options]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  help        Show this help message\n";
    std::cout << "  test        Run built-in tests\n";
    std::cout << "  examples    Show example equations\n";
    std::cout << "  interactive Start interactive mode\n";
    std::cout << "\nEquation format: \"Reactants -> Products\"\n";
    std::cout << "Example: " << programName << " \"H2 + O2 -> H2O\"\n";
    std::cout << "Example: " << programName << " \"CH4 + O2 -> CO2 + H2O\"\n";
}

void printExamples() {
    std::cout << "Example Chemical Equations:\n\n";
    
    std::vector<std::string> examples = {
        "H2 + O2 -> H2O",
        "CH4 + O2 -> CO2 + H2O",
        "C6H12O6 + O2 -> CO2 + H2O",
        "Fe + O2 -> Fe2O3",
        "NH3 + O2 -> NO + H2O",
        "C2H6 + O2 -> CO2 + H2O",
        "Al + HCl -> AlCl3 + H2",
        "CaCO3 + HCl -> CaCl2 + CO2 + H2O",
        "Na + H2O -> NaOH + H2",
        "Mg + N2 -> Mg3N2"
    };
    
    for (size_t i = 0; i < examples.size(); ++i) {
        std::cout << (i + 1) << ". " << examples[i] << "\n";
    }
    
    std::cout << "\nTry: ./chemical_balancer \"" << examples[0] << "\"\n";
}

void runTests() {
    std::cout << "Running built-in tests...\n\n";
    
    std::vector<std::string> testEquations = {
        "H2 + O2 -> H2O",
        "CH4 + O2 -> CO2 + H2O",
        "C6H12O6 + O2 -> CO2 + H2O",
        "Fe + O2 -> Fe2O3"
    };
    
    EquationBalancer balancer;
    ReactionClassifier classifier;
    int passed = 0;
    
    for (const auto& eq : testEquations) {
        std::cout << "Testing: " << eq << "\n";
        try {
            auto equation = EquationBalancer::parseEquationString(eq);
            auto result = balancer.balance(equation);
            
            if (result.result == BalanceResult::SUCCESS || result.result == BalanceResult::ALREADY_BALANCED) {
                std::cout << "✅ Result: " << equation.toDisplayString() << "\n";
                
                auto type = classifier.classify(equation);
                std::cout << "   Type: " << classifier.getReactionName(type) << "\n";
                passed++;
            } else {
                std::cout << "❌ Failed: " << result.message << "\n";
            }
        } catch (const std::exception& e) {
            std::cout << "❌ Error: " << e.what() << "\n";
        }
        std::cout << "\n";
    }
    
    std::cout << "Tests completed: " << passed << "/" << testEquations.size() << " passed\n";
}

void interactiveMode() {
    std::cout << "=== Chemical Equation Balancer - Interactive Mode ===\n";
    std::cout << "Enter chemical equations to balance (type 'quit' to exit)\n";
    std::cout << "Format: Reactants -> Products (e.g., H2 + O2 -> H2O)\n\n";
    
    EquationBalancer balancer;
    StoichiometryCalculator calculator;
    ReactionClassifier classifier;
    std::string input;
    
    while (true) {
        std::cout << "Equation> ";
        std::getline(std::cin, input);
        
        if (input == "quit" || input == "exit" || input == "q") {
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        if (input == "help") {
            std::cout << "Commands:\n";
            std::cout << "  help      - Show this help\n";
            std::cout << "  examples  - Show example equations\n";
            std::cout << "  quit      - Exit program\n";
            std::cout << "Or enter a chemical equation to balance\n\n";
            continue;
        }
        
        if (input == "examples") {
            printExamples();
            std::cout << "\n";
            continue;
        }
        
        try {
            auto equation = EquationBalancer::parseEquationString(input);
            std::cout << "\nOriginal: " << equation.toString() << "\n";
            
            auto result = balancer.balance(equation);
            
            if (result.result == BalanceResult::SUCCESS || result.result == BalanceResult::ALREADY_BALANCED) {
                std::cout << "Balanced: " << equation.toDisplayString() << "\n";
                
                // Show coefficients
                std::cout << "Coefficients: ";
                for (size_t i = 0; i < result.coefficients.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << result.coefficients[i];
                }
                std::cout << "\n";
                
                // Show reaction type
                auto type = classifier.classify(equation);
                std::cout << "Type: " << classifier.getReactionName(type) << "\n";
                std::cout << "Description: " << classifier.getReactionDescription(type) << "\n";
                
                // Show molar ratios
                auto ratios = calculator.calculateMolarRatios(equation);
                std::cout << "Molar ratios: " << ratios.description << "\n";
                
            } else {
                std::cout << "❌ Balancing failed: " << result.message << "\n";
            }
            
        } catch (const std::exception& e) {
            std::cout << "❌ Error: " << e.what() << "\n";
        }
        
        std::cout << "\n";
    }
    
    std::cout << "Goodbye!\n";
}

void processEquation(const std::string& equationStr) {
    try {
        EquationBalancer balancer;
        StoichiometryCalculator calculator;
        ReactionClassifier classifier;
        
        std::cout << "=== Chemical Equation Balancer ===\n\n";
        
        // Parse equation
        auto equation = EquationBalancer::parseEquationString(equationStr);
        std::cout << "Original equation: " << equation.toString() << "\n\n";
        
        // Balance equation
        std::cout << "Balancing...\n";
        auto result = balancer.balance(equation);
        
        if (result.result == BalanceResult::SUCCESS || result.result == BalanceResult::ALREADY_BALANCED) {
            std::cout << "✅ SUCCESS!\n\n";
            
            // Show balanced equation
            std::cout << "Balanced equation: " << equation.toDisplayString() << "\n\n";
            
            // Show coefficients
            std::cout << "Coefficients: ";
            for (size_t i = 0; i < result.coefficients.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << result.coefficients[i];
            }
            std::cout << "\n\n";
            
            // Show conservation check
            std::cout << "Atom conservation check:\n";
            for (const auto& atom : result.atomBalance) {
                std::cout << "• " << atom.first << ": " << 
                    (atom.second == 0 ? "✅ Balanced" : "❌ Not balanced") << 
                    " (difference: " << atom.second << ")\n";
            }
            std::cout << "\n";
            
            // Show reaction classification
            auto type = classifier.classify(equation);
            auto info = classifier.getReactionInfo(type);
            std::cout << "Reaction type: " << info.name << "\n";
            std::cout << "Description: " << info.description << "\n";
            std::cout << "General form: " << info.generalForm << "\n\n";
            
            // Show molar relationships
            auto ratios = calculator.calculateMolarRatios(equation);
            std::cout << "Molar ratios: " << ratios.description << "\n\n";
            
            // Show balancing steps
            auto steps = balancer.getBalancingSteps();
            if (!steps.empty()) {
                std::cout << "Balancing steps:\n";
                for (size_t i = 0; i < steps.size(); ++i) {
                    std::cout << (i + 1) << ". " << steps[i] << "\n";
                }
            }
            
        } else {
            std::cout << "❌ FAILED!\n";
            std::cout << "Error: " << result.message << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ ERROR: " << e.what() << "\n";
    }
}

int main(int argc, char* argv[]) {
    // Initialize database
    CompoundDatabase::getInstance();
    
    if (argc == 1) {
        // No arguments - start interactive mode
        interactiveMode();
        return 0;
    }
    
    std::string arg = argv[1];
    
    if (arg == "help" || arg == "--help" || arg == "-h") {
        printUsage(argv[0]);
        return 0;
    }
    
    if (arg == "examples" || arg == "--examples") {
        printExamples();
        return 0;
    }
    
    if (arg == "test" || arg == "--test") {
        runTests();
        return 0;
    }
    
    if (arg == "interactive" || arg == "-i") {
        interactiveMode();
        return 0;
    }
    
    // Treat as equation string
    processEquation(arg);
    return 0;
}