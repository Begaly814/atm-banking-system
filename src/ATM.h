// Written by: Munkhtemuulen (class design and session)
// Written by: Moosa (deposit and withdrawal)
// Written by: Begaly (transfer)

/*
 * ========================================================================
 * FILE: ATM.h
 * PURPOSE: Declares the ATM class - the heart of the system
 * 
 * WHAT THIS FILE CONTAINS:
 * The ATM class represents a physical ATM machine. Each ATM has:
 *   - Cash inventory (physical bills)
 *   - Configuration (which bank, serial number, type, language)
 *   - Transaction history
 *   - Methods to perform banking operations
 * 
 * IMPLEMENTATION FILES (method implementations split across multiple files):
 *   - ATM_Core.cpp: Constructor, cash calculations, transaction recording
 *   - ATM_Session.cpp: User session management and menu system
 *   - ATM_CashDeposit.cpp: Cash deposit functionality
 *   - ATM_CheckDeposit.cpp: Check deposit functionality
 *   - ATM_Withdrawal.cpp: Cash withdrawal with bill dispensing logic
 *   - ATM_Transfer.cpp: Money transfer between accounts
 * 
 * CONNECTS TO:
 *   - Bank.h: ATM searches banks for accounts
 *   - DataStructures.h: Uses Cash_Denomination, Bank_Account_Information, Transaction_History_Record
 *   - Exceptions.h: Throws exceptions for error handling
 *   - main.cpp: Creates ATM objects and calls start_user_session_and_show_menu
 * ========================================================================
 */

#ifndef ATM_H
#define ATM_H

#include "Bank.h"               // For Bank class and account searches
#include "DataStructures.h"     // For Cash_Denomination, Transaction_History_Record
#include <string>
#include <vector>

/*
 * CLASS: ATM
 * PURPOSE: Represents a physical ATM machine with all its capabilities
 * 
 * KEY RESPONSIBILITIES:
 *   1. Store cash inventory (physical bills in 4 denominations)
 *   2. Manage user sessions (login, menu, operations)
 *   3. Perform banking operations (deposit, withdrawal, transfer)
 *   4. Track transaction history
 *   5. Apply appropriate fees based on bank relationships
 */
class ATM {
private:
  // ========== ATM CONFIGURATION ==========
  
  /*
   * primary_bank: The bank that owns/operates this ATM
   * 
   * USAGE: Determines transaction fees
   *   - Same bank (primary): Lower or no fees
   *   - Different bank: Higher fees
   */
  string primary_bank;
  
  /*
   * serial_number: Unique identifier for this ATM (e.g., "000111")
   * USAGE: User enters this to select which ATM to use
   */
  string serial_number;
  
  /*
   * atm_type: Either "Single" or "Multi"
   *   - Single: Only accepts cards from primary bank
   *   - Multi: Accepts cards from all banks (with fees)
   */
  string atm_type;
  
  /*
   * language_mode: Either "Unilingual" (English only) or "Bilingual" (English/Korean)
   */
  string language_mode;

  // ========== CASH INVENTORY ==========
  
  /*
   * cash_inventory: Array of 4 Cash_Denomination structures
   * 
   * STRUCTURE:
   *   [0]: 50,000 won bills
   *   [1]: 10,000 won bills
   *   [2]:  5,000 won bills
   *   [3]:  1,000 won bills
   * 
   * USAGE:
   *   - Withdrawals: Decrease bill counts
   *   - Deposits: Increase bill counts
   *   - Must check availability before dispensing
   */
  Cash_Denomination cash_inventory[4];

  // ========== TRANSACTION TRACKING ==========
  
  /*
   * transactions: Complete history of all transactions ever performed on this ATM
   * 
   * PERSISTS across user sessions - grows continuously
   */
  vector<Transaction_History_Record> transactions;
  
  /*
   * session_transactions: Transactions from current user session only
   * 
   * CLEARED after each session ends - shown in session summary
   */
  vector<Transaction_History_Record> session_transactions;

public:
  // ========== CONSTRUCTORS ==========
  
  /*
   * CONSTRUCTOR: ATM (with parameters)
   * PURPOSE: Creates a new ATM with specified configuration and cash
   * 
   * PARAMETERS:
   *   bank_name: Which bank owns this ATM
   *   serial_num: Unique identifier
   *   type: "Single" or "Multi"
   *   lang_mode: "Unilingual" or "Bilingual"
   *   count_50k, count_10k, count_5k, count_1k: Initial cash inventory
   * 
   * CALLED BY: main.cpp when loading ATMs from file
   * IMPLEMENTATION: ATM_Core.cpp
   */
  ATM(string bank_name, string serial_num, string type, string lang_mode,
      int count_50k, int count_10k, int count_5k, int count_1k);
  
  /*
   * CONSTRUCTOR: ATM (default)
   * PURPOSE: Creates an empty ATM
   * USAGE: Rarely used; mainly for declaring ATM variables before initialization
   */
  ATM() {}

  // ========== GETTER METHODS ==========
  
  /*
   * All getters are const (don't modify the ATM object)
   * and provide read-only access to private configuration
   */
  
  string get_primary_bank() const { return primary_bank; }

  string get_serial_number() const { return serial_number; }

  string get_atm_type() const { return atm_type; }

  string get_language_mode() const { return language_mode; }

  // ========== SESSION MANAGEMENT ==========
  
  /*
   * METHOD: start_user_session_and_show_menu
   * PURPOSE: Main entry point for user interaction - handles complete ATM session
   * 
   * WHAT IT DOES (in order):
   *   1. Language selection (if bilingual)
   *   2. Card insertion (find account)
   *   3. Password verification (3 attempts max)
   *   4. Menu loop (balance, deposit, withdraw, transfer, exit)
   *   5. Session summary (show all transactions)
   * 
   * PARAMETER: banks - Reference to vector of all banks (to search for accounts)
   *            Using '&' (reference) avoids copying the entire vector
   * 
   * SPECIAL FEATURES:
   *   - Admin mode: Enter "ADMIN" as card number to save transaction history
   *   - Withdrawal limit: Max 3 withdrawals per session
   * 
   * CALLED BY: main.cpp when user selects an ATM
   * IMPLEMENTATION: ATM_Session.cpp
   */
  void start_user_session_and_show_menu(vector<Bank> &banks);

  // ========== TRANSACTION RECORDING ==========
  
  /*
   * METHOD: record_transaction
   * PURPOSE: Logs a transaction to history
   * 
   * PARAMETERS:
   *   card_number: Which card performed the transaction (or "CASH" for cash transfers)
   *   description: What happened (e.g., "Deposit 50000", "Withdraw 100000")
   * 
   * WHAT IT DOES:
   *   1. Creates a Transaction_History_Record with auto-generated ID
   *   2. Adds to 'transactions' (permanent history)
   *   3. Adds to 'session_transactions' (current session only)
   * 
   * CALLED BY: All transaction methods after successful completion
   * IMPLEMENTATION: ATM_Core.cpp
   */
  void record_transaction(string card_number, string description);
  
  /*
   * METHOD: calculate_total_cash
   * PURPOSE: Calculates total amount of cash currently in ATM
   * 
   * CALCULATION: Sum of (bill_value × number_of_bills) for all 4 denominations
   * 
   * RETURNS: Total cash in won
   * 
   * USAGE: Displayed in system snapshot for debugging/monitoring
   * IMPLEMENTATION: ATM_Core.cpp
   */
  long long calculate_total_cash();
  
  /*
   * METHOD: save_history_to_file
   * PURPOSE: Saves all transactions to "transaction_history.txt" and displays them
   * 
   * TRIGGERED BY: Entering "ADMIN" as card number in session
   * 
   * OUTPUT: Console display + file with all transaction IDs, card numbers, descriptions
   * IMPLEMENTATION: ATM_Core.cpp
   */
  void save_history_to_file();

  // ========== BANKING OPERATIONS ==========
  
  /*
   * Each operation method receives:
   *   - account: Pointer to user's account (to modify balance)
   *   - is_primary_bank: true if user's bank = ATM's primary bank (affects fees)
   *   - is_korean: true if user selected Korean language (for messages)
   * 
   * All methods handle:
   *   - User input
   *   - Validation
   *   - Fee calculation
   *   - Balance updates
   *   - Transaction recording
   *   - Error handling (try-catch blocks)
   */
  
  /*
   * METHOD: perform_cash_deposit
   * PURPOSE: Allows user to deposit physical cash bills
   * 
   * PROCESS:
   *   1. Display fee (0 for primary bank, 1000 for others)
   *   2. Get bill counts from user (50k, 10k, 5k, 1k)
   *   3. Validate: Max 50 bills total
   *   4. Collect fee if applicable
   *   5. Add cash to account balance
   *   6. Add bills to ATM inventory
   *   7. Record transaction
   * 
   * IMPLEMENTATION: ATM_CashDeposit.cpp
   */
  void perform_cash_deposit(Bank_Account_Information *account,
                            bool is_primary_bank, bool is_korean);
  
  /*
   * METHOD: perform_check_deposit
   * PURPOSE: Allows user to deposit checks
   * 
   * PROCESS:
   *   1. Display fee (0 for primary bank, 1000 for others)
   *   2. Get number of checks (max 30)
   *   3. Get total check amount (min 100,000 won)
   *   4. Collect fee if applicable
   *   5. Add amount to account balance
   *   6. Record transaction
   * 
   * NOTE: Doesn't affect ATM cash inventory (checks aren't cash)
   * IMPLEMENTATION: ATM_CheckDeposit.cpp
   */
  void perform_check_deposit(Bank_Account_Information *account,
                             bool is_primary_bank, bool is_korean);
  
  /*
   * METHOD: perform_withdrawal
   * PURPOSE: Dispenses cash to user and deducts from account
   * 
   * PROCESS:
   *   1. Get withdrawal amount (max 500,000 per transaction)
   *   2. Calculate fee (1000 for primary bank, 2000 for others)
   *   3. Check if account has enough balance
   *   4. Calculate which bills to dispense (greedy algorithm: largest bills first)
   *   5. Verify ATM has enough bills to dispense exact amount
   *   6. Deduct (amount + fee) from account
   *   7. Remove bills from ATM inventory
   *   8. Show bill breakdown to user
   *   9. Record transaction
   * 
   * SPECIAL LOGIC: Bill dispensing algorithm tries to use largest bills first
   * IMPLEMENTATION: ATM_Withdrawal.cpp
   */
  void perform_withdrawal(Bank_Account_Information *account,
                          bool is_primary_bank, bool is_korean);
  
  /*
   * METHOD: perform_transfer
   * PURPOSE: Transfers money to another account (cash or account-to-account)
   * 
   * TWO MODES:
   *   1. Cash Transfer: User inserts physical cash to send
   *   2. Account Transfer: Deduct from user's account
   * 
   * PROCESS:
   *   1. Choose transfer type (cash or account)
   *   2. Enter recipient's account number
   *   3. Search all banks for recipient
   *   4. Calculate fee based on transfer type and bank relationships
   *   5. Verify sufficient funds/cash
   *   6. Deduct from sender, add to recipient
   *   7. Record transaction
   * 
   * FEE STRUCTURE:
   *   Cash transfer: 2000 won
   *   Account transfer: 1000-4000 depending on banks
   * 
   * PARAMETER: banks - Need access to all banks to find recipient
   * IMPLEMENTATION: ATM_Transfer.cpp
   */
  void perform_transfer(Bank_Account_Information *account, vector<Bank> &banks,
                        bool is_primary_bank, bool is_korean);
};

#endif
