// Written by: Munkhtemuulen

/*
 * ========================================================================
 * FILE: main.cpp
 * PURPOSE: Entry point of the ATM Banking System - orchestrates the entire program
 * 
 * PROGRAM FLOW:
 *   1. Load initial data from file (banks, accounts, ATMs)
 *   2. Enter main loop: User selects ATM → ATM session runs → Repeat
 *   3. Special commands: '/' shows system snapshot, "ADMIN" saves history
 * 
 * WHAT THIS FILE DOES:
 *   - Defines global data structures (banks and atms vectors)
 *   - Loads initial configuration from sample_initial_condition.txt
 *   - Provides main interaction loop for ATM selection
 *   - Includes debugging/monitoring functionality (snapshot)
 * 
 * CONNECTS TO:
 *   - ATM.h: Creates and manages ATM objects
 *   - Bank.h: Creates and manages Bank objects
 *   - DataStructures.h: Uses account and transaction structures
 *   - Exceptions.h: Handles file I/O errors
 *   - sample_initial_condition.txt: Reads initial system state
 * ========================================================================
 */

#include "ATM.h"
#include "Bank.h"
#include "DataStructures.h"
#include "Exceptions.h"
#include <fstream>   // For file input (ifstream)
#include <iostream>  // For console I/O
#include <string>
#include <vector>    // For dynamic arrays


using namespace std;

/*
 * GLOBAL VARIABLES
 * ================
 * 
 * WHY GLOBAL?
 * These vectors are accessed by multiple functions and represent the
 * entire system's state. Making them global simplifies access.
 * 
 * ALTERNATIVE: Could pass them as parameters everywhere, but that's
 * more cumbersome for this project size.
 */

/*
 * banks: Vector storing all banks in the system
 * 
 * USAGE: 
 *   - Loaded from file at startup
 *   - Passed to ATM sessions for account searches
 *   - Each Bank contains vector of accounts
 */
vector<Bank> banks;

/*
 * atms: Vector storing all ATM machines in the system
 * 
 * USAGE:
 *   - Loaded from file at startup
 *   - User selects one by serial number
 *   - Selected ATM runs user session
 */
vector<ATM> atms;

/*
 * FUNCTION: print_system_snapshot_for_debugging
 * PURPOSE: Displays current state of all ATMs and accounts
 * 
 * WHEN CALLED: User enters '/' at ATM selection prompt
 * 
 * WHAT IT DISPLAYS:
 *   1. All ATMs with:
 *      - Primary bank name
 *      - Serial number
 *      - Remaining cash amount
 *   2. All accounts in all banks with:
 *      - Bank name
 *      - Account number
 *      - Owner name
 *      - Current balance
 * 
 * USAGE: Useful for monitoring system state during testing/debugging
 */
void print_system_snapshot_for_debugging() {
  cout << "\n=== SNAPSHOT ===" << endl;

  // Display all ATMs
  int numAtms = atms.size();
  for(int i = 0; i < numAtms; i++) {
    string primaryBank = atms[i].get_primary_bank();
    string serialNum = atms[i].get_serial_number();
    long long cashAmt = atms[i].calculate_total_cash();
    
    cout << primaryBank
         << " ATM [SN:" << serialNum << "] ";
    cout << "Remaining cash : " << cashAmt << endl;
  }

  // Display all accounts in all banks
  int numBanks = banks.size();
  for(int i = 0; i < numBanks; i++) {
    banks[i].print_all_accounts();  // Each bank prints its own accounts
  }

  cout << "================\n" << endl;
}

/*
 * FUNCTION: main
 * PURPOSE: Entry point of the program - runs when program starts
 * 
 * RETURN VALUE:
 *   0: Program completed successfully
 *   1: Error occurred (file not found, etc.)
 * 
 * MAIN SECTIONS:
 *   1. FILE LOADING: Read initial data (try-catch block)
 *   2. MAIN LOOP: User selects ATMs and performs transactions (infinite loop)
 */
int main() {
  /*
   * ========================================
   * SECTION 1: FILE LOADING
   * ========================================
   * 
   * This entire section is wrapped in try-catch for error handling.
   * If file can't be opened or data is corrupted, we catch the exception
   * and exit gracefully rather than crashing.
   */
  try {
    /*
     * Open the initial conditions file
     * 
     * FILE FORMAT (sample_initial_condition.txt):
     *   Line 1: <num_banks> <num_accounts> <num_atms>
     *   Next num_banks lines: <bank_name>
     *   Next num_accounts lines: <bank_name> <owner> <account_num> <balance> <card_num> <password>
     *   Next num_atms lines: <primary_bank> <serial> <type> <lang_mode> <50k_count> <10k_count> <5k_count> <1k_count>
     */
    ifstream input_file_for_initial_conditions("sample_initial_condition.txt");

    // Check if file opened successfully
    if(!input_file_for_initial_conditions.is_open()) {
      // File doesn't exist or can't be read - throw exception
      throw File_IO_Exception("Cannot open sample_initial_condition.txt");
    }

    // Read counts from first line
    int total_number_of_banks;
    int total_number_of_accounts;
    int total_number_of_atms;

    input_file_for_initial_conditions >> total_number_of_banks >>
        total_number_of_accounts >> total_number_of_atms;

    /*
     * LOAD BANKS
     * ==========
     * Read each bank name and create Bank objects
     */
    for(int i = 0; i < total_number_of_banks; i++) {
      string bankName;
      input_file_for_initial_conditions >> bankName;  // Read bank name (e.g., "Kakao")
      
      Bank newBank = Bank(bankName);  // Create new Bank object
      banks.push_back(newBank);       // Add to global banks vector
    }

    /*
     * LOAD ACCOUNTS
     * =============
     * Read each account's information and add to appropriate bank
     */
    for(int i = 0; i < total_number_of_accounts; i++) {
      // Read account data
      string bankName;
      string ownerName;
      string accNum;
      string cardNum;
      string pwd;
      long long initBalance;

      input_file_for_initial_conditions >> bankName >>
          ownerName >> accNum >> initBalance >> cardNum >>
          pwd;

      /*
       * Find which bank this account belongs to
       * Loop through banks vector to find matching bank name
       */
      bool foundBank = false;
      for(int j = 0; j < banks.size(); j++) {
        string currBank = banks[j].get_name();
        if(currBank == bankName) {
          // Found the right bank - add account to it
          banks[j].add_new_account_to_bank(Bank_Account_Information(
              bankName, ownerName, accNum,
              initBalance, cardNum, pwd));
          foundBank = true;
          break;  // No need to check other banks
        }
      }
      // Note: If bank not found, account is silently skipped
      // In production code, you might want to log a warning
    }

    /*
     * LOAD ATMS
     * =========
     * Read each ATM's configuration and create ATM objects
     */
    for(int i = 0; i < total_number_of_atms; i++) {
      // Read ATM data
      string primaryBank;
      string serialNum;
      string atmType;
      string langMode;
      int cnt50k;
      int cnt10k;
      int cnt5k;
      int cnt1k;

      input_file_for_initial_conditions >> primaryBank >> serialNum >>
          atmType >> langMode >> cnt50k >>
          cnt10k >> cnt5k >> cnt1k;

      // Create new ATM with all configuration
      ATM newAtm = ATM(primaryBank, serialNum, atmType,
                       langMode, cnt50k, cnt10k,
                       cnt5k, cnt1k);
      atms.push_back(newAtm);  // Add to global atms vector
    }

    // Done reading file - close it
    input_file_for_initial_conditions.close();

  } 
  /*
   * EXCEPTION HANDLING
   * ==================
   * If anything goes wrong during file loading, catch and handle
   */
  catch(const File_IO_Exception &e) {
    // File couldn't be opened or read
    cout << "File Error: " << e.what() << endl;
    return 1;  // Exit with error code
  } 
  catch(const exception &e) {
    // Some other unexpected error
    cout << "Unexpected Error: " << e.what() << endl;
    return 1;  // Exit with error code
  }

  /*
   * ========================================
   * SECTION 2: MAIN INTERACTION LOOP
   * ========================================
   * 
   * This loop runs forever (until program is terminated externally).
   * User repeatedly:
   *   1. Selects an ATM
   *   2. Performs transactions
   *   3. Returns to ATM selection
   */
  while(true) {
    // Prompt user to select an ATM
    cout << "\nSelect an ATM (Enter Serial Number, or '/' for snapshot): ";
    string userInput;
    cin >> userInput;

    /*
     * SPECIAL COMMAND: '/' shows system snapshot
     * Useful for debugging - see all ATMs and accounts
     */
    if(userInput == "/") {
      print_system_snapshot_for_debugging();
      continue;  // Skip rest of loop, go back to selection
    }

    /*
     * SEARCH FOR ATM BY SERIAL NUMBER
     * ================================
     * Loop through atms vector to find matching serial number
     */
    ATM *selectedAtm = NULL;  // Pointer to selected ATM (NULL = not found)
    bool atmFound = false;
    
    for(int i = 0; i < atms.size(); i++) {
      string serialNum = atms[i].get_serial_number();
      if(serialNum == userInput) {
        // Found matching ATM - get pointer to it
        selectedAtm = &atms[i];  // & = address-of operator
        atmFound = true;
        break;  // Stop searching
      }
    }

    /*
     * START ATM SESSION OR SHOW ERROR
     * ================================
     */
    if(atmFound) {
      /*
       * ATM found - start user session
       * This call:
       *   1. Handles language selection
       *   2. Gets and verifies card number
       *   3. Verifies password
       *   4. Shows menu and processes transactions
       *   5. Shows session summary
       *   6. Returns here when user exits
       */
      selectedAtm->start_user_session_and_show_menu(banks);
    } 
    else {
      // User entered invalid serial number
      cout << "ATM not found." << endl;
    }
    
    // Loop continues - user can select another ATM
  }

  /*
   * This return statement is never reached (infinite loop),
   * but included for completeness
   */
  return 0;
}
