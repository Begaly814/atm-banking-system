// Written by: Munkhtemuulen 
// Written by: Begaly 

/*
 * ========================================================================
 * FILE: Bank.h
 * PURPOSE: Declares the Bank class that manages customer accounts
 * 
 * WHAT THIS FILE CONTAINS:
 * The Bank class represents a financial institution (e.g., "Kakao Bank", "Daegu Bank").
 * Each Bank object stores and manages multiple customer accounts.
 * 
 * CONNECTS TO:
 *   - DataStructures.h: Uses Bank_Account_Information class
 *   - main.cpp: Creates Bank objects and stores them in a global vector
 *   - ATM_Session.cpp: Searches banks for user's account
 *   - ATM_Transfer.cpp: Searches banks for recipient's account
 * 
 * KEY CONCEPT - VECTOR:
 *   'vector<Bank_Account_Information> accounts' is a dynamic array that can
 *   grow/shrink as needed. It stores all accounts that belong to this bank.
 * ========================================================================
 */

#ifndef BANK_H
#define BANK_H

#include "DataStructures.h"  // For Bank_Account_Information class
#include <string>
#include <vector>              // For std::vector container


/*
 * CLASS: Bank
 * PURPOSE: Represents a financial institution that holds customer accounts
 * 
 * RESPONSIBILITIES:
 *   1. Store all accounts that belong to this bank
 *   2. Add new accounts to the bank
 *   3. Search for accounts by card number or account number
 *   4. Verify passwords for authentication
 *   5. Display all accounts (for debugging/admin)
 * 
 * DATA STORAGE:
 *   Each Bank object contains a vector (dynamic array) of Bank_Account_Information objects.
 *   When ATM needs to find an account, it searches through all banks in the system.
 */
class Bank {
private:
  // PRIVATE MEMBERS: Cannot be accessed directly from outside the class

  string name;  // Name of the bank (e.g., "Kakao", "Daegu")
  
  /*
   * Vector of accounts belonging to this bank
   * 
   * IMPORTANT CONCEPT:
   * This is a vector of OBJECTS (not pointers). Each Bank_Account_Information
   * is stored directly in the vector. When we return a pointer from search
   * functions, we're returning the ADDRESS of an object that lives in this vector.
   */
  vector<Bank_Account_Information> accounts;

public:
  // PUBLIC METHODS: Can be called from anywhere

  /*
   * CONSTRUCTOR: Bank(string bank_name)
   * PURPOSE: Creates a new bank with a specified name
   * CALLED BY: main.cpp when loading banks from file
   * IMPLEMENTATION: In Bank.cpp
   */
  Bank(string bank_name);
  
  /*
   * CONSTRUCTOR: Bank() - default constructor
   * PURPOSE: Creates a bank with name "Unknown"
   * USAGE: When you need to declare a Bank variable before initializing it
   * IMPLEMENTATION: In Bank.cpp
   */
  Bank();

  /*
   * METHOD: get_name
   * PURPOSE: Returns the name of this bank
   * RETURNS: Bank name as a string
   * USAGE: Checking if account's bank matches ATM's primary bank
   */
  string get_name() const { return name; }

  /*
   * METHOD: add_new_account_to_bank
   * PURPOSE: Adds a new account to this bank's account list
   * 
   * PARAMETER: account_to_add - The account object to add
   * 
   * CALLED BY: main.cpp when loading accounts from initial condition file
   * 
   * IMPLEMENTATION: In Bank.cpp
   *                 Simply calls accounts.push_back() to add to vector
   */
  void add_new_account_to_bank(Bank_Account_Information account_to_add);

  /*
   * METHOD: find_account_by_debit_card_number
   * PURPOSE: Searches for an account using the debit card number
   * 
   * PARAMETER: debit_card_number_to_search - Card number to look for (e.g., "1111-1111-1111")
   * 
   * RETURNS: Pointer to the account if found, NULL if not found
   * 
   * CALLED BY: ATM_Session.cpp when user inserts card at ATM
   * 
   * WHY RETURN A POINTER?
   * We return a pointer so the ATM can modify the actual account (change balance, etc.)
   * Returning a pointer avoids copying the entire account object.
   * 
   * IMPLEMENTATION: In Bank.cpp
   *                 Loops through accounts vector comparing card numbers
   */
  Bank_Account_Information *
  find_account_by_debit_card_number(string debit_card_number_to_search);
  
  /*
   * METHOD: find_account_by_account_number
   * PURPOSE: Searches for an account using the account number
   * 
   * PARAMETER: account_number_to_search - Account number to look for (e.g., "100-100-111111")
   * 
   * RETURNS: Pointer to the account if found, NULL if not found
   * 
   * CALLED BY: ATM_Transfer.cpp when looking up recipient's account
   * 
   * DIFFERENCE FROM ABOVE: Uses account number instead of card number for lookup
   * 
   * IMPLEMENTATION: In Bank.cpp
   *                 Similar to find_account_by_debit_card_number but compares account numbers
   */
  Bank_Account_Information *
  find_account_by_account_number(string account_number_to_search);

  /*
   * METHOD: verify_password_for_card_number
   * PURPOSE: Checks if the password is correct for a given card number
   * 
   * PARAMETERS:
   *   card_number_to_verify - The card number to check
   *   password_to_check - The password to verify
   * 
   * RETURNS: true if password is correct, false if wrong or card not found
   * 
   * HOW IT WORKS:
   *   1. Find the account with the given card number
   *   2. If account found, verify the password
   *   3. If account not found or password wrong, return false
   * 
   * CALLED BY: Could be used for authentication (not actively used in current code)
   * 
   * IMPLEMENTATION: In Bank.cpp
   *                 Calls find_account_by_debit_card_number, then verifies password
   */
  bool verify_password_for_card_number(string card_number_to_verify,
                                       string password_to_check);

  /*
   * METHOD: print_all_accounts
   * PURPOSE: Displays all accounts in this bank (for debugging/admin purposes)
   * 
   * CALLED BY: main.cpp's print_system_snapshot_for_debugging() function
   * 
   * OUTPUT FORMAT: Shows bank name, account number, owner name, and balance
   * 
   * MARKED const: This method doesn't modify the Bank object
   * 
   * IMPLEMENTATION: In Bank.cpp
   *                 Loops through accounts vector and prints each one
   */
  void print_all_accounts() const;
};

#endif
