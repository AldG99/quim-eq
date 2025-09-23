#include "CompoundDatabase.h"
#include <iostream>

CompoundDatabase* CompoundDatabase::instance_ = nullptr;

CompoundDatabase::CompoundDatabase() {
    loadElements();
    loadCompounds();
}

CompoundDatabase& CompoundDatabase::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new CompoundDatabase();
    }
    return *instance_;
}

void CompoundDatabase::loadElements() {
    // Common elements with atomic masses
    elements_["H"] = {"Hydrogen", "H", 1.008, 1, "Nonmetal"};
    elements_["He"] = {"Helium", "He", 4.003, 2, "Noble gas"};
    elements_["Li"] = {"Lithium", "Li", 6.941, 3, "Alkali metal"};
    elements_["Be"] = {"Beryllium", "Be", 9.012, 4, "Alkaline earth metal"};
    elements_["B"] = {"Boron", "B", 10.811, 5, "Metalloid"};
    elements_["C"] = {"Carbon", "C", 12.011, 6, "Nonmetal"};
    elements_["N"] = {"Nitrogen", "N", 14.007, 7, "Nonmetal"};
    elements_["O"] = {"Oxygen", "O", 15.999, 8, "Nonmetal"};
    elements_["F"] = {"Fluorine", "F", 18.998, 9, "Halogen"};
    elements_["Ne"] = {"Neon", "Ne", 20.180, 10, "Noble gas"};
    elements_["Na"] = {"Sodium", "Na", 22.990, 11, "Alkali metal"};
    elements_["Mg"] = {"Magnesium", "Mg", 24.305, 12, "Alkaline earth metal"};
    elements_["Al"] = {"Aluminum", "Al", 26.982, 13, "Metal"};
    elements_["Si"] = {"Silicon", "Si", 28.086, 14, "Metalloid"};
    elements_["P"] = {"Phosphorus", "P", 30.974, 15, "Nonmetal"};
    elements_["S"] = {"Sulfur", "S", 32.066, 16, "Nonmetal"};
    elements_["Cl"] = {"Chlorine", "Cl", 35.453, 17, "Halogen"};
    elements_["Ar"] = {"Argon", "Ar", 39.948, 18, "Noble gas"};
    elements_["K"] = {"Potassium", "K", 39.098, 19, "Alkali metal"};
    elements_["Ca"] = {"Calcium", "Ca", 40.078, 20, "Alkaline earth metal"};
    elements_["Sc"] = {"Scandium", "Sc", 44.956, 21, "Transition metal"};
    elements_["Ti"] = {"Titanium", "Ti", 47.867, 22, "Transition metal"};
    elements_["V"] = {"Vanadium", "V", 50.942, 23, "Transition metal"};
    elements_["Cr"] = {"Chromium", "Cr", 51.996, 24, "Transition metal"};
    elements_["Mn"] = {"Manganese", "Mn", 54.938, 25, "Transition metal"};
    elements_["Fe"] = {"Iron", "Fe", 55.845, 26, "Transition metal"};
    elements_["Co"] = {"Cobalt", "Co", 58.933, 27, "Transition metal"};
    elements_["Ni"] = {"Nickel", "Ni", 58.693, 28, "Transition metal"};
    elements_["Cu"] = {"Copper", "Cu", 63.546, 29, "Transition metal"};
    elements_["Zn"] = {"Zinc", "Zn", 65.38, 30, "Transition metal"};
    elements_["Ga"] = {"Gallium", "Ga", 69.723, 31, "Metal"};
    elements_["Ge"] = {"Germanium", "Ge", 72.630, 32, "Metalloid"};
    elements_["As"] = {"Arsenic", "As", 74.922, 33, "Metalloid"};
    elements_["Se"] = {"Selenium", "Se", 78.971, 34, "Nonmetal"};
    elements_["Br"] = {"Bromine", "Br", 79.904, 35, "Halogen"};
    elements_["Kr"] = {"Krypton", "Kr", 83.798, 36, "Noble gas"};
    elements_["Rb"] = {"Rubidium", "Rb", 85.468, 37, "Alkali metal"};
    elements_["Sr"] = {"Strontium", "Sr", 87.62, 38, "Alkaline earth metal"};
    elements_["Y"] = {"Yttrium", "Y", 88.906, 39, "Transition metal"};
    elements_["Zr"] = {"Zirconium", "Zr", 91.224, 40, "Transition metal"};
    elements_["Nb"] = {"Niobium", "Nb", 92.906, 41, "Transition metal"};
    elements_["Mo"] = {"Molybdenum", "Mo", 95.95, 42, "Transition metal"};
    elements_["Tc"] = {"Technetium", "Tc", 98.0, 43, "Transition metal"};
    elements_["Ru"] = {"Ruthenium", "Ru", 101.07, 44, "Transition metal"};
    elements_["Rh"] = {"Rhodium", "Rh", 102.91, 45, "Transition metal"};
    elements_["Pd"] = {"Palladium", "Pd", 106.42, 46, "Transition metal"};
    elements_["Ag"] = {"Silver", "Ag", 107.87, 47, "Transition metal"};
    elements_["Cd"] = {"Cadmium", "Cd", 112.41, 48, "Transition metal"};
    elements_["In"] = {"Indium", "In", 114.82, 49, "Metal"};
    elements_["Sn"] = {"Tin", "Sn", 118.71, 50, "Metal"};
    elements_["Sb"] = {"Antimony", "Sb", 121.76, 51, "Metalloid"};
    elements_["Te"] = {"Tellurium", "Te", 127.60, 52, "Metalloid"};
    elements_["I"] = {"Iodine", "I", 126.90, 53, "Halogen"};
    elements_["Xe"] = {"Xenon", "Xe", 131.29, 54, "Noble gas"};
    elements_["Cs"] = {"Cesium", "Cs", 132.91, 55, "Alkali metal"};
    elements_["Ba"] = {"Barium", "Ba", 137.33, 56, "Alkaline earth metal"};
    elements_["La"] = {"Lanthanum", "La", 138.91, 57, "Lanthanide"};
    elements_["Ce"] = {"Cerium", "Ce", 140.12, 58, "Lanthanide"};
    elements_["Pr"] = {"Praseodymium", "Pr", 140.91, 59, "Lanthanide"};
    elements_["Nd"] = {"Neodymium", "Nd", 144.24, 60, "Lanthanide"};
    elements_["Pm"] = {"Promethium", "Pm", 145.0, 61, "Lanthanide"};
    elements_["Sm"] = {"Samarium", "Sm", 150.36, 62, "Lanthanide"};
    elements_["Eu"] = {"Europium", "Eu", 151.96, 63, "Lanthanide"};
    elements_["Gd"] = {"Gadolinium", "Gd", 157.25, 64, "Lanthanide"};
    elements_["Tb"] = {"Terbium", "Tb", 158.93, 65, "Lanthanide"};
    elements_["Dy"] = {"Dysprosium", "Dy", 162.50, 66, "Lanthanide"};
    elements_["Ho"] = {"Holmium", "Ho", 164.93, 67, "Lanthanide"};
    elements_["Er"] = {"Erbium", "Er", 167.26, 68, "Lanthanide"};
    elements_["Tm"] = {"Thulium", "Tm", 168.93, 69, "Lanthanide"};
    elements_["Yb"] = {"Ytterbium", "Yb", 173.04, 70, "Lanthanide"};
    elements_["Lu"] = {"Lutetium", "Lu", 174.97, 71, "Lanthanide"};
    elements_["Hf"] = {"Hafnium", "Hf", 178.49, 72, "Transition metal"};
    elements_["Ta"] = {"Tantalum", "Ta", 180.95, 73, "Transition metal"};
    elements_["W"] = {"Tungsten", "W", 183.84, 74, "Transition metal"};
    elements_["Re"] = {"Rhenium", "Re", 186.21, 75, "Transition metal"};
    elements_["Os"] = {"Osmium", "Os", 190.23, 76, "Transition metal"};
    elements_["Ir"] = {"Iridium", "Ir", 192.22, 77, "Transition metal"};
    elements_["Pt"] = {"Platinum", "Pt", 195.08, 78, "Transition metal"};
    elements_["Au"] = {"Gold", "Au", 196.97, 79, "Transition metal"};
    elements_["Hg"] = {"Mercury", "Hg", 200.59, 80, "Transition metal"};
    elements_["Tl"] = {"Thallium", "Tl", 204.38, 81, "Metal"};
    elements_["Pb"] = {"Lead", "Pb", 207.2, 82, "Metal"};
    elements_["Bi"] = {"Bismuth", "Bi", 208.98, 83, "Metal"};
    elements_["Po"] = {"Polonium", "Po", 209.0, 84, "Metalloid"};
    elements_["At"] = {"Astatine", "At", 210.0, 85, "Halogen"};
    elements_["Rn"] = {"Radon", "Rn", 222.0, 86, "Noble gas"};
    elements_["Fr"] = {"Francium", "Fr", 223.0, 87, "Alkali metal"};
    elements_["Ra"] = {"Radium", "Ra", 226.0, 88, "Alkaline earth metal"};
    elements_["Ac"] = {"Actinium", "Ac", 227.0, 89, "Actinide"};
    elements_["Th"] = {"Thorium", "Th", 232.04, 90, "Actinide"};
    elements_["Pa"] = {"Protactinium", "Pa", 231.04, 91, "Actinide"};
    elements_["U"] = {"Uranium", "U", 238.03, 92, "Actinide"};
}

void CompoundDatabase::loadCompounds() {
    // Common compounds
    compounds_["H2O"] = {"H2O", "Water", "Water", 18.015, "Liquid", "Polar solvent, essential for life", false};
    compounds_["CO2"] = {"CO2", "Carbon dioxide", "Dry ice (solid)", 44.010, "Gas", "Greenhouse gas, product of combustion", false};
    compounds_["NaCl"] = {"NaCl", "Sodium chloride", "Table salt", 58.443, "Solid", "Ionic compound, essential electrolyte", false};
    compounds_["HCl"] = {"HCl", "Hydrogen chloride", "Hydrochloric acid", 36.461, "Gas/Aqueous", "Strong acid, corrosive", true};
    compounds_["NH3"] = {"NH3", "Ammonia", "Ammonia", 17.031, "Gas", "Weak base, pungent odor", true};
    compounds_["CH4"] = {"CH4", "Methane", "Natural gas", 16.043, "Gas", "Greenhouse gas, fuel", false};
    compounds_["C2H6"] = {"C2H6", "Ethane", "Ethane", 30.070, "Gas", "Hydrocarbon, fuel", false};
    compounds_["C3H8"] = {"C3H8", "Propane", "Propane", 44.097, "Gas", "Fuel gas, liquefied petroleum gas", false};
    compounds_["C6H12O6"] = {"C6H12O6", "Glucose", "Blood sugar", 180.156, "Solid", "Simple sugar, energy source", false};
    compounds_["CaCO3"] = {"CaCO3", "Calcium carbonate", "Limestone, marble", 100.087, "Solid", "Common mineral, antacid", false};
    compounds_["H2SO4"] = {"H2SO4", "Sulfuric acid", "Battery acid", 98.079, "Liquid", "Strong acid, very corrosive", true};
    compounds_["HNO3"] = {"HNO3", "Nitric acid", "Nitric acid", 63.012, "Liquid", "Strong acid, oxidizing agent", true};
    compounds_["NaOH"] = {"NaOH", "Sodium hydroxide", "Lye, caustic soda", 39.997, "Solid", "Strong base, very corrosive", true};
    compounds_["KOH"] = {"KOH", "Potassium hydroxide", "Caustic potash", 56.106, "Solid", "Strong base, very corrosive", true};
    compounds_["Ca(OH)2"] = {"Ca(OH)2", "Calcium hydroxide", "Slaked lime", 74.093, "Solid", "Strong base, used in cement", false};
    compounds_["Fe2O3"] = {"Fe2O3", "Iron(III) oxide", "Rust, hematite", 159.688, "Solid", "Common iron ore, reddish-brown", false};
    compounds_["Al2O3"] = {"Al2O3", "Aluminum oxide", "Alumina", 101.961, "Solid", "Very hard, used in abrasives", false};
    compounds_["SiO2"] = {"SiO2", "Silicon dioxide", "Quartz, sand", 60.084, "Solid", "Very common mineral", false};
    compounds_["MgO"] = {"MgO", "Magnesium oxide", "Magnesia", 40.304, "Solid", "Refractory material", false};
    compounds_["CaO"] = {"CaO", "Calcium oxide", "Quicklime", 56.077, "Solid", "Used in cement, very reactive", false};
    compounds_["H2O2"] = {"H2O2", "Hydrogen peroxide", "Peroxide", 34.015, "Liquid", "Oxidizing agent, antiseptic", false};
    compounds_["C2H5OH"] = {"C2H5OH", "Ethanol", "Ethyl alcohol", 46.069, "Liquid", "Alcohol, solvent, fuel", false};
    compounds_["CH3OH"] = {"CH3OH", "Methanol", "Methyl alcohol", 32.042, "Liquid", "Toxic alcohol, solvent", true};
    compounds_["C8H18"] = {"C8H18", "Octane", "Gasoline component", 114.229, "Liquid", "Hydrocarbon fuel", false};
    compounds_["C2H4"] = {"C2H4", "Ethylene", "Ethene", 28.054, "Gas", "Plant hormone, polymer precursor", false};
    compounds_["C2H2"] = {"C2H2", "Acetylene", "Ethyne", 26.038, "Gas", "Welding gas, very flammable", false};
    compounds_["AgNO3"] = {"AgNO3", "Silver nitrate", "Silver nitrate", 169.873, "Solid", "Photography, antiseptic", false};
    compounds_["BaCl2"] = {"BaCl2", "Barium chloride", "Barium chloride", 208.233, "Solid", "Used in fireworks, toxic", true};
    compounds_["CuSO4"] = {"CuSO4", "Copper(II) sulfate", "Blue vitriol", 159.609, "Solid", "Fungicide, blue crystals", false};
    compounds_["FeCl3"] = {"FeCl3", "Iron(III) chloride", "Ferric chloride", 162.204, "Solid", "Coagulant, etching agent", false};
    compounds_["KMnO4"] = {"KMnO4", "Potassium permanganate", "Permanganate", 158.034, "Solid", "Strong oxidizing agent, purple", false};
    compounds_["Na2CO3"] = {"Na2CO3", "Sodium carbonate", "Washing soda", 105.988, "Solid", "Water softener, glass making", false};
    compounds_["K2CO3"] = {"K2CO3", "Potassium carbonate", "Potash", 138.205, "Solid", "Used in soap making", false};
    compounds_["NH4Cl"] = {"NH4Cl", "Ammonium chloride", "Sal ammoniac", 53.491, "Solid", "Fertilizer, flux", false};
    compounds_["MgSO4"] = {"MgSO4", "Magnesium sulfate", "Epsom salt", 120.366, "Solid", "Laxative, bath salts", false};
    compounds_["ZnO"] = {"ZnO", "Zinc oxide", "Zinc white", 81.38, "Solid", "Sunscreen, white pigment", false};
    compounds_["TiO2"] = {"TiO2", "Titanium dioxide", "Titania", 79.866, "Solid", "White pigment, sunscreen", false};
    compounds_["PbO"] = {"PbO", "Lead(II) oxide", "Litharge", 223.2, "Solid", "Used in ceramics, toxic", true};
    compounds_["SO2"] = {"SO2", "Sulfur dioxide", "Sulfur dioxide", 64.066, "Gas", "Preservative, air pollutant", true};
    compounds_["NO"] = {"NO", "Nitric oxide", "Nitrogen monoxide", 30.006, "Gas", "Cell signaling, air pollutant", false};
    compounds_["NO2"] = {"NO2", "Nitrogen dioxide", "Nitrogen dioxide", 46.006, "Gas", "Brown toxic gas, air pollutant", true};
    compounds_["O3"] = {"O3", "Ozone", "Ozone", 47.998, "Gas", "UV protection, toxic at ground level", true};
}

double CompoundDatabase::getAtomicMass(const std::string& element) const {
    auto it = elements_.find(element);
    if (it != elements_.end()) {
        return it->second.atomicMass;
    }
    std::cerr << "Unknown element: " << element << std::endl;
    return 0.0;
}

ElementData CompoundDatabase::getElementData(const std::string& element) const {
    auto it = elements_.find(element);
    if (it != elements_.end()) {
        return it->second;
    }
    return {"Unknown", element, 0.0, 0, "Unknown"};
}

CompoundData CompoundDatabase::getCompoundData(const std::string& formula) const {
    auto it = compounds_.find(formula);
    if (it != compounds_.end()) {
        return it->second;
    }
    return {formula, "Unknown compound", "", 0.0, "Unknown", "No data available", false};
}

bool CompoundDatabase::isValidElement(const std::string& element) const {
    return elements_.find(element) != elements_.end();
}

bool CompoundDatabase::isKnownCompound(const std::string& formula) const {
    return compounds_.find(formula) != compounds_.end();
}

std::vector<std::string> CompoundDatabase::getAllElements() const {
    std::vector<std::string> result;
    for (const auto& element : elements_) {
        result.push_back(element.first);
    }
    return result;
}

std::vector<std::string> CompoundDatabase::getAllCompounds() const {
    std::vector<std::string> result;
    for (const auto& compound : compounds_) {
        result.push_back(compound.first);
    }
    return result;
}

std::string CompoundDatabase::getElementName(const std::string& symbol) const {
    auto it = elements_.find(symbol);
    if (it != elements_.end()) {
        return it->second.name;
    }
    return "Unknown";
}

std::string CompoundDatabase::getCompoundName(const std::string& formula) const {
    auto it = compounds_.find(formula);
    if (it != compounds_.end()) {
        return it->second.name;
    }
    return "Unknown compound";
}