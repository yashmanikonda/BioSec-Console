#include "GenerateUID.h"
#include "PasswordHandling.h"
#include <iostream>
#include <fstream>
#include <string>

void displayMenu() {
    std::cout << "Welcome to Bio Sec\n";
    std::cout << " \n";
    std::cout << "Available commands:\n";
    std::cout << " - start: Start the application process\n";
    std::cout << " - exit or close: Stop the application\n";
}

int startApplication() {
    std::cout << "Starting the application process...\n";
   
    std::cout << "Extracting UID Info...\n";

    if (UIDGenerator::GenerateAndWriteUID()) {
        std::cout << "UID verified" << std::endl;

        PasswordHandler passwordHandler;

        if (passwordHandler.HandlePasswordLogic()) {
            std::cout << "Password verified" << std::endl;

        }
        //else {
        //    std::cout << "Invalid Password Settings" << std::endl;
        //}
    }
    else {
        std::cerr << "UID not matching" << std::endl;
        return 1; // Return an error code or handle accordingly
    }

}

int main() {

    // Display the command menu
    displayMenu();

    std::string command;
    while (true) {
        std::cout << " \n";
        std::cout << "Enter a command: ";
        std::cin >> command;

        if (command == "start" || command == "START" || command == "Start") {
            startApplication();
        }
        else if (command == "exit" || command == "EXIT" || command == "Exit" || command == "close" || command == "Close" || command == "CLOSE") {
            std::cout << "Exiting the application...\n";
            break;
        }
        else {
            std::cout << "Invalid command. Please try again.\n";
        }
    }

    // Your application logic goes here

    return 0;
}
