#ifndef MATRIX_SOLVER_H
#define MATRIX_SOLVER_H

#include <vector>
#include <string>

struct SolutionStep {
    std::string description;
    std::vector<std::vector<double>> matrix;
    std::string operation;
};

class MatrixSolver {
private:
    std::vector<SolutionStep> steps_;
    const double EPSILON = 1e-10;
    
    void addStep(const std::string& description, const std::vector<std::vector<double>>& matrix, const std::string& operation = "");
    void swapRows(std::vector<std::vector<double>>& matrix, int row1, int row2);
    void scaleRow(std::vector<std::vector<double>>& matrix, int row, double factor);
    void addRowToRow(std::vector<std::vector<double>>& matrix, int sourceRow, int targetRow, double factor);
    bool isZero(double value) const;
    
public:
    std::vector<double> gaussianElimination(std::vector<std::vector<double>>& matrix);
    std::vector<double> solveHomogeneous(std::vector<std::vector<double>>& matrix);
    
    int rank(const std::vector<std::vector<double>>& matrix);
    bool hasUniqueSolution(const std::vector<std::vector<double>>& matrix);
    
    std::vector<SolutionStep> getSteps() const;
    void clearSteps();
    
    void printMatrix(const std::vector<std::vector<double>>& matrix) const;
    std::string matrixToString(const std::vector<std::vector<double>>& matrix) const;
    
    std::vector<int> reduceToIntegers(const std::vector<double>& coefficients);
    int gcd(int a, int b);
    int lcm(int a, int b);
};

#endif // MATRIX_SOLVER_H