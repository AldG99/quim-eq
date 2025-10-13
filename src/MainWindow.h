#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>

#include "ChemicalCompound.h"
#include "EquationBalancer.h"
#include "StoichiometryCalculator.h"
#include "ReactionClassifier.h"
#include "CompoundDatabase.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void balanceEquation();
    void clearEquation();
    void loadExample();
    void showHelp();
    void onReactantChanged();
    void onProductChanged();
    void calculateLimitingReagent();
    void exportResults();
    void about();

private:
    // Core components
    ChemicalEquation currentEquation_;
    EquationBalancer balancer_;
    StoichiometryCalculator calculator_;
    ReactionClassifier classifier_;
    
    // Main UI components
    QWidget *centralWidget_;
    QVBoxLayout *mainLayout_;
    
    // Input section
    QGroupBox *inputGroup_;
    QGridLayout *inputLayout_;
    QLineEdit *reactantsEdit_;
    QLineEdit *productsEdit_;
    QLabel *equationLabel_;
    QLabel *statusLabel_;
    QPushButton *balanceButton_;
    QPushButton *clearButton_;
    QPushButton *examplesButton_;
    QPushButton *helpButton_;
    
    // Results tabs
    QTabWidget *resultsTabWidget_;
    
    // Tab 1: Balanced Equation
    QWidget *balancedTab_;
    QVBoxLayout *balancedLayout_;
    QLabel *balancedEquationLabel_;
    QLabel *balanceStatusLabel_;
    QTextEdit *conservationText_;
    QLabel *coefficientsLabel_;
    QLabel *reactionTypeLabel_;
    
    // Tab 2: Mathematical Process
    QWidget *mathTab_;
    QVBoxLayout *mathLayout_;
    QTextEdit *mathStepsText_;
    QTextEdit *matrixText_;
    
    // Tab 3: Chemical Information
    QWidget *chemicalTab_;
    QScrollArea *chemicalScrollArea_;
    QWidget *chemicalWidget_;
    QVBoxLayout *chemicalLayout_;
    QTextEdit *reactantsInfoText_;
    QTextEdit *productsInfoText_;
    
    // Tab 4: Stoichiometric Calculations
    QWidget *stoichiometryTab_;
    QVBoxLayout *stoichiometryLayout_;
    QLabel *molarRatioLabel_;
    QTextEdit *massRelationsText_;
    QGroupBox *limitingReagentGroup_;
    QGridLayout *limitingReagentLayout_;
    QTableWidget *reagentTable_;
    QPushButton *calculateButton_;
    QTextEdit *limitingReagentResults_;
    
    // Tab 5: Theory
    QWidget *theoryTab_;
    QScrollArea *theoryScrollArea_;
    QWidget *theoryWidget_;
    QVBoxLayout *theoryLayout_;
    QTextEdit *theoryText_;
    
    // Bottom status
    QLabel *summaryLabel_;
    
    // Menu and status
    QMenuBar *menuBar_;
    QStatusBar *statusBar_;
    
    // Private methods
    void setupUI();
    void setupMenus();
    void setupInputSection();
    void setupResultsTabs();
    void setupBalancedTab();
    void setupMathTab();
    void setupChemicalTab();
    void setupStoichiometryTab();
    void setupTheoryTab();
    void setupBottomStatus();
    
    void updateEquationDisplay();
    void updateBalancedTab(const BalanceInfo& result);
    void updateMathTab(const std::vector<SolutionStep>& steps, const std::vector<std::string>& balancingSteps);
    void updateChemicalTab();
    void updateStoichiometryTab();
    void updateTheoryTab();
    void updateSummary();
    
    void displayError(const QString& message);
    void displaySuccess(const QString& message);
    
    QString formatChemicalFormula(const QString& formula);
    QString formatEquation(const QString& equation);
    
    std::vector<QString> getExampleEquations();
    void parseInputToEquation();
};

#endif // MAINWINDOW_H