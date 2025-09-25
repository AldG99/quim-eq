#include <QtWidgets/QApplication>
#include <QtCore/QCoreApplication>
#include <QtGui/QFont>
#include <QtWidgets/QStyleFactory>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtGui/QScreen>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDebug>
#include "MainWindow.h"
#include "CompoundDatabase.h"

void setupApplication(QApplication& app) {
    // Set application properties
    app.setApplicationName("Chemical Equation Balancer");
    app.setApplicationDisplayName("⚖️ Balanceador de Ecuaciones Químicas");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Educational Software");
    app.setOrganizationDomain("education.chemistry");
    
    // Set application icon (if available)
    // app.setWindowIcon(QIcon(":/icons/app_icon.png"));
    
    // Set application style
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Set custom font
    QFont font = app.font();
    font.setFamily("Segoe UI, Arial, sans-serif");
    font.setPointSize(10);
    app.setFont(font);
    
    // Set application stylesheet for better appearance
    QString styleSheet = R"(
        QMainWindow {
            background-color: #f8f9fa;
        }
        
        QGroupBox {
            font-weight: bold;
            border: 2px solid #cccccc;
            border-radius: 8px;
            margin-top: 1ex;
            padding-top: 10px;
            background-color: white;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            color: #495057;
        }
        
        QTabWidget::pane {
            border: 1px solid #dee2e6;
            background-color: white;
            border-radius: 4px;
        }
        
        QTabBar::tab {
            background-color: #e9ecef;
            border: 1px solid #dee2e6;
            padding: 8px 12px;
            margin-right: 2px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        
        QTabBar::tab:selected {
            background-color: white;
            border-bottom: 1px solid white;
        }
        
        QTabBar::tab:hover:!selected {
            background-color: #f8f9fa;
        }
        
        QLineEdit {
            border: 2px solid #ced4da;
            border-radius: 4px;
            padding: 8px;
            font-size: 11pt;
            background-color: white;
        }
        
        QLineEdit:focus {
            border-color: #007bff;
            outline: none;
        }
        
        QPushButton {
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
            background-color: #6c757d;
            color: white;
        }
        
        QPushButton:hover {
            background-color: #5a6268;
        }
        
        QPushButton:pressed {
            background-color: #495057;
        }
        
        QTextEdit {
            border: 1px solid #dee2e6;
            border-radius: 4px;
            background-color: white;
            font-family: 'Courier New', monospace;
        }
        
        QTableWidget {
            border: 1px solid #dee2e6;
            border-radius: 4px;
            background-color: white;
            gridline-color: #dee2e6;
        }
        
        QTableWidget::item {
            padding: 4px;
        }
        
        QTableWidget::item:selected {
            background-color: #007bff;
            color: white;
        }
        
        QScrollArea {
            border: none;
            background-color: white;
        }
        
        QStatusBar {
            background-color: #e9ecef;
            border-top: 1px solid #dee2e6;
        }
        
        QMenuBar {
            background-color: #f8f9fa;
            border-bottom: 1px solid #dee2e6;
        }
        
        QMenuBar::item {
            background-color: transparent;
            padding: 4px 8px;
        }
        
        QMenuBar::item:selected {
            background-color: #007bff;
            color: white;
        }
        
        QMenu {
            background-color: white;
            border: 1px solid #dee2e6;
        }
        
        QMenu::item {
            padding: 8px 20px;
        }
        
        QMenu::item:selected {
            background-color: #007bff;
            color: white;
        }
    )";
    
    app.setStyleSheet(styleSheet);
}

void showSplashMessage() {
    // Could add a splash screen here for professional appearance
    qDebug() << "Initializing Chemical Equation Balancer...";
    qDebug() << "Loading compound database...";
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Setup application appearance and properties
    setupApplication(app);
    
    // Show loading message
    showSplashMessage();
    
    try {
        // Initialize compound database (this might take a moment)
        CompoundDatabase::getInstance();
        
        // Create and show main window
        MainWindow window;
        window.show();
        
        // Center window on screen
        QScreen* screen = QApplication::primaryScreen();
        if (screen) {
            QRect screenGeometry = screen->geometry();
            int x = (screenGeometry.width() - window.width()) / 2;
            int y = (screenGeometry.height() - window.height()) / 2;
            window.move(x, y);
        }
        
        qDebug() << "Application initialized successfully";
        
        // Start event loop
        return app.exec();
        
    } catch (const std::exception& e) {
        qCritical() << "Failed to initialize application:" << e.what();
        
        QMessageBox::critical(nullptr, "Initialization Error", 
            QString("Failed to start Chemical Equation Balancer:\n\n%1\n\n"
                   "Please check your installation and try again.").arg(e.what()));
        
        return 1;
    } catch (...) {
        qCritical() << "Unknown error during initialization";
        
        QMessageBox::critical(nullptr, "Initialization Error", 
            "An unknown error occurred during startup.\n\n"
            "Please check your installation and try again.");
        
        return 1;
    }
}