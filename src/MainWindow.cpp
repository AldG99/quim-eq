#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtCore/QTimer>
#include <QtGui/QFont>
#include <sstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , currentEquation_()
    , balancer_()
    , calculator_()
    , classifier_()
    , centralWidget_(nullptr)
{
    setupUI();
    setupMenus();
    
    // Set window properties
    setWindowTitle("⚖️ Chemical Equation Balancer - Educational Tool");
    setMinimumSize(1000, 700);
    resize(1200, 800);
    
    // Load example equation
    reactantsEdit_->setText("H2 + O2");
    productsEdit_->setText("H2O");
    updateEquationDisplay();
    
    statusBar()->showMessage("Ready - Enter reactants and products, then click 'Balancear'");
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);
    
    mainLayout_ = new QVBoxLayout(centralWidget_);
    mainLayout_->setSpacing(10);
    mainLayout_->setContentsMargins(10, 10, 10, 10);
    
    setupInputSection();
    setupResultsTabs();
    setupBottomStatus();
}

void MainWindow::setupMenus() {
    menuBar_ = menuBar();
    
    // File menu
    QMenu *fileMenu = menuBar_->addMenu("&Archivo");
    fileMenu->addAction("&Exportar Resultados", this, &MainWindow::exportResults, QKeySequence::Save);
    fileMenu->addSeparator();
    fileMenu->addAction("&Salir", this, &QWidget::close, QKeySequence::Quit);
    
    // Edit menu
    QMenu *editMenu = menuBar_->addMenu("&Editar");
    editMenu->addAction("&Limpiar", this, &MainWindow::clearEquation, QKeySequence::Delete);
    editMenu->addAction("&Ejemplos", this, &MainWindow::loadExample, QKeySequence("Ctrl+E"));
    
    // Help menu
    QMenu *helpMenu = menuBar_->addMenu("&Ayuda");
    helpMenu->addAction("&Ayuda", this, &MainWindow::showHelp, QKeySequence::HelpContents);
    helpMenu->addSeparator();
    helpMenu->addAction("&Acerca de", this, &MainWindow::about);
    
    statusBar_ = statusBar();
}

void MainWindow::setupInputSection() {
    inputGroup_ = new QGroupBox("Ecuación Química", centralWidget_);
    inputLayout_ = new QGridLayout(inputGroup_);
    
    // Input fields
    inputLayout_->addWidget(new QLabel("Reactivos:"), 0, 0);
    reactantsEdit_ = new QLineEdit(inputGroup_);
    reactantsEdit_->setPlaceholderText("Ej: H2 + O2");
    connect(reactantsEdit_, &QLineEdit::textChanged, this, &MainWindow::onReactantChanged);
    inputLayout_->addWidget(reactantsEdit_, 0, 1);
    
    inputLayout_->addWidget(new QLabel("Productos:"), 1, 0);
    productsEdit_ = new QLineEdit(inputGroup_);
    productsEdit_->setPlaceholderText("Ej: H2O");
    connect(productsEdit_, &QLineEdit::textChanged, this, &MainWindow::onProductChanged);
    inputLayout_->addWidget(productsEdit_, 1, 1);
    
    // Current equation display
    equationLabel_ = new QLabel("Ecuación actual: ", inputGroup_);
    QFont equationFont = equationLabel_->font();
    equationFont.setPointSize(12);
    equationFont.setBold(true);
    equationLabel_->setFont(equationFont);
    inputLayout_->addWidget(equationLabel_, 2, 0, 1, 2);
    
    // Status
    statusLabel_ = new QLabel("Estado: ❌ No balanceada", inputGroup_);
    statusLabel_->setStyleSheet("QLabel { color: red; }");
    inputLayout_->addWidget(statusLabel_, 3, 0, 1, 2);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    balanceButton_ = new QPushButton("⚖️ Balancear", inputGroup_);
    balanceButton_->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }");
    connect(balanceButton_, &QPushButton::clicked, this, &MainWindow::balanceEquation);
    buttonLayout->addWidget(balanceButton_);
    
    clearButton_ = new QPushButton("🧹 Limpiar", inputGroup_);
    connect(clearButton_, &QPushButton::clicked, this, &MainWindow::clearEquation);
    buttonLayout->addWidget(clearButton_);
    
    examplesButton_ = new QPushButton("📋 Ejemplos", inputGroup_);
    connect(examplesButton_, &QPushButton::clicked, this, &MainWindow::loadExample);
    buttonLayout->addWidget(examplesButton_);
    
    helpButton_ = new QPushButton("❓ Ayuda", inputGroup_);
    connect(helpButton_, &QPushButton::clicked, this, &MainWindow::showHelp);
    buttonLayout->addWidget(helpButton_);
    
    inputLayout_->addLayout(buttonLayout, 4, 0, 1, 2);
    
    mainLayout_->addWidget(inputGroup_);
}

void MainWindow::setupResultsTabs() {
    resultsTabWidget_ = new QTabWidget(centralWidget_);
    
    setupBalancedTab();
    setupMathTab();
    setupChemicalTab();
    setupStoichiometryTab();
    setupTheoryTab();
    
    mainLayout_->addWidget(resultsTabWidget_);
}

void MainWindow::setupBalancedTab() {
    balancedTab_ = new QWidget();
    balancedLayout_ = new QVBoxLayout(balancedTab_);
    
    // Balanced equation display
    balancedEquationLabel_ = new QLabel("", balancedTab_);
    QFont bigFont = balancedEquationLabel_->font();
    bigFont.setPointSize(16);
    bigFont.setBold(true);
    balancedEquationLabel_->setFont(bigFont);
    balancedEquationLabel_->setAlignment(Qt::AlignCenter);
    balancedEquationLabel_->setStyleSheet("QLabel { padding: 20px; background-color: #f0f0f0; border: 2px solid #ccc; border-radius: 8px; }");
    balancedLayout_->addWidget(balancedEquationLabel_);
    
    // Balance status
    balanceStatusLabel_ = new QLabel("", balancedTab_);
    balanceStatusLabel_->setAlignment(Qt::AlignCenter);
    balancedLayout_->addWidget(balanceStatusLabel_);
    
    // Conservation verification
    QGroupBox *conservationGroup = new QGroupBox("Verificación de Conservación", balancedTab_);
    QVBoxLayout *conservationLayout = new QVBoxLayout(conservationGroup);
    
    conservationText_ = new QTextEdit(conservationGroup);
    conservationText_->setMaximumHeight(100);
    conservationText_->setReadOnly(true);
    conservationLayout->addWidget(conservationText_);
    
    balancedLayout_->addWidget(conservationGroup);
    
    // Additional info
    coefficientsLabel_ = new QLabel("Coeficientes: ", balancedTab_);
    balancedLayout_->addWidget(coefficientsLabel_);
    
    reactionTypeLabel_ = new QLabel("Tipo de reacción: ", balancedTab_);
    balancedLayout_->addWidget(reactionTypeLabel_);
    
    balancedLayout_->addStretch();
    
    resultsTabWidget_->addTab(balancedTab_, "⚖️ Ecuación Balanceada");
}

void MainWindow::setupMathTab() {
    mathTab_ = new QWidget();
    mathLayout_ = new QVBoxLayout(mathTab_);
    
    QGroupBox *stepsGroup = new QGroupBox("Proceso Paso a Paso", mathTab_);
    QVBoxLayout *stepsLayout = new QVBoxLayout(stepsGroup);
    
    mathStepsText_ = new QTextEdit(stepsGroup);
    mathStepsText_->setReadOnly(true);
    mathStepsText_->setFont(QFont("Courier", 10));
    stepsLayout->addWidget(mathStepsText_);
    
    mathLayout_->addWidget(stepsGroup);
    
    QGroupBox *matrixGroup = new QGroupBox("Matriz Estequiométrica y Resolución", mathTab_);
    QVBoxLayout *matrixLayout = new QVBoxLayout(matrixGroup);
    
    matrixText_ = new QTextEdit(matrixGroup);
    matrixText_->setReadOnly(true);
    matrixText_->setFont(QFont("Courier", 9));
    matrixLayout->addWidget(matrixText_);
    
    mathLayout_->addWidget(matrixGroup);
    
    resultsTabWidget_->addTab(mathTab_, "🔢 Proceso Matemático");
}

void MainWindow::setupChemicalTab() {
    chemicalTab_ = new QWidget();
    chemicalScrollArea_ = new QScrollArea(chemicalTab_);
    chemicalWidget_ = new QWidget();
    chemicalLayout_ = new QVBoxLayout(chemicalWidget_);
    
    // Reactants information
    QGroupBox *reactantsGroup = new QGroupBox("REACTIVOS", chemicalWidget_);
    QVBoxLayout *reactantsLayout = new QVBoxLayout(reactantsGroup);
    
    reactantsInfoText_ = new QTextEdit(reactantsGroup);
    reactantsInfoText_->setReadOnly(true);
    reactantsInfoText_->setMaximumHeight(200);
    reactantsLayout->addWidget(reactantsInfoText_);
    
    chemicalLayout_->addWidget(reactantsGroup);
    
    // Products information
    QGroupBox *productsGroup = new QGroupBox("PRODUCTOS", chemicalWidget_);
    QVBoxLayout *productsLayout = new QVBoxLayout(productsGroup);
    
    productsInfoText_ = new QTextEdit(productsGroup);
    productsInfoText_->setReadOnly(true);
    productsInfoText_->setMaximumHeight(200);
    productsLayout->addWidget(productsInfoText_);
    
    chemicalLayout_->addWidget(productsGroup);
    
    chemicalScrollArea_->setWidget(chemicalWidget_);
    chemicalScrollArea_->setWidgetResizable(true);
    
    QVBoxLayout *chemicalTabLayout = new QVBoxLayout(chemicalTab_);
    chemicalTabLayout->addWidget(chemicalScrollArea_);
    
    resultsTabWidget_->addTab(chemicalTab_, "📊 Información Química");
}

void MainWindow::setupStoichiometryTab() {
    stoichiometryTab_ = new QWidget();
    stoichiometryLayout_ = new QVBoxLayout(stoichiometryTab_);
    
    // Molar ratios
    QGroupBox *ratiosGroup = new QGroupBox("Relaciones Molares", stoichiometryTab_);
    QVBoxLayout *ratiosLayout = new QVBoxLayout(ratiosGroup);
    
    molarRatioLabel_ = new QLabel("", ratiosGroup);
    molarRatioLabel_->setWordWrap(true);
    ratiosLayout->addWidget(molarRatioLabel_);
    
    stoichiometryLayout_->addWidget(ratiosGroup);
    
    // Mass relations
    QGroupBox *massGroup = new QGroupBox("Relaciones de Masa", stoichiometryTab_);
    QVBoxLayout *massLayout = new QVBoxLayout(massGroup);
    
    massRelationsText_ = new QTextEdit(massGroup);
    massRelationsText_->setReadOnly(true);
    massRelationsText_->setMaximumHeight(150);
    massLayout->addWidget(massRelationsText_);
    
    stoichiometryLayout_->addWidget(massGroup);
    
    // Limiting reagent calculator
    limitingReagentGroup_ = new QGroupBox("Calculadora de Reactivo Limitante", stoichiometryTab_);
    limitingReagentLayout_ = new QGridLayout(limitingReagentGroup_);
    
    reagentTable_ = new QTableWidget(limitingReagentGroup_);
    reagentTable_->setColumnCount(2);
    reagentTable_->setHorizontalHeaderLabels({"Compuesto", "Masa disponible (g)"});
    reagentTable_->horizontalHeader()->setStretchLastSection(true);
    limitingReagentLayout_->addWidget(reagentTable_, 0, 0, 1, 2);
    
    calculateButton_ = new QPushButton("Calcular Limitante", limitingReagentGroup_);
    connect(calculateButton_, &QPushButton::clicked, this, &MainWindow::calculateLimitingReagent);
    limitingReagentLayout_->addWidget(calculateButton_, 1, 0, 1, 2);
    
    limitingReagentResults_ = new QTextEdit(limitingReagentGroup_);
    limitingReagentResults_->setReadOnly(true);
    limitingReagentResults_->setMaximumHeight(150);
    limitingReagentLayout_->addWidget(limitingReagentResults_, 2, 0, 1, 2);
    
    stoichiometryLayout_->addWidget(limitingReagentGroup_);
    
    stoichiometryLayout_->addStretch();
    
    resultsTabWidget_->addTab(stoichiometryTab_, "⚗️ Cálculos Estequiométricos");
}

void MainWindow::setupTheoryTab() {
    theoryTab_ = new QWidget();
    theoryScrollArea_ = new QScrollArea(theoryTab_);
    theoryWidget_ = new QWidget();
    theoryLayout_ = new QVBoxLayout(theoryWidget_);
    
    theoryText_ = new QTextEdit(theoryWidget_);
    theoryText_->setReadOnly(true);
    theoryText_->setHtml(
        "<h2>🔬 Conceptos de Estequiometría</h2>"
        "<h3>Ley de Conservación de la Masa (Lavoisier):</h3>"
        "<p><i>\"En una reacción química, la masa no se crea ni se destruye, solo se transforma.\"</i></p>"
        "<p>Esta ley fundamental establece que el número total de átomos de cada elemento debe ser igual en reactivos y productos.</p>"
        
        "<h3>⚖️ Balanceo de Ecuaciones:</h3>"
        "<p>Proceso para igualar el número de átomos de cada elemento en reactivos y productos mediante coeficientes estequiométricos.</p>"
        
        "<h3>🧮 Método Matemático:</h3>"
        "<ol>"
        "<li><strong>Asignar variables:</strong> Cada coeficiente se representa con una variable (x₁, x₂, x₃...)</li>"
        "<li><strong>Crear ecuaciones:</strong> Para cada elemento, igualar átomos en reactivos y productos</li>"
        "<li><strong>Resolver sistema:</strong> Usar eliminación gaussiana para resolver el sistema de ecuaciones lineales</li>"
        "<li><strong>Coeficientes enteros:</strong> Convertir la solución a los enteros positivos más pequeños</li>"
        "</ol>"
        
        "<h3>📏 Estequiometría:</h3>"
        "<p>Estudio de las relaciones cuantitativas entre reactivos y productos en reacciones químicas.</p>"
        "<p>Permite calcular:</p>"
        "<ul>"
        "<li>Cantidad de productos formados</li>"
        "<li>Cantidad de reactivos necesarios</li>"
        "<li>Reactivo limitante</li>"
        "<li>Rendimiento teórico</li>"
        "</ul>"
        
        "<h3>🔄 Tipos de Reacciones:</h3>"
        "<ul>"
        "<li><strong>Síntesis:</strong> A + B → AB</li>"
        "<li><strong>Descomposición:</strong> AB → A + B</li>"
        "<li><strong>Sustitución Simple:</strong> A + BC → AC + B</li>"
        "<li><strong>Sustitución Doble:</strong> AB + CD → AD + CB</li>"
        "<li><strong>Combustión:</strong> CₓHᵧ + O₂ → CO₂ + H₂O</li>"
        "</ul>"
        
        "<h3>🧪 Aplicaciones Prácticas:</h3>"
        "<ul>"
        "<li><strong>Laboratorio:</strong> Preparación de soluciones, cálculo de reactivos</li>"
        "<li><strong>Industria:</strong> Optimización de procesos químicos</li>"
        "<li><strong>Farmacia:</strong> Síntesis de medicamentos</li>"
        "<li><strong>Medio ambiente:</strong> Análisis de contaminación, tratamiento de aguas</li>"
        "</ul>"
    );
    
    theoryLayout_->addWidget(theoryText_);
    
    theoryScrollArea_->setWidget(theoryWidget_);
    theoryScrollArea_->setWidgetResizable(true);
    
    QVBoxLayout *theoryTabLayout = new QVBoxLayout(theoryTab_);
    theoryTabLayout->addWidget(theoryScrollArea_);
    
    resultsTabWidget_->addTab(theoryTab_, "📚 Teoría");
}

void MainWindow::setupBottomStatus() {
    summaryLabel_ = new QLabel("Fórmula: | Tipo: | Átomos balanceados: ", centralWidget_);
    summaryLabel_->setStyleSheet("QLabel { background-color: #f8f9fa; padding: 8px; border: 1px solid #dee2e6; border-radius: 4px; }");
    mainLayout_->addWidget(summaryLabel_);
}

void MainWindow::balanceEquation() {
    try {
        parseInputToEquation();
        
        statusBar()->showMessage("Balanceando ecuación...");
        balanceButton_->setEnabled(false);
        
        // Use QTimer to allow UI to update
        QTimer::singleShot(100, [this]() {
            auto result = balancer_.balance(currentEquation_);
            
            updateBalancedTab(result);
            updateMathTab(balancer_.getMathematicalSteps(), balancer_.getBalancingSteps());
            updateChemicalTab();
            updateStoichiometryTab();
            updateSummary();
            
            if (result.result == BalanceResult::SUCCESS) {
                displaySuccess("Ecuación balanceada correctamente");
            } else {
                displayError(QString::fromStdString(result.message));
            }
            
            balanceButton_->setEnabled(true);
        });
        
    } catch (const std::exception& e) {
        displayError(QString("Error: %1").arg(e.what()));
        balanceButton_->setEnabled(true);
    }
}

void MainWindow::clearEquation() {
    reactantsEdit_->clear();
    productsEdit_->clear();
    currentEquation_.clear();
    
    balancedEquationLabel_->clear();
    balanceStatusLabel_->clear();
    conservationText_->clear();
    coefficientsLabel_->setText("Coeficientes: ");
    reactionTypeLabel_->setText("Tipo de reacción: ");
    
    mathStepsText_->clear();
    matrixText_->clear();
    
    reactantsInfoText_->clear();
    productsInfoText_->clear();
    
    molarRatioLabel_->clear();
    massRelationsText_->clear();
    limitingReagentResults_->clear();
    reagentTable_->setRowCount(0);
    
    summaryLabel_->setText("Fórmula: | Tipo: | Átomos balanceados: ");
    statusLabel_->setText("Estado: ❌ No balanceada");
    statusLabel_->setStyleSheet("QLabel { color: red; }");
    
    statusBar()->showMessage("Ecuación limpiada - Lista para nueva entrada");
}

void MainWindow::loadExample() {
    auto examples = getExampleEquations();
    
    // For simplicity, just load the first example (in full implementation, show selection dialog)
    if (!examples.empty()) {
        // Parse example: "H2 + O2 -> H2O"
        QString example = examples[0];
        QStringList parts = example.split(" -> ");
        if (parts.size() == 2) {
            reactantsEdit_->setText(parts[0].trimmed());
            productsEdit_->setText(parts[1].trimmed());
            updateEquationDisplay();
        }
    }
}

void MainWindow::showHelp() {
    QMessageBox::information(this, "Ayuda - Balanceador de Ecuaciones Químicas",
        "📝 <b>Instrucciones de Uso:</b><br><br>"
        "1. <b>Reactivos:</b> Escriba las fórmulas de los reactivos separadas por '+' (ej: H2 + O2)<br>"
        "2. <b>Productos:</b> Escriba las fórmulas de los productos separadas por '+' (ej: H2O)<br>"
        "3. <b>Balancear:</b> Haga clic en 'Balancear' para resolver automáticamente<br>"
        "4. <b>Explorar:</b> Use las pestañas para ver información detallada<br><br>"
        
        "🧪 <b>Ejemplos de Fórmulas:</b><br>"
        "• H2O (agua)<br>"
        "• NaCl (sal)<br>"
        "• Ca(OH)2 (hidróxido de calcio)<br>"
        "• CuSO4·5H2O (sulfato de cobre hidratado)<br><br>"
        
        "📊 <b>Características:</b><br>"
        "• Balanceo automático usando álgebra lineal<br>"
        "• Proceso paso a paso explicado<br>"
        "• Información química detallada<br>"
        "• Cálculos estequiométricos<br>"
        "• Clasificación de tipos de reacciones<br>"
        "• Teoría química educativa"
    );
}

void MainWindow::onReactantChanged() {
    updateEquationDisplay();
}

void MainWindow::onProductChanged() {
    updateEquationDisplay();
}

void MainWindow::updateEquationDisplay() {
    QString reactants = reactantsEdit_->text().trimmed();
    QString products = productsEdit_->text().trimmed();
    
    if (reactants.isEmpty() || products.isEmpty()) {
        equationLabel_->setText("Ecuación actual: (incompleta)");
        statusLabel_->setText("Estado: ❌ Incompleta");
        statusLabel_->setStyleSheet("QLabel { color: red; }");
        return;
    }
    
    QString equation = QString("%1 → %2").arg(formatEquation(reactants)).arg(formatEquation(products));
    equationLabel_->setText(QString("Ecuación actual: %1").arg(equation));
    
    statusLabel_->setText("Estado: ❌ No balanceada");
    statusLabel_->setStyleSheet("QLabel { color: red; }");
}

void MainWindow::calculateLimitingReagent() {
    if (!currentEquation_.isBalanced()) {
        displayError("La ecuación debe estar balanceada antes de calcular el reactivo limitante");
        return;
    }
    
    std::map<std::string, double> masses;
    
    // Get masses from table
    for (int row = 0; row < reagentTable_->rowCount(); ++row) {
        QTableWidgetItem* formulaItem = reagentTable_->item(row, 0);
        QTableWidgetItem* massItem = reagentTable_->item(row, 1);
        
        if (formulaItem && massItem) {
            QString formula = formulaItem->text();
            bool ok;
            double mass = massItem->text().toDouble(&ok);
            
            if (ok && mass > 0) {
                masses[formula.toStdString()] = mass;
            }
        }
    }
    
    if (masses.empty()) {
        displayError("Ingrese las masas disponibles para los reactivos");
        return;
    }
    
    try {
        auto result = calculator_.findLimitingReagent(currentEquation_, masses);
        QString resultText = QString::fromStdString(calculator_.formatLimitingReagentResult(result));
        limitingReagentResults_->setText(resultText);
        
        statusBar()->showMessage("Cálculo de reactivo limitante completado");
    } catch (const std::exception& e) {
        displayError(QString("Error en el cálculo: %1").arg(e.what()));
    }
}

void MainWindow::exportResults() {
    // Simplified export - in full implementation would use file dialog
    displaySuccess("Función de exportación no implementada en esta versión de prueba");
}

void MainWindow::about() {
    QMessageBox::about(this, "Acerca de - Balanceador de Ecuaciones Químicas",
        "<h2>⚖️ Balanceador de Ecuaciones Químicas</h2>"
        "<p><b>Versión:</b> 1.0 Educativo</p><br>"
        
        "<p><b>Descripción:</b><br>"
        "Aplicación educativa que balancea automáticamente ecuaciones químicas "
        "usando álgebra lineal y proporciona información química detallada.</p><br>"
        
        "<p><b>Características:</b></p>"
        "<ul>"
        "<li>Balanceo automático mediante eliminación gaussiana</li>"
        "<li>Visualización del proceso matemático paso a paso</li>"
        "<li>Base de datos de compuestos químicos</li>"
        "<li>Cálculos estequiométricos avanzados</li>"
        "<li>Clasificación de tipos de reacciones</li>"
        "<li>Contenido educativo sobre química</li>"
        "</ul><br>"
        
        "<p><b>Tecnología:</b> C++17 con Qt5</p>"
        "<p><b>Algoritmo:</b> Resolución de sistemas de ecuaciones lineales</p><br>"
        
        "<p>Proyecto educativo para aprender química y programación.</p>"
    );
}

// Additional implementation methods continue...
void MainWindow::parseInputToEquation() {
    currentEquation_.clear();
    
    QString reactantsText = reactantsEdit_->text().trimmed();
    QString productsText = productsEdit_->text().trimmed();
    
    if (reactantsText.isEmpty() || productsText.isEmpty()) {
        throw std::invalid_argument("Reactivos y productos no pueden estar vacíos");
    }
    
    // Parse reactants
    QStringList reactants = reactantsText.split("+", Qt::SkipEmptyParts);
    for (const QString& reactant : reactants) {
        QString formula = reactant.trimmed();
        if (!formula.isEmpty()) {
            ChemicalCompound compound(formula.toStdString());
            if (!compound.isValid()) {
                throw std::invalid_argument("Reactivo inválido: " + formula.toStdString());
            }
            currentEquation_.addReactant(compound);
        }
    }
    
    // Parse products
    QStringList products = productsText.split("+", Qt::SkipEmptyParts);
    for (const QString& product : products) {
        QString formula = product.trimmed();
        if (!formula.isEmpty()) {
            ChemicalCompound compound(formula.toStdString());
            if (!compound.isValid()) {
                throw std::invalid_argument("Producto inválido: " + formula.toStdString());
            }
            currentEquation_.addProduct(compound);
        }
    }
}

void MainWindow::updateBalancedTab(const BalanceInfo& result) {
    if (result.result == BalanceResult::SUCCESS || result.result == BalanceResult::ALREADY_BALANCED) {
        balancedEquationLabel_->setText(formatEquation(QString::fromStdString(currentEquation_.toDisplayString())));
        balanceStatusLabel_->setText("✅ Ecuación balanceada correctamente");
        balanceStatusLabel_->setStyleSheet("QLabel { color: green; font-weight: bold; }");
        statusLabel_->setText("Estado: ✅ Balanceada");
        statusLabel_->setStyleSheet("QLabel { color: green; }");
        
        // Show conservation verification
        QString conservation;
        for (const auto& atom : result.atomBalance) {
            QString symbol = QString::fromStdString(atom.first);
            int balance = atom.second;
            QString status = (balance == 0) ? "✅" : "❌";
            conservation += QString("• %1: %2 átomos (diferencia: %3) %4\n")
                           .arg(symbol).arg("balanceado").arg(balance).arg(status);
        }
        conservationText_->setText(conservation);
        
        // Show coefficients
        QString coeffs = "Coeficientes: ";
        for (size_t i = 0; i < result.coefficients.size(); ++i) {
            if (i > 0) coeffs += ", ";
            coeffs += QString::number(result.coefficients[i]);
        }
        coefficientsLabel_->setText(coeffs);
        
        // Show reaction type
        ReactionType type = classifier_.classify(currentEquation_);
        QString typeName = QString::fromStdString(classifier_.getReactionName(type));
        reactionTypeLabel_->setText(QString("Tipo de reacción: %1").arg(typeName));
        
    } else {
        balancedEquationLabel_->setText("Error en el balanceo");
        balanceStatusLabel_->setText(QString("❌ %1").arg(QString::fromStdString(result.message)));
        balanceStatusLabel_->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    }
}

void MainWindow::updateMathTab(const std::vector<SolutionStep>& mathSteps, const std::vector<std::string>& balancingSteps) {
    // Show balancing steps
    QString stepsText;
    for (size_t i = 0; i < balancingSteps.size(); ++i) {
        stepsText += QString("Paso %1: %2\n").arg(i + 1).arg(QString::fromStdString(balancingSteps[i]));
    }
    mathStepsText_->setText(stepsText);
    
    // Show matrix operations
    QString matrixText;
    for (const auto& step : mathSteps) {
        matrixText += QString("=== %1 ===\n").arg(QString::fromStdString(step.description));
        matrixText += QString::fromStdString(step.operation) + "\n";
        matrixText += QString::fromStdString("Matrix:\n");
        
        // Format matrix
        for (const auto& row : step.matrix) {
            for (double val : row) {
                matrixText += QString("%1 ").arg(val, 8, 'f', 3);
            }
            matrixText += "\n";
        }
        matrixText += "\n";
    }
    matrixText_->setText(matrixText);
}

void MainWindow::updateChemicalTab() {
    CompoundDatabase& db = CompoundDatabase::getInstance();
    
    // Reactants info
    QString reactantsInfo;
    for (const auto& reactant : currentEquation_.getReactants()) {
        auto data = db.getCompoundData(reactant.first.getFormula());
        reactantsInfo += QString("├─ %1 (%2)\n").arg(QString::fromStdString(data.formula)).arg(QString::fromStdString(data.name));
        reactantsInfo += QString("│  • Masa molar: %.3f g/mol\n").arg(data.molarMass);
        reactantsInfo += QString("│  • Estado: %1\n").arg(QString::fromStdString(data.state));
        reactantsInfo += QString("│  • Propiedades: %1\n\n").arg(QString::fromStdString(data.properties));
    }
    reactantsInfoText_->setText(reactantsInfo);
    
    // Products info
    QString productsInfo;
    for (const auto& product : currentEquation_.getProducts()) {
        auto data = db.getCompoundData(product.first.getFormula());
        productsInfo += QString("└─ %1 (%2)\n").arg(QString::fromStdString(data.formula)).arg(QString::fromStdString(data.name));
        productsInfo += QString("   • Masa molar: %.3f g/mol\n").arg(data.molarMass);
        productsInfo += QString("   • Estado: %1\n").arg(QString::fromStdString(data.state));
        productsInfo += QString("   • Propiedades: %1\n\n").arg(QString::fromStdString(data.properties));
    }
    productsInfoText_->setText(productsInfo);
}

void MainWindow::updateStoichiometryTab() {
    if (!currentEquation_.isBalanced()) {
        molarRatioLabel_->setText("Balancee la ecuación primero para ver relaciones estequiométricas");
        massRelationsText_->clear();
        reagentTable_->setRowCount(0);
        return;
    }
    
    // Molar ratios
    auto ratios = calculator_.calculateMolarRatios(currentEquation_);
    molarRatioLabel_->setText(QString("Relaciones molares: %1").arg(QString::fromStdString(ratios.description)));
    
    // Mass relations
    auto relations = calculator_.calculateAllRelations(currentEquation_);
    QString massText;
    for (const auto& relation : relations) {
        massText += QString("• %.3f g %1 : %.3f g %2 (ratio: %.3f)\n")
                   .arg(QString::fromStdString(relation.compound1))
                   .arg(QString::fromStdString(relation.compound2))
                   .arg(relation.massRatio);
    }
    massRelationsText_->setText(massText);
    
    // Setup reagent table
    reagentTable_->setRowCount(currentEquation_.getReactants().size());
    int row = 0;
    for (const auto& reactant : currentEquation_.getReactants()) {
        reagentTable_->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(reactant.first.getFormula())));
        reagentTable_->setItem(row, 1, new QTableWidgetItem(""));
        row++;
    }
}

void MainWindow::updateSummary() {
    QString formula = QString::fromStdString(currentEquation_.toString());
    ReactionType type = classifier_.classify(currentEquation_);
    QString typeName = QString::fromStdString(classifier_.getReactionName(type));
    
    QString atoms;
    auto elements = currentEquation_.getAllElements();
    for (size_t i = 0; i < elements.size(); ++i) {
        if (i > 0) atoms += ", ";
        atoms += QString::fromStdString(elements[i]);
    }
    
    summaryLabel_->setText(QString("Fórmula: %1 | Tipo: %2 | Elementos: %3")
                          .arg(formula).arg(typeName).arg(atoms));
}

void MainWindow::displayError(const QString& message) {
    statusBar()->showMessage("Error: " + message, 5000);
    statusBar()->setStyleSheet("QStatusBar { color: red; }");
}

void MainWindow::displaySuccess(const QString& message) {
    statusBar()->showMessage(message, 3000);
    statusBar()->setStyleSheet("QStatusBar { color: green; }");
}

QString MainWindow::formatChemicalFormula(const QString& formula) {
    // Simple subscript formatting
    QString formatted = formula;
    formatted.replace("2", "₂");
    formatted.replace("3", "₃");
    formatted.replace("4", "₄");
    formatted.replace("5", "₅");
    formatted.replace("6", "₆");
    formatted.replace("7", "₇");
    formatted.replace("8", "₈");
    formatted.replace("9", "₉");
    return formatted;
}

QString MainWindow::formatEquation(const QString& equation) {
    return formatChemicalFormula(equation);
}

std::vector<QString> MainWindow::getExampleEquations() {
    return {
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
}