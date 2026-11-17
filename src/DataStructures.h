// Written by: Munkhtemuulen

/*
 * ========================================================================
 * FILE: DataStructures.h
 * PURPOSE: Defines the fundamental data structures used throughout the ATM system
 * 
 * This file contains the core data types that represent:
 * 1. Cash denominations (bill types and quantities in ATM)
 * 2. Bank account information (customer accounts)
 * 3. Transaction history records (logging all transactions)
 * 
 * These structures are used by both Bank and ATM classes to manage data.
 * ========================================================================
 */

#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <string>
#include <vector>

using namespace std;

/*
 * STRUCT: Cash_Denomination
 * PURPOSE: Represents one type of bill denomination in the ATM cash inventory
 * 
 * USAGE: Each ATM stores an array of 4 Cash_Denominations:
 *        - 50,000 won bills
 *        - 10,000 won bills
 *        - 5,000 won bills
 *        - 1,000 won bills
 * 
 * MEMBERS:
 *   bill_value: The face value of the bill (e.g., 50000, 10000, 5000, 1000)
 *   number_of_bills: How many bills of this denomination are currently in ATM
 */
struct Cash_Denomination {

  int bill_value;        // The value of each bill (50000, 10000, 5000, or 1000)
  int number_of_bills;   // How many bills of this type are available

};

/*
 * CLASS: Bank_Account_Information
 * PURPOSE: Stores all information about a customer's bank account
 * 
 * This class represents a single bank account in the system. It contains
 * private member variables to protect sensitive data (balance, password)
 * and provides public methods to safely access and modify account information.
 * 
 * CONNECTS TO:
 *   - Bank class: Each Bank contains a vector of Bank_Account_Information objects
 *   - ATM class: ATM operations modify account balances through this class
 * 
 * KEY CONCEPTS FOR BEGINNERS:
 *   - Private members: Can only be accessed through public methods (encapsulation)
 *   - Public methods: Provide controlled access to private data
 *   - const methods: Methods marked 'const' don't modify the object
 */
class Bank_Account_Information {
private:
  // PRIVATE MEMBERS: These cannot be accessed directly from outside the class
  
  string bank_name;            // Which bank this account belongs to (e.g., "Kakao", "Daegu")
  string account_owner_name;   // Name of the person who owns this account
  string account_number;       // Unique account identifier (e.g., "100-100-111111")
  long long balance;           // Current money in account (using long long for large amounts)
  string debit_card_number;    // Card number used at ATM (e.g., "1111-1111-1111")
  string account_password;     // Secret password to access account (stored in plain text - not secure in real systems!)

public:
  /*
   * CONSTRUCTOR: Bank_Account_Information (with parameters)
   * PURPOSE: Creates a new account with all details specified
   * 
   * CALLED BY: main.cpp when loading accounts from file
   *            Bank.cpp when adding new accounts
   * 
   * PARAMETERS: All the information needed to create a complete account
   */
  Bank_Account_Information(string bank_name_param, string owner_name,
                           string account_num, long long balance_amount,
                           string card_number, string password) {

    // Initialize all private member variables with provided values
    bank_name = bank_name_param;
    account_owner_name = owner_name;
    account_number = account_num;
    balance = balance_amount;
    debit_card_number = card_number;
    account_password = password;

  }

  /*
   * CONSTRUCTOR: Bank_Account_Information (default/empty)
   * PURPOSE: Creates an empty account with default values
   * 
   * USAGE: Used when you need to declare an account variable before initializing it
   */
  Bank_Account_Information() {

    // Set all fields to empty/zero values
    bank_name = "";
    account_owner_name = "";
    account_number = "";
    balance = 0;
    debit_card_number = "";
    account_password = "";

  }

  /*
   * GETTER METHODS: These allow read-only access to private member variables
   * They are marked 'const' because they don't modify the object
   */
  
  string get_bank_name() const { return bank_name; }

  string get_account_owner_name() const { return account_owner_name; }

  string get_account_number() const { return account_number; }

  long long get_balance() const { return balance; }

  string get_debit_card_number() const { return debit_card_number; }

  /*
   * SETTER METHOD: set_balance
   * PURPOSE: Replace the current balance with a new value
   * USAGE: Rarely used; add_to_balance and subtract_from_balance are preferred
   */
  void set_balance(long long new_balance) { balance = new_balance; }

  /*
   * METHOD: add_to_balance
   * PURPOSE: Increases account balance by specified amount
   * 
   * CALLED BY: 
   *   - ATM_CashDeposit.cpp: When customer deposits cash
   *   - ATM_CheckDeposit.cpp: When customer deposits checks
   *   - ATM_Transfer.cpp: When receiving a transfer from another account
   */
  void add_to_balance(long long amount) { balance += amount; }

  /*
   * METHOD: subtract_from_balance
   * PURPOSE: Decreases account balance by specified amount
   * 
   * CALLED BY:
   *   - ATM_Withdrawal.cpp: When customer withdraws cash
   *   - ATM_Transfer.cpp: When sending money to another account
   * 
   * NOTE: This method doesn't check if balance is sufficient - caller must verify!
   */
  void subtract_from_balance(long long amount) { balance -= amount; }

  /*
   * METHOD: verify_password
   * PURPOSE: Checks if provided password matches the account's password
   * 
   * CALLED BY: Bank.cpp verify_password_for_card_number method
   * 
   * RETURNS: true if password matches, false otherwise
   * 
   * SECURITY NOTE: In real systems, passwords should be hashed, not stored/compared as plain text!
   */
  bool verify_password(string password_to_check) const {
    return account_password == password_to_check;
  }
};

/*
 * STRUCT: Transaction_History_Record
 * PURPOSE: Records information about a single transaction for logging/history
 * 
 * IMPORTANT CONCEPT - STATIC VARIABLE:
 *   'static int next_transaction_id' is shared across ALL Transaction_History_Record objects.
 *   This ensures each transaction gets a unique, incrementing ID.
 *   - Defined here (declared)
 *   - Initialized in ATM_Core.cpp (defined)
 * 
 * USAGE: Every time a transaction occurs (deposit, withdrawal, transfer),
 *        a new Transaction_History_Record is created and stored in ATM's transaction vectors.
 * 
 * CONNECTS TO: ATM class stores vectors of these records to track transaction history
 */
struct Transaction_History_Record {

  static int next_transaction_id;  // Shared counter across all records - auto-increments
  int transaction_id;              // Unique ID for this specific transaction
  string card_number;              // Which card performed the transaction (or "CASH" for cash transfers)
  string description;              // Description of what happened (e.g., "Deposit 50000")

  /*
   * CONSTRUCTOR: Creates a new transaction record
   * 
   * AUTOMATIC ID ASSIGNMENT: Uses and increments the static counter
   * so each transaction gets the next available ID (1, 2, 3, ...)
   */
  Transaction_History_Record(string card_num, string desc) {
    
    transaction_id = next_transaction_id++;  // Assign current ID, then increment for next transaction
    card_number = card_num;
    description = desc;
    
  }
};

#endif
