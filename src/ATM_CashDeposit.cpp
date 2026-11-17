// Written by: Moosa

/*
 * ========================================================================
 * FILE: ATM_CashDeposit.cpp
 * PURPOSE: Implements cash deposit functionality for ATM
 * 
 * WHAT THIS FILE DOES:
 * Allows users to deposit physical cash bills into their account.
 * Process includes:
 *   1. Calculate fee based on bank relationship
 *   2. Get bill counts from user
 *   3. Validate total bills (max 50)
 *   4. Collect fee if applicable
 *   5. Add cash to account balance
 *   6. Add bills to ATM inventory
 *   7. Record transaction
 * 
 * CONNECTS TO:
 *   - ATM.h: Implements perform_cash_deposit method
 *   - Bank_Account_Information: Modifies account balance
 *   - Exceptions.h: Throws Invalid_Input_Exception for validation errors
 * ========================================================================
 */

#include "ATM.h"
#include "Exceptions.h"
#include <iostream>

using namespace std;

/*
 * METHOD: perform_cash_deposit
 * PURPOSE: Handles the complete cash deposit process
 * 
 * PARAMETERS:
 *   account: Pointer to user's account (to add money)
 *   is_primary_bank: true if user's bank = ATM's primary bank
 *   is_korean: true if user selected Korean language
 * 
 * FEE STRUCTURE:
 *   Primary bank: 0 won (free for bank's own customers)
 *   Other banks: 1,000 won
 * 
 * CONSTRAINTS:
 *   - Maximum 50 bills total (hardware limitation)
 *   - Accepts: 50,000 / 10,000 / 5,000 / 1,000 won bills
 * 
 * ERROR HANDLING:
 *   - Too many bills: Throws Invalid_Input_Exception
 *   - Insufficient fee payment: Throws Invalid_Input_Exception
 * 
 * CALLED BY: ATM_Session.cpp when user selects deposit option
 */
void ATM::perform_cash_deposit(Bank_Account_Information *account,
                               bool is_primary_bank, bool is_korean) {
  // Display operation header
  if (is_korean) {
    cout << "--- 현금 입금 (CASH DEPOSIT) ---" << endl;
  } else {
    cout << "--- CASH DEPOSIT ---" << endl;
  }

  /*
   * Entire operation wrapped in try-catch to handle validation errors
   * gracefully without crashing the program
   */
  try {
    /*
     * STEP 1: CALCULATE AND DISPLAY FEE
     * ===================================
     * 
     * Fee depends on relationship between user's bank and ATM's bank:
     *   - Same bank (primary): No fee (encourage own customers)
     *   - Different bank: 1,000 won fee
     */
    int fee;
    if (is_primary_bank) {
      fee = 0;  // No fee for primary bank customers
    } else {
      fee = 1000;  // Fee for other bank customers
    }

    if (is_korean) {
      cout << "수수료: " << fee << " won" << endl;
    } else {
      cout << "Deposit Fee: " << fee << " won" << endl;
    }

    /*
     * STEP 2: GET BILL COUNTS FROM USER
     * ==================================
     * 
     * Ask user how many bills of each denomination they want to deposit.
     * User enters 0 for first prompt to cancel operation.
     */
    int count_50k_bills;
    int count_10k_bills;
    int count_5k_bills;
    int count_1k_bills;

    // Get 50,000 won bill count (with cancel option)
    if (is_korean) {
      cout << "50,000 won (0 to cancel): ";
    } else {
      cout << "Enter 50,000 bills (0 to cancel): ";
    }
    cin >> count_50k_bills;

    // Allow cancellation with -1 input
    if (count_50k_bills == -1) {
      return;  // Exit without depositing
    }

    // Get 10,000 won bill count
    if (is_korean) {
      cout << "10,000 won: ";
    } else {
      cout << "Enter 10,000 bills: ";
    }
    cin >> count_10k_bills;

    // Get 5,000 won bill count
    if (is_korean) {
      cout << "5,000 won: ";
    } else {
      cout << "Enter 5,000 bills: ";
    }
    cin >> count_5k_bills;

    // Get 1,000 won bill count
    if (is_korean) {
      cout << "1,000 won: ";
    } else {
      cout << "Enter 1,000 bills: ";
    }
    cin >> count_1k_bills;

    /*
     * STEP 3: VALIDATE TOTAL BILL COUNT
     * ==================================
     * 
     * ATM hardware has physical limit on bills it can accept at once.
     * Maximum 50 bills per deposit transaction.
     * 
     * If exceeded, throw exception (caught at bottom of method)
     */
    int total_bills =
        count_50k_bills + count_10k_bills + count_5k_bills + count_1k_bills;

    int max_bills = 50;
    if (total_bills > max_bills) {
      // Too many bills - throw exception
      throw Invalid_Input_Exception("Maximum 50 bills allowed");
    }

    /*
     * STEP 4: CALCULATE TOTAL DEPOSIT AMOUNT
     * =======================================
     * 
     * For each denomination, multiply count × value to get amount.
     * Sum all denominations for total deposit.
     * 
     * USING long long: Prevents overflow for large amounts
     */
    int val_50k = 50000;
    int val_10k = 10000;
    int val_5k = 5000;
    int val_1k = 1000;

    // Calculate amount for each denomination (cast to long long for safety)
    long long amt_50k = (long long)count_50k_bills * val_50k;
    long long amt_10k = (long long)count_10k_bills * val_10k;
    long long amt_5k = (long long)count_5k_bills * val_5k;
    long long amt_1k = (long long)count_1k_bills * val_1k;

    // Sum all denominations
    long long total_amount = amt_50k + amt_10k + amt_5k + amt_1k;

    // Handle zero deposit (user entered all zeros = cancel)
    if (total_amount == 0) {
      if (is_korean) {
        cout << "취소했습니다." << endl;
      } else {
        cout << "Cancelled." << endl;
      }
      return;  // Exit without depositing
    }

    /*
     * STEP 5: COLLECT FEE (if applicable)
     * ====================================
     * 
     * If there's a fee (non-primary bank), user must insert fee separately.
     * Fee is NOT deducted from deposit - user pays extra.
     */
    if (fee > 0) {
      if (is_korean) {
        cout << "수수료를 내어 주세요 (" << fee << "): ";
      } else {
        cout << "Insert fee (" << fee << "): ";
      }

      int fee_input;
      cin >> fee_input;

      // Validate fee payment
      if (fee_input < fee) {
        // User didn't provide enough for fee
        throw Invalid_Input_Exception("Insufficient fee provided");
      }
      // If fee_input > fee, extra is kept by ATM (no change given in this implementation)
    }

    /*
     * STEP 6: UPDATE ACCOUNT BALANCE
     * ===============================
     * 
     * Add the total deposit amount to user's account.
     * This immediately increases their balance.
     */
    account->add_to_balance(total_amount);

    /*
     * STEP 7: UPDATE ATM CASH INVENTORY
     * ==================================
     * 
     * Add deposited bills to ATM's inventory.
     * These bills are now available for withdrawal by other users.
     * 
     * ARRAY INDICES:
     *   [0] = 50,000 won bills
     *   [1] = 10,000 won bills
     *   [2] = 5,000 won bills
     *   [3] = 1,000 won bills
     */
    cash_inventory[0].number_of_bills += count_50k_bills;
    cash_inventory[1].number_of_bills += count_10k_bills;
    cash_inventory[2].number_of_bills += count_5k_bills;
    cash_inventory[3].number_of_bills += count_1k_bills;

    /*
     * STEP 8: DISPLAY SUCCESS MESSAGE
     * ================================
     * 
     * Show user the deposit was successful,
     * display amount deposited and new balance.
     */
    long long new_bal = account->get_balance();
    if (is_korean) {
      cout << "입금 성공. 합계: " << total_amount << " won" << endl;
      cout << "현재 잔액: " << new_bal << " won" << endl;
    } else {
      cout << "Deposit Successful. Total: " << total_amount << endl;
      cout << "Current Balance: " << new_bal << endl;
    }

    /*
     * STEP 9: RECORD TRANSACTION
     * ===========================
     * 
     * Add this deposit to transaction history.
     * Creates description like "Deposit 50000"
     * Records card number and description in transaction log.
     */
    string desc_prefix = "Deposit ";
    string amt_str = to_string(total_amount);
    string desc = desc_prefix + amt_str;
    record_transaction(account->get_debit_card_number(), desc);

  } 
  /*
   * EXCEPTION HANDLING
   * ==================
   * 
   * Catch validation errors and display appropriate messages.
   * Operation fails gracefully without crashing program.
   */
  catch (const Invalid_Input_Exception &e) {
    // Handle validation errors (too many bills or insufficient fee)
    if (is_korean) {
      // Check error message to determine which error occurred
      if (string(e.what()).find("50") != string::npos) {
        cout << "너무 많은 지폐 (Max 50)." << endl;
      } else {
        cout << "수수료 부족." << endl;
      }
    } else {
      cout << "Error: " << e.what() << endl;
    }
  } 
  catch (const exception &e) {
    // Catch any other unexpected errors
    cout << "Error: " << e.what() << endl;
  }
  // Method ends, returns to menu in ATM_Session.cpp
}
