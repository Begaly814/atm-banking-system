// Written by: Begaly

/*
 * ========================================================================
 * FILE: Bank.cpp
 * PURPOSE: Implements the Bank class methods declared in Bank.h
 * 
 * This file contains the actual code for bank operations:
 *   - Creating banks
 *   - Adding accounts
 *   - Searching for accounts
 *   - Password verification
 *   - Displaying account information
 * 
 * CONNECTS TO:
 *   - Bank.h: Implements methods declared there
 *   - DataStructures.h: Works with Bank_Account_Information objects
 * ========================================================================
 */

#include "Bank.h"
#include <iostream>

/*
 * CONSTRUCTOR: Bank(string bank_name)
 * PURPOSE: Creates a new bank with the specified name
 * 
 * PARAMETER: bank_name - Name of the bank (e.g., "Kakao", "Daegu")
 * 
 * SIMPLE IMPLEMENTATION: Just sets the name member variable
 * The accounts vector is automatically initialized as empty
 */
Bank::Bank(string bank_name) { 
  name = bank_name; 
}

/*
 * CONSTRUCTOR: Bank() - default constructor
 * PURPOSE: Creates a bank with default name "Unknown"
 * 
 * USAGE: Rarely used; mainly for declaring Bank variables before assignment
 */
Bank::Bank() { 
  name = "Unknown"; 
}

/*
 * METHOD: add_new_account_to_bank
 * PURPOSE: Adds a new account to this bank's account list
 * 
 * PARAMETER: account_to_add - The Bank_Account_Information object to add
 * 
 * HOW IT WORKS:
 *   - Uses vector's push_back() method to add to the end of accounts vector
 *   - The account object is COPIED into the vector (not just a pointer)
 * 
 * CALLED BY: main.cpp when loading initial accounts from file
 */
void Bank::add_new_account_to_bank(Bank_Account_Information account_to_add){
  accounts.push_back(account_to_add);  // Add account to end of vector
}

/*
 * METHOD: find_account_by_debit_card_number
 * PURPOSE: Searches for an account matching the given debit card number
 * 
 * PARAMETER: debit_card_number_to_search - Card number to find (e.g., "1111-1111-1111")
 * 
 * RETURNS: 
 *   - Pointer to the account if found
 *   - NULL if no matching account exists
 * 
 * ALGORITHM:
 *   1. Initialize result pointer to NULL (assume not found)
 *   2. Loop through all accounts in the vector
 *   3. For each account, get its card number
 *   4. If it matches the search term, return pointer to that account
 *   5. If loop completes without finding match, return NULL
 * 
 * IMPORTANT NOTE - RETURNING POINTER:
 *   We return the ADDRESS (&) of the account stored in the vector.
 *   This allows the caller to modify the original account (change balance, etc.)
 *   The account continues to live in the vector - we're just giving access to it.
 * 
 * CALLED BY: 
 *   - ATM_Session.cpp: To find user's account when card is inserted
 *   - verify_password_for_card_number: Helper method below
 */
Bank_Account_Information *
Bank::find_account_by_debit_card_number(string debit_card_number_to_search){
  Bank_Account_Information *result = NULL;  // Start with NULL (not found)
  
  // Loop through all accounts
  for(int i = 0; i < accounts.size(); i++){
    string cardNum = accounts[i].get_debit_card_number();  // Get this account's card number
    
    if(cardNum == debit_card_number_to_search){  // Does it match what we're looking for?
      result = &accounts[i];  // Get ADDRESS of account in vector (& = address-of operator)
      return result;          // Return immediately when found
    }
  }
  
  return NULL;  // Not found - return NULL pointer
}

/*
 * METHOD: find_account_by_account_number
 * PURPOSE: Searches for an account matching the given account number
 * 
 * PARAMETER: account_number_to_search - Account number to find (e.g., "100-100-111111")
 * 
 * RETURNS: Pointer to account if found, NULL otherwise
 * 
 * DIFFERENCE FROM ABOVE: 
 *   Same logic, but searches by account_number instead of debit_card_number
 *   This is needed for transfers where recipient provides account number, not card
 * 
 * ALGORITHM: Identical to find_account_by_debit_card_number, just different field
 * 
 * CALLED BY:
 *   - ATM_Transfer.cpp: To find recipient's account for transfers
 */
Bank_Account_Information *
Bank::find_account_by_account_number(string account_number_to_search){
  Bank_Account_Information *result = NULL;  // Start with NULL (not found)
  
  // Loop through all accounts
  for(int i = 0; i < accounts.size(); i++){
    string accNum = accounts[i].get_account_number();  // Get this account's account number
    
    if(accNum == account_number_to_search){  // Match?
      result = &accounts[i];  // Get address of account
      return result;
    }
  }
  
  return NULL;  // Not found
}

/*
 * METHOD: verify_password_for_card_number
 * PURPOSE: Checks if a password is correct for a given card number
 * 
 * PARAMETERS:
 *   card_number_to_verify - Card number to check
 *   password_to_check - Password to verify
 * 
 * RETURNS: 
 *   - true if card exists AND password is correct
 *   - false if card not found OR password is wrong
 * 
 * HOW IT WORKS:
 *   1. Find the account with the given card number
 *   2. If account doesn't exist, return false
 *   3. If account exists, ask it to verify the password
 *   4. Return whatever the account's verify_password method returns
 * 
 * NOTE: This method demonstrates composition - using one method to help implement another
 * 
 * CURRENTLY: Not actively used in the project, but available for authentication
 */
bool Bank::verify_password_for_card_number(string card_number_to_verify,
                                           string password_to_check){
  // Step 1: Find the account
  Bank_Account_Information *foundAcc =
      find_account_by_debit_card_number(card_number_to_verify);
  
  // Step 2: Check if account exists
  if(foundAcc == NULL){
    return false;  // Card not found
  }
  
  // Step 3: Account exists, verify password
  return foundAcc->verify_password(password_to_check);
}

/*
 * METHOD: print_all_accounts
 * PURPOSE: Displays information about all accounts in this bank
 * 
 * USAGE: Called by main.cpp's print_system_snapshot_for_debugging() function
 *        Shows current state of all accounts for monitoring/debugging
 * 
 * MARKED const: This method doesn't modify the Bank object (only reads and displays)
 * 
 * OUTPUT FORMAT: 
 *   "Account [Bank: Kakao, No. 100-100-111111, Owner: Chris] Balance : 4000"
 * 
 * ALGORITHM:
 *   1. Get total number of accounts
 *   2. Loop through each account
 *   3. Extract and display relevant information
 */
void Bank::print_all_accounts() const{
  int totalAccs = accounts.size();  // How many accounts in this bank?
  
  // Loop through each account
  for(int i = 0; i < totalAccs; i++){
    // Extract information from current account
    string bankName = accounts[i].get_bank_name();
    string accNum = accounts[i].get_account_number();
    string ownerName = accounts[i].get_account_owner_name();
    long long bal = accounts[i].get_balance();
    
    // Display account information
    cout << "Account [Bank: " << bankName << ", No. "
         << accNum
         << ", Owner: " << ownerName << "] ";
    cout << "Balance : " << bal << endl;
  }
}
