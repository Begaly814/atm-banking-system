// Written by: Munkhtemuulen

/*
 * ========================================================================
 * FILE: ATM_Session.cpp
 * PURPOSE: Implements user session management - the "main menu" of the ATM
 * 
 * This file contains the start_user_session_and_show_menu method, which:
 *   1. Handles language selection
 *   2. Gets and validates card number
 *   3. Verifies password (with 3 attempts)
 *   4. Displays menu and processes user choices
 *   5. Shows session summary at end
 * 
 * CONNECTS TO:
 *   - ATM.h: Implements the main session method
 *   - Bank classes: Searches for user's account
 *   - Other ATM_*.cpp files: Calls specific transaction methods
 * ========================================================================
 */

#include "ATM.h"
#include <iostream>

using namespace std;

/*
 * METHOD: start_user_session_and_show_menu
 * PURPOSE: Orchestrates entire user interaction from card insertion to session end
 * 
 * PARAMETER: banks - Reference to vector of all banks (to search for accounts)
 * 
 * MAJOR SECTIONS:
 *   1. Language Selection (if bilingual ATM)
 *   2. Card Insertion & Account Search
 *   3. ATM Type Validation (Single vs Multi bank)
 *   4. Password Verification (3 attempts max)
 *   5. Menu Loop (transactions)
 *   6. Session Summary
 * 
 * CALLED BY: main.cpp after user selects an ATM
 * 
 * FLOW DIAGRAM:
 *   Start → Language → Card → Bank Check → Password → Menu Loop → Summary → End
 */
void ATM::start_user_session_and_show_menu(vector<Bank> &banks) {
  
  /*
   * ================================================
   * SECTION 1: LANGUAGE SELECTION
   * ================================================
   * 
   * If ATM is bilingual, ask user to choose language.
   * If unilingual, default to English.
   */
  string currentLang = "English";  // Default language
  string lang_mode = get_language_mode();  // Get ATM's language mode

  if(lang_mode == "Bilingual") {
    // This ATM supports both languages - let user choose
    cout << "Select Language (1: English, 2: Korean): ";
    int langChoice;
    cin >> langChoice;
    if(langChoice == 2)
      currentLang = "Korean";
    // Any other input defaults to English
  }

  // Store as boolean for easier checking throughout session
  bool is_korean = (currentLang == "Korean");

  /*
   * ================================================
   * SECTION 2: CARD INSERTION & ACCOUNT SEARCH
   * ================================================
   * 
   * Welcome user and prompt for card number.
   * Search all banks to find account with matching card number.
   */
  
  // Welcome message in selected language
  if(is_korean)
    cout << "어서 오세요 (Welcome). ";
  else
    cout << "Welcome. ";

  // Prompt for card number
  if(is_korean)
    cout << "체크카드를 넣어 주세요 (Enter Card Number): ";
  else
    cout << "Please insert your debit card (Enter Card Number): ";

  string cardInput;
  cin >> cardInput;

  /*
   * Search for account across all banks
   * 
   * WHY SEARCH ALL BANKS?
   * User might have account at different bank than ATM's primary bank.
   * Multi-bank ATMs support cards from any bank (with fees).
   * 
   * POINTERS:
   *   userAccount: Points to the user's account object (to modify balance)
   *   userBank: Points to the bank that owns the account (to check primary bank)
   */
  Bank_Account_Information *userAccount = NULL;  // Will point to user's account if found
  Bank *userBank = NULL;  // Will point to user's bank if found

  // Loop through all banks looking for matching card number
  for(int i = 0; i < banks.size(); i++) {
    Bank_Account_Information *foundAcc =
        banks[i].find_account_by_debit_card_number(cardInput);

    if(foundAcc != NULL) {
      // Found the account!
      userAccount = foundAcc;  // Store pointer to account
      userBank = &banks[i];    // Store pointer to bank that contains this account
      break;  // Stop searching
    } 
    else {
      // Not in this bank, check next bank
      continue;
    }
  }

  /*
   * Handle account not found
   * 
   * SPECIAL CASE: "ADMIN" card triggers transaction history save
   * This is a backdoor for administrators to view/save transaction logs
   */
  if(userAccount == NULL) {
    string adminCard = "ADMIN";
    if(cardInput == adminCard) {
      // Special admin command - save and display all transactions
      save_history_to_file();
      return;  // End session
    }
    // Invalid card number - no account found
    cout << (is_korean ? "잘못된 카드입니다." : "Invalid Card.") << endl;
    return;  // End session
  }

  /*
   * ================================================
   * SECTION 3: ATM TYPE VALIDATION
   * ================================================
   * 
   * Check if ATM type allows this user's bank:
   *   - Single-bank ATM: Only accepts primary bank cards
   *   - Multi-bank ATM: Accepts all bank cards (with fees)
   */
  
  // Check if user's bank matches ATM's primary bank
  bool isPrimaryBank = (userBank->get_name() == get_primary_bank());
  string atmType = get_atm_type();

  if(atmType == "Single" && !isPrimaryBank) {
    // This ATM only serves primary bank customers
    string primaryBankName = get_primary_bank();
    if(is_korean)
      cout << "이 ATM은 " << primaryBankName << " 만 지원합니다." << endl;
    else
      cout << "This ATM only supports " << primaryBankName << "." << endl;
    return;  // Reject card, end session
  }
  // If we reach here: either Multi-bank ATM, or Single-bank with matching bank

  /*
   * ================================================
   * SECTION 4: PASSWORD VERIFICATION
   * ================================================
   * 
   * User gets 3 attempts to enter correct password.
   * After 3 failed attempts, session is aborted.
   * 
   * SECURITY NOTE: In real systems, card would be retained or locked
   * after failed attempts to prevent brute force attacks.
   */
  
  int pwdFails = 0;          // Count of failed password attempts
  int maxPwdAttempts = 3;    // Maximum allowed attempts

  // Loop until correct password or max attempts reached
  while(true) {
    // Prompt for password
    cout << (is_korean ? "비밀번호를 입력해 주세요: "
                       : "Enter Password: ");
    string pwd_input;
    cin >> pwd_input;

    // Verify password using account's method
    bool pwdCorrect = userAccount->verify_password(pwd_input);
    if(pwdCorrect) {
      // Password correct - proceed to menu
      break;  // Exit password loop
    } 
    else {
      // Password wrong
      cout << (is_korean ? "비밀번호가 틀렸습니다." : "Wrong Password.")
           << endl;
      pwdFails++;
      
      if(pwdFails >= maxPwdAttempts) {
        // Too many failed attempts - abort session
        cout << (is_korean ? "3회 오류. 세션 종료."
                           : "3 wrong attempts. Session aborted.")
             << endl;
        return;  // End session
      }
      // Otherwise, loop continues for another attempt
    }
  }
  // If we reach here: Password verified successfully

  /*
   * ================================================
   * SECTION 5: MENU LOOP (Main Transaction Area)
   * ================================================
   * 
   * User authenticated successfully - now show menu and process transactions.
   * Loop continues until user chooses to exit (option 6).
   * 
   * MENU OPTIONS:
   *   1. View Balance - Just displays current balance
   *   2. Cash Deposit - Insert physical bills
   *   3. Check Deposit - Deposit checks
   *   4. Withdrawal - Withdraw cash (max 3 per session)
   *   5. Transfer - Send money to another account
   *   6. Exit - End session
   * 
   * WITHDRAWAL LIMIT:
   * To prevent emptying ATM cash, limit 3 withdrawals per session.
   * User can still deposit, transfer, etc. unlimited times.
   */
  
  int withdrawalCnt = 0;       // Track withdrawals in this session
  int maxWithdrawalCnt = 3;    // Maximum withdrawals allowed per session

  // Main menu loop
  while(true) {
    // Display menu header
    cout << "\n=== MENU ===" << endl;

    // Display menu options in selected language
    if(is_korean) {
      cout << "1. 잔액 조회 (View Balance)" << endl;
      cout << "2. 현금 입금 (Cash Deposit)" << endl;
      cout << "3. 수표 입금 (Check Deposit)" << endl;
      cout << "4. 출금 (Withdrawal)" << endl;
      cout << "5. 이체 (Transfer)" << endl;
      cout << "6. 종료 (Exit)" << endl;
      cout << "선택: ";
    } 
    else {
      cout << "1. View Balance" << endl;
      cout << "2. Cash Deposit" << endl;
      cout << "3. Check Deposit" << endl;
      cout << "4. Withdrawal" << endl;
      cout << "5. Transfer" << endl;
      cout << "6. Exit" << endl;
      cout << "Choose: ";
    }

    // Get user's choice
    int menuChoice;
    cin >> menuChoice;

    /*
     * OPTION 1: VIEW BALANCE
     * Simple query - just display current balance
     * No transaction recorded (just viewing, not modifying)
     */
    if(menuChoice == 1) {
      long long bal = userAccount->get_balance();
      if(is_korean)
        cout << "잔액: " << bal << " won" << endl;
      else
        cout << "Current Balance: " << bal << " won" << endl;
    } 
    
    /*
     * OPTION 2: CASH DEPOSIT
     * Calls perform_cash_deposit method (implemented in ATM_CashDeposit.cpp)
     * Passes account pointer, primary bank status, and language preference
     */
    else if(menuChoice == 2) {
      perform_cash_deposit(userAccount, isPrimaryBank, is_korean);
    } 
    
    /*
     * OPTION 3: CHECK DEPOSIT
     * Calls perform_check_deposit method (implemented in ATM_CheckDeposit.cpp)
     */
    else if(menuChoice == 3) {
      perform_check_deposit(userAccount, isPrimaryBank, is_korean);
    } 
    
    /*
     * OPTION 4: WITHDRAWAL
     * Calls perform_withdrawal method (implemented in ATM_Withdrawal.cpp)
     * 
     * SPECIAL HANDLING: Enforces 3-withdrawal limit per session
     * If limit reached, display message and don't allow withdrawal
     */
    else if(menuChoice == 4) {
      if(withdrawalCnt >= maxWithdrawalCnt) {
        // Already withdrew 3 times this session
        cout << (is_korean ? "1회 3분만 출금 가능."
                           : "Max 3 withdrawals per session.")
             << endl;
      } else {
        // Still have withdrawal attempts remaining
        perform_withdrawal(userAccount, isPrimaryBank, is_korean);
        withdrawalCnt++;  // Increment counter (even if withdrawal fails)
        int remaining = maxWithdrawalCnt - withdrawalCnt;  // Calculate remaining (not displayed)
      }
    } 
    
    /*
     * OPTION 5: TRANSFER
     * Calls perform_transfer method (implemented in ATM_Transfer.cpp)
     * Requires banks vector to search for recipient account
     */
    else if(menuChoice == 5) {
      perform_transfer(userAccount, banks, isPrimaryBank, is_korean);
    } 
    
    /*
     * OPTION 6: EXIT
     * Break out of menu loop to show session summary
     */
    else if(menuChoice == 6) {
      break;  // Exit menu loop
    } 
    
    /*
     * INVALID INPUT
     * User entered something other than 1-6
     */
    else {
      cout << (is_korean ? "잘못된 입력." : "Invalid input.") << endl;
    }
    
    // Loop continues until user chooses option 6 (Exit)
  }
  // If we reach here: User chose to exit

  /*
   * ================================================
   * SECTION 6: SESSION SUMMARY
   * ================================================
   * 
   * After user exits menu, display summary of all transactions
   * performed during this session (if any).
   * 
   * WHAT IS session_transactions?
   * It's a vector that stores only the transactions from THIS session.
   * It gets cleared at the end so next session starts fresh.
   * 
   * The main 'transactions' vector (in ATM) keeps ALL transactions
   * forever, but session_transactions is temporary.
   */
  
  int transCnt = session_transactions.size();  // How many transactions in this session?
  
  if(transCnt > 0) {
    // User performed at least one transaction - show summary
    
    // Header
    cout << "\n" << (is_korean ? "=== 세션 요약 (SESSION SUMMARY) ===" 
                                : "=== SESSION SUMMARY ===") << endl;
    
    // Display account information
    cout << (is_korean ? "카드: " : "Card: ") 
         << userAccount->get_debit_card_number() << endl;
    cout << (is_korean ? "계좌: " : "Account: ") 
         << userAccount->get_account_number() << endl;
    cout << (is_korean ? "총 거래 수: " : "Total Transactions: ") 
         << transCnt << endl;
    
    // Display transaction details
    cout << (is_korean ? "\n거래 내역:" : "\nTransaction Details:") << endl;
    
    // Loop through each transaction in this session
    for(int i = 0; i < transCnt; i++) {
      Transaction_History_Record curr = session_transactions[i];
      
      // Display transaction ID and description
      cout << (is_korean ? "  [거래 ID: " : "  [Transaction ID: ") 
           << curr.transaction_id << "] "
           << curr.description << endl;
    }
    
    cout << "====================" << endl;
   
    /*
     * CLEAR SESSION TRANSACTIONS
     * 
     * Very important: Clear the session_transactions vector
     * so the next session starts with an empty list.
     * 
     * The main 'transactions' vector is NOT cleared - it keeps
     * accumulating all transactions for the ADMIN view.
     */
    session_transactions.clear();
  }
  // If transCnt == 0, user didn't perform any transactions
  // (maybe just checked balance and exited), so no summary displayed

  // Farewell message
  cout << (is_korean ? "Session 종료." : "Session Ended.") << endl;
  
  // Method ends, returns to main.cpp's ATM selection loop
}
