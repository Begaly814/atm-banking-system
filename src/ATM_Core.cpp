// Written by: Munkhtemuulen

/*
 * ========================================================================
 * FILE: ATM_Core.cpp
 * PURPOSE: Implements core ATM functionality - construction, cash management, transaction logging
 * 
 * This file contains:
 *   1. Static variable initialization for transaction IDs
 *   2. ATM constructor (sets up a new ATM machine)
 *   3. Cash calculation methods
 *   4. Transaction recording and history management
 * 
 * CONNECTS TO:
 *   - ATM.h: Implements core methods declared there
 *   - DataStructures.h: Works with Cash_Denomination and Transaction_History_Record
 * ========================================================================
 */

#include "ATM.h"
#include <fstream>   // For file output (ofstream)
#include <iostream>  // For console output (cout)

using namespace std;

/*
 * STATIC VARIABLE INITIALIZATION
 * ===============================
 * 
 * WHAT IS THIS?
 * Remember in DataStructures.h, Transaction_History_Record has:
 *   static int next_transaction_id;
 * 
 * That was just a DECLARATION (saying it exists).
 * This is the DEFINITION (actually creating it in memory and setting initial value).
 * 
 * WHY HERE?
 * Static variables must be defined in exactly ONE .cpp file.
 * We chose ATM_Core.cpp since it's the main ATM implementation file.
 * 
 * WHAT IT DOES:
 * Initializes the shared transaction ID counter to 1.
 * The first transaction will get ID 1, next gets 2, etc.
 * This counter is SHARED across all Transaction_History_Record objects.
 */
int Transaction_History_Record::next_transaction_id = 1;

/*
 * CONSTRUCTOR: ATM
 * PURPOSE: Creates and initializes a new ATM machine with configuration and cash
 * 
 * PARAMETERS:
 *   bank_name: Which bank owns this ATM (e.g., "Kakao", "Daegu")
 *   serial_num: Unique identifier (e.g., "000111")
 *   type: "Single" (primary bank only) or "Multi" (all banks)
 *   lang_mode: "Unilingual" (English) or "Bilingual" (English/Korean)
 *   count_50k: Number of 50,000 won bills to start with
 *   count_10k: Number of 10,000 won bills to start with
 *   count_5k: Number of 5,000 won bills to start with
 *   count_1k: Number of 1,000 won bills to start with
 * 
 * CALLED BY: main.cpp when loading ATMs from sample_initial_condition.txt
 * 
 * WHAT IT DOES:
 *   1. Stores ATM configuration (bank, serial number, type, language)
 *   2. Initializes cash_inventory array with 4 denominations
 *   3. Sets up bill values and quantities for each denomination
 * 
 * NOTE: The transactions and session_transactions vectors are automatically
 *       initialized as empty by the vector default constructor
 */
ATM::ATM(string bank_name, string serial_num, string type, string lang_mode,
         int count_50k, int count_10k, int count_5k, int count_1k) {
  
  // Store ATM configuration
  primary_bank = bank_name;
  serial_number = serial_num;
  atm_type = type;
  language_mode = lang_mode;

  /*
   * Initialize cash_inventory array
   * Array has 4 elements: [0]=50k, [1]=10k, [2]=5k, [3]=1k
   * 
   * For each denomination, we set:
   *   - bill_value: The value of each bill (50000, 10000, 5000, or 1000)
   *   - number_of_bills: How many bills of this type we have
   */
  
  // Index 0: 50,000 won bills
  cash_inventory[0].bill_value = 50000;
  cash_inventory[0].number_of_bills = count_50k;
  
  // Index 1: 10,000 won bills
  cash_inventory[1].bill_value = 10000;
  cash_inventory[1].number_of_bills = count_10k;
  
  // Index 2: 5,000 won bills
  cash_inventory[2].bill_value = 5000;
  cash_inventory[2].number_of_bills = count_5k;
  
  // Index 3: 1,000 won bills
  cash_inventory[3].bill_value = 1000;
  cash_inventory[3].number_of_bills = count_1k;
}

/*
 * METHOD: calculate_total_cash
 * PURPOSE: Calculates the total amount of cash currently stored in the ATM
 * 
 * RETURNS: Total cash value in won (as long long to handle large amounts)
 * 
 * ALGORITHM:
 *   For each denomination:
 *     1. Get bill value (50000, 10000, 5000, or 1000)
 *     2. Get number of bills of that type
 *     3. Multiply: value × count = total for this denomination
 *     4. Add to running total
 * 
 * EXAMPLE:
 *   If ATM has:
 *     - 2 × 50,000 won bills = 100,000
 *     - 3 × 10,000 won bills = 30,000
 *     - 1 × 5,000 won bills  = 5,000
 *     - 3 × 1,000 won bills  = 3,000
 *   Total = 138,000 won
 * 
 * CALLED BY: 
 *   - main.cpp: print_system_snapshot_for_debugging() to show ATM cash levels
 * 
 * WHY long long?
 *   Using long long (64-bit integer) ensures we can handle very large amounts
 *   without overflow. Regular int (32-bit) maxes out around 2 billion.
 */
long long ATM::calculate_total_cash() {
  long long totalCash = 0;  // Initialize accumulator to zero
  int numDenoms = 4;        // We have 4 types of bills

  // Loop through each denomination
  for(int i = 0; i < numDenoms; i++) {
    int billVal = cash_inventory[i].bill_value;        // Value of each bill
    int billCnt = cash_inventory[i].number_of_bills;   // How many we have
    
    // Calculate value for this denomination
    // Cast to long long to prevent overflow during multiplication
    long long amt = (long long)billVal * billCnt;
    
    totalCash += amt;  // Add to total
  }
  
  return totalCash;  // Return final sum
}

/*
 * METHOD: record_transaction
 * PURPOSE: Logs a completed transaction to the ATM's history
 * 
 * PARAMETERS:
 *   card_number: Which card performed the transaction (or "CASH" for cash transfers)
 *   description: What happened (e.g., "Deposit 50000", "Withdraw 100000", "Transfer 20000 to 200-200-222222")
 * 
 * WHAT IT DOES:
 *   1. Creates a new Transaction_History_Record object
 *      - This automatically assigns next available transaction ID
 *      - Stores card_number and description
 *   2. Adds record to 'transactions' vector (permanent history for this ATM)
 *   3. Adds record to 'session_transactions' vector (current session only)
 * 
 * WHY TWO VECTORS?
 *   - transactions: Keeps complete history across all sessions (for ADMIN view)
 *   - session_transactions: Only current session (shown at end of session)
 *                           Cleared after each session summary
 * 
 * CALLED BY: All transaction methods after successful completion:
 *   - ATM_CashDeposit.cpp: After deposit completes
 *   - ATM_CheckDeposit.cpp: After deposit completes
 *   - ATM_Withdrawal.cpp: After withdrawal completes
 *   - ATM_Transfer.cpp: After transfer completes
 */
void ATM::record_transaction(string card_number, string description) {
  // Create new transaction record (automatically gets next ID from static counter)
  Transaction_History_Record record(card_number, description);
  
  transactions.push_back(record);          // Add to permanent history
  session_transactions.push_back(record);  // Add to current session history
}

/*
 * METHOD: save_history_to_file
 * PURPOSE: Displays and saves all transaction history to a file
 * 
 * TRIGGERED BY: User entering "ADMIN" as card number in ATM session
 * 
 * WHAT IT DOES:
 *   1. Display all transactions on console
 *   2. Create/overwrite "transaction_history.txt"
 *   3. Write ATM serial number
 *   4. Write total transaction count
 *   5. Write details of each transaction (ID, card, description)
 *   6. Close file
 *   7. Confirm save to user
 * 
 * OUTPUT FORMAT (both console and file):
 *   [Transaction ID: 1] 1111-1111-1111: Deposit 50000
 *   [Transaction ID: 2] 2222-2222-2222: Withdraw 100000
 *   [Transaction ID: 3] CASH: Transfer 20000 to 200-200-222222
 * 
 * FILE OPERATIONS:
 *   - ofstream: Output file stream (writes to file)
 *   - Creates new file if doesn't exist
 *   - Overwrites existing file with new content
 */
void ATM::save_history_to_file() {
  // Display on console first
  cout << "\n=== TRANSACTION HISTORY ===" << endl;

  int totalTrans = transactions.size();  // How many transactions total?
  
  // Loop through and display each transaction
  for(int i = 0; i < totalTrans; i++) {
    int transID = transactions[i].transaction_id;
    string cardNum = transactions[i].card_number;
    string descText = transactions[i].description;
    cout << "[Transaction ID: " << transID << "] "
         << cardNum << ": " << descText << endl;
  }

  /*
   * FILE OUTPUT
   * ===========
   * ofstream: Output File Stream
   *   - Opens file for writing
   *   - Creates file if it doesn't exist
   *   - Overwrites file if it already exists
   *   - Works like cout, but writes to file instead of console
   */
  ofstream fileOut("transaction_history.txt");

  // Write header information
  fileOut << "Transaction History for ATM " << serial_number << endl;
  fileOut << "Total Transactions: " << totalTrans << endl << endl;

  // Write each transaction to file
  for(int i = 0; i < totalTrans; i++) {
    fileOut << "[Transaction ID: " << transactions[i].transaction_id << "] "
            << transactions[i].card_number << ": "
            << transactions[i].description << endl;
  }

  fileOut.close();  // Close file (flushes buffer, releases file handle)
  cout << "History saved to transaction_history.txt" << endl;
}
