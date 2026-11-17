// Written by: Munkhtemuulen

/*
 * ========================================================================
 * FILE: Exceptions.h
 * PURPOSE: Defines custom exception classes for error handling in the ATM system
 * 
 * WHAT ARE EXCEPTIONS?
 * Exceptions are C++'s way of handling errors. When something goes wrong,
 * we "throw" an exception. The code that called the function can "catch"
 * the exception and handle the error gracefully instead of crashing.
 * 
 * INHERITANCE HIERARCHY:
 *   std::exception (C++ standard library base class)
 *      └─ ATM_Exception (our custom base class)
 *           ├─ Insufficient_Funds_Exception (not enough money in account)
 *           ├─ Insufficient_Cash_Exception (ATM out of cash)
 *           ├─ Invalid_Input_Exception (user entered bad data)
 *           ├─ File_IO_Exception (can't read/write files)
 *           └─ Account_Not_Found_Exception (account doesn't exist)
 * 
 * WHY USE INHERITANCE?
 * All our exceptions inherit from ATM_Exception, which inherits from std::exception.
 * This allows us to catch specific exceptions or catch all ATM exceptions at once.
 * ========================================================================
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>  // For std::exception base class
#include <string>

using namespace std;

/*
 * CLASS: ATM_Exception
 * PURPOSE: Base class for all ATM-related exceptions
 * 
 * INHERITANCE: Inherits from std::exception (C++ standard exception class)
 * 
 * KEY CONCEPTS:
 *   - 'public exception': ATM_Exception IS-A exception (inheritance)
 *   - 'protected': Members accessible to this class and derived classes
 *   - 'virtual': Allows derived classes to override the method
 *   - 'const noexcept': Method won't modify object and won't throw exceptions
 *   - 'override': Explicitly states we're overriding parent's virtual method
 * 
 * USAGE: Not thrown directly; instead, throw one of the specific derived exceptions
 */
class ATM_Exception : public exception {
protected:
  // 'protected' means derived classes can access this, but outside code cannot
  string error_message;  // Stores the error message describing what went wrong

public:

  /*
   * CONSTRUCTOR: Stores the error message when exception is created
   * CALLED BY: Derived class constructors
   */
  ATM_Exception(string message) { error_message = message; }

  /*
   * METHOD: what()
   * PURPOSE: Returns the error message as a C-style string
   * 
   * OVERRIDES: std::exception::what() method
   * 
   * EXPLANATION OF KEYWORDS:
   *   - virtual: Can be overridden by derived classes
   *   - const: Doesn't modify the object
   *   - noexcept: Promises not to throw any exceptions
   *   - override: Confirms we're overriding a parent virtual method
   * 
   * RETURNS: C-style string (const char*) with error message
   * 
   * USAGE: When you catch an exception, call e.what() to get the message
   */
  virtual const char *what() const noexcept override {

    return error_message.c_str();  // Convert C++ string to C-style string

  }
};

/*
 * CLASS: Insufficient_Funds_Exception
 * PURPOSE: Thrown when account doesn't have enough money for an operation
 * 
 * WHEN THROWN:
 *   - ATM_Withdrawal.cpp: When balance < (withdrawal amount + fee)
 *   - ATM_Transfer.cpp: When balance < (transfer amount + fee)
 * 
 * INHERITANCE: Inherits from ATM_Exception, so it has access to error_message
 *              and the what() method
 * 
 * CONSTRUCTOR SYNTAX EXPLAINED:
 *   'Insufficient_Funds_Exception(string message) : ATM_Exception(message)'
 *   The part after ':' calls the parent class constructor to initialize error_message
 */
class Insufficient_Funds_Exception : public ATM_Exception {
public:

  // Constructor: Takes message and passes it to parent (ATM_Exception) constructor
  Insufficient_Funds_Exception(string message) : ATM_Exception(message) {}

};

/*
 * CLASS: Insufficient_Cash_Exception
 * PURPOSE: Thrown when ATM doesn't have enough physical cash to fulfill withdrawal
 * 
 * WHEN THROWN:
 *   - ATM_Withdrawal.cpp: When ATM can't dispense exact amount with available bills
 * 
 * EXAMPLE: User wants 7,000 won but ATM only has 50,000 won bills left
 */
class Insufficient_Cash_Exception : public ATM_Exception {
public:

  Insufficient_Cash_Exception(string message) : ATM_Exception(message) {}

};

/*
 * CLASS: Invalid_Input_Exception
 * PURPOSE: Thrown when user provides invalid input
 * 
 * WHEN THROWN:
 *   - ATM_CashDeposit.cpp: Too many bills (>50) or insufficient fee
 *   - ATM_CheckDeposit.cpp: Too many checks (>30), check < 100,000 won, or insufficient fee
 *   - ATM_Withdrawal.cpp: Withdrawal amount > 500,000 won
 *   - ATM_Transfer.cpp: Cash transfer amount too small to cover fee
 * 
 * USAGE: For validation errors where the user needs to re-enter correct data
 */
class Invalid_Input_Exception : public ATM_Exception {
public:

  Invalid_Input_Exception(string message) : ATM_Exception(message) {}

};

/*
 * CLASS: File_IO_Exception
 * PURPOSE: Thrown when file operations fail (reading/writing)
 * 
 * WHEN THROWN:
 *   - main.cpp: When sample_initial_condition.txt cannot be opened
 * 
 * USAGE: For all file-related errors (file not found, no permission, etc.)
 */
class File_IO_Exception : public ATM_Exception {
public:

  File_IO_Exception(string message) : ATM_Exception(message) {}

};

/*
 * CLASS: Account_Not_Found_Exception
 * PURPOSE: Thrown when trying to access an account that doesn't exist
 * 
 * WHEN THROWN:
 *   - ATM_Transfer.cpp: When recipient's account number is not found in any bank
 * 
 * USAGE: When searching for accounts by account number or card number fails
 */
class Account_Not_Found_Exception : public ATM_Exception {
public:

  Account_Not_Found_Exception(string message) : ATM_Exception(message) {}

};

/*
 * HOW TO USE EXCEPTIONS (FOR BEGINNERS):
 * 
 * 1. THROWING AN EXCEPTION (when error occurs):
 *    throw Insufficient_Funds_Exception("Account balance too low");
 * 
 * 2. CATCHING EXCEPTIONS (handling the error):
 *    try {
 *        // Code that might throw exception
 *        perform_withdrawal(...);
 *    }
 *    catch (const Insufficient_Funds_Exception &e) {
 *        // Handle this specific error
 *        cout << "Error: " << e.what() << endl;
 *    }
 *    catch (const ATM_Exception &e) {
 *        // Handle any other ATM exception
 *        cout << "ATM Error: " << e.what() << endl;
 *    }
 * 
 * 3. WHY USE &e (reference)?
 *    Catching by reference avoids copying the exception object and allows
 *    polymorphism to work (catching base class catches derived classes too)
 */

#endif
