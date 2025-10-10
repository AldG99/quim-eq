#include "MatrixSolver.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

void MatrixSolver::addStep(const std::string& description, const std::vector<std::vector<double>>& matrix, const std::string& operation) {
    steps_.push_back({description, matrix, operation});
}

void MatrixSolver::swapRows(std::vector<std::vector<double>>& matrix, int row1, int row2) {
    if (row1 != row2) {
        std::swap(matrix[row1], matrix[row2]);
        addStep("Swap rows " + std::to_string(row1 + 1) + " and " + std::to_string(row2 + 1), matrix, "row_swap");
    }
}

void MatrixSolver::scaleRow(std::vector<std::vector<double>>& matrix, int row, double factor) {
    for (size_t col = 0; col < matrix[row].size(); ++col) {
        matrix[row][col] *= factor;
    }
    std::stringstream ss;
    ss << "Multiply row " << (row + 1) << " by " << std::fixed << std::setprecision(3) << factor;
    addStep(ss.str(), matrix, "row_scale");
}

void MatrixSolver::addRowToRow(std::vector<std::vector<double>>& matrix, int sourceRow, int targetRow, double factor) {
    for (size_t col = 0; col < matrix[sourceRow].size(); ++col) {
        matrix[targetRow][col] += factor * matrix[sourceRow][col];
    }
    std::stringstream ss;
    ss << "Add " << std::fixed << std::setprecision(3) << factor << " times row " 
       << (sourceRow + 1) << " to row " << (targetRow + 1);
    addStep(ss.str(), matrix, "row_add");
}

bool MatrixSolver::isZero(double value) const {
    return std::abs(value) < EPSILON;
}

std::vector<double> MatrixSolver::gaussianElimination(std::vector<std::vector<double>>& matrix) {
    steps_.clear();
    
    if (matrix.empty() || matrix[0].empty()) {
        return {};
    }
    
    int rows = matrix.size();
    int cols = matrix[0].size();
    
    addStep("Initial matrix", matrix, "initial");
    
    // Forward elimination
    for (int pivot = 0; pivot < std::min(rows, cols); ++pivot) {
        // Find pivot row (row with largest absolute value in current column)
        int pivotRow = pivot;
        for (int row = pivot + 1; row < rows; ++row) {
            if (std::abs(matrix[row][pivot]) > std::abs(matrix[pivotRow][pivot])) {
                pivotRow = row;
            }
        }
        
        // Swap rows if needed
        if (pivotRow != pivot) {
            swapRows(matrix, pivot, pivotRow);
        }
        
        // Check if pivot is zero
        if (isZero(matrix[pivot][pivot])) {
            continue;
        }
        
        // Eliminate column
        for (int row = pivot + 1; row < rows; ++row) {
            if (!isZero(matrix[row][pivot])) {
                double factor = -matrix[row][pivot] / matrix[pivot][pivot];
                addRowToRow(matrix, pivot, row, factor);
            }
        }
    }
    
    addStep("After forward elimination", matrix, "forward_done");
    
    // Back substitution for homogeneous system
    return solveHomogeneous(matrix);
}

std::vector<double> MatrixSolver::solveHomogeneous(std::vector<std::vector<double>>& matrix) {
    int rows = matrix.size();
    int cols = matrix[0].size();
    
    std::vector<double> solution(cols, 0.0);
    
    // For homogeneous system, set the last variable to 1 and solve backwards
    solution[cols - 1] = 1.0;
    
    // Back substitution
    for (int row = rows - 1; row >= 0; --row) {
        // Find the first non-zero element in this row
        int pivot = -1;
        for (int col = 0; col < cols; ++col) {
            if (!isZero(matrix[row][col])) {
                pivot = col;
                break;
            }
        }
        
        if (pivot == -1) continue; // Zero row
        
        // Calculate the value for this variable
        double sum = 0.0;
        for (int col = pivot + 1; col < cols; ++col) {
            sum += matrix[row][col] * solution[col];
        }
        
        if (!isZero(matrix[row][pivot])) {
            solution[pivot] = -sum / matrix[row][pivot];
        }
    }
    
    addStep("Back substitution complete", matrix, "back_substitution");
    
    return solution;
}

int MatrixSolver::rank(const std::vector<std::vector<double>>& matrix) {
    if (matrix.empty()) return 0;
    
    auto tempMatrix = matrix;
    gaussianElimination(tempMatrix);
    
    int rank = 0;
    for (const auto& row : tempMatrix) {
        bool hasNonZero = false;
        for (double val : row) {
            if (!isZero(val)) {
                hasNonZero = true;
                break;
            }
        }
        if (hasNonZero) rank++;
    }
    
    return rank;
}

bool MatrixSolver::hasUniqueSolution(const std::vector<std::vector<double>>& matrix) {
    if (matrix.empty()) return false;
    
    int matrixRank = rank(matrix);
    int numVars = matrix[0].size();
    
    return matrixRank == numVars - 1;
}

std::vector<SolutionStep> MatrixSolver::getSteps() const {
    return steps_;
}

void MatrixSolver::clearSteps() {
    steps_.clear();
}

void MatrixSolver::printMatrix(const std::vector<std::vector<double>>& matrix) const {
    for (const auto& row : matrix) {
        for (double val : row) {
            std::cout << std::fixed << std::setprecision(3) << std::setw(8) << val << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

std::string MatrixSolver::matrixToString(const std::vector<std::vector<double>>& matrix) const {
    std::stringstream ss;
    for (const auto& row : matrix) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) ss << "  ";
            ss << std::fixed << std::setprecision(3) << std::setw(8) << row[i];
        }
        ss << "\n";
    }
    return ss.str();
}

std::vector<int> MatrixSolver::reduceToIntegers(const std::vector<double>& coefficients) {
    std::vector<int> result;
    
    // Find denominators by converting to fractions
    std::vector<int> numerators;
    std::vector<int> denominators;
    
    const double TOLERANCE = 1e-6;
    
    for (double coef : coefficients) {
        // Find a reasonable fraction representation
        bool found = false;
        for (int denom = 1; denom <= 1000 && !found; ++denom) {
            double num = coef * denom;
            if (std::abs(num - std::round(num)) < TOLERANCE) {
                numerators.push_back(static_cast<int>(std::round(num)));
                denominators.push_back(denom);
                found = true;
            }
        }
        
        if (!found) {
            // Fallback: multiply by 1000 and round
            numerators.push_back(static_cast<int>(std::round(coef * 1000)));
            denominators.push_back(1000);
        }
    }
    
    // Find LCM of all denominators
    int lcmValue = denominators[0];
    for (size_t i = 1; i < denominators.size(); ++i) {
        lcmValue = lcm(lcmValue, denominators[i]);
    }
    
    // Convert to integers
    for (size_t i = 0; i < numerators.size(); ++i) {
        result.push_back(numerators[i] * lcmValue / denominators[i]);
    }
    
    // Find GCD and reduce
    int gcdValue = std::abs(result[0]);
    for (size_t i = 1; i < result.size(); ++i) {
        gcdValue = gcd(gcdValue, std::abs(result[i]));
    }
    
    if (gcdValue > 1) {
        for (int& val : result) {
            val /= gcdValue;
        }
    }
    
    // Make sure all coefficients are positive
    bool allNegative = true;
    for (int val : result) {
        if (val > 0) {
            allNegative = false;
            break;
        }
    }
    
    if (allNegative) {
        for (int& val : result) {
            val = -val;
        }
    }
    
    return result;
}

int MatrixSolver::gcd(int a, int b) {
    a = std::abs(a);
    b = std::abs(b);
    
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int MatrixSolver::lcm(int a, int b) {
    return std::abs(a * b) / gcd(a, b);
}