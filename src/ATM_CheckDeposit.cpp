// Written by: Moosa

/*
 * ========================================================================
 * FILE: ATM_CheckDeposit.cpp
 * PURPOSE: Implements check deposit functionality for ATM
 * 
 * WHAT THIS FILE DOES:
 * Allows users to deposit checks (paper documents that represent money).
 * Process includes:
 *   1. Calculate fee based on bank relationship
 *   2. Get number of checks and total amount
 *   3. Validate constraints (max 30 checks, min 100,000 won)
 *   4. Collect fee if applicable
 *   5. Add amount to account balance
 *   6. Record transaction
 * 
 * KEY DIFFERENCE FROM CASH DEPOSIT:
 * Checks don't affect ATM's cash inventory (they're paper, not cash).
 * User just enters the amount - ATM trusts it (in reality, checks
 * would be verified later by the bank).
 * 
 * CONNECTS TO:
 *   - ATM.h: Implements perform_check_deposit method
 *   - Bank_Account_Information: Modifies account balance
 *   - Exceptions.h: Throws Invalid_Input_Exception for validation errors
 * ========================================================================
 */

#include "ATM.h"
#include "Exceptions.h"
#include <iostream>

using namespace std;

/*
 * METHOD: perform_check_deposit
 * PURPOSE: Handles the complete check deposit process
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
 *   - Maximum 30 checks per transaction (hardware/processing limitation)
 *   - Minimum 100,000 won total (bank policy - small checks not accepted)
 * 
 * ERROR HANDLING:
 *   - Too many checks: Throws Invalid_Input_Exception
 *   - Amount too small: Throws Invalid_Input_Exception
 *   - Insufficient fee: Throws Invalid_Input_Exception
 * 
 * CALLED BY: ATM_Session.cpp when user selects check deposit option
 */
void ATM::perform_check_deposit(Bank_Account_Information *account,
                                bool is_primary_bank, bool is_korean) 
{
  // Display operation header
  if (is_korean) {
    cout << "--- 수표 입금 (CHECK DEPOSIT) ---" << endl;
  } 
  else {
    cout << "--- CHECK DEPOSIT ---" << endl;
  }

  /*
   * Entire operation wrapped in try-catch for error handling
   */
  try {
    /*
     * STEP 1: CALCULATE AND DISPLAY FEE
     * ===================================
     * 
     * Same fee structure as cash deposit:
     *   - Primary bank: Free
     *   - Other banks: 1,000 won
     */
    int fee;
    if (is_primary_bank) {
      fee = 0;  // No fee for primary bank
    } 
    else {
      fee = 1000;  // Fee for other banks
    }

    if (is_korean) {
      cout << "수수료: " << fee << " won" << endl;
    } 
    else {
      cout << "Deposit Fee: " << fee << " won" << endl;
    }

    /*
     * STEP 2: GET NUMBER OF CHECKS
     * =============================
     * 
     * User enters how many checks they want to deposit.
     * 0 or negative = cancel operation
     * Max 30 checks per transaction
     */
    int num_checks;
    if (is_korean) {
      cout << "수표 매수 (Enter count, 0 to cancel): ";
    } 
    else {
      cout << "Enter number of checks (0 to cancel): ";
    }
    cin >> num_checks;

    // Handle cancellation
    if (num_checks <= 0) {
      return;  // Exit without depositing
    }

    // Validate maximum
    if (num_checks > 30) {
      throw Invalid_Input_Exception("Maximum 30 checks allowed");
    }

    /*
     * STEP 3: GET TOTAL CHECK AMOUNT
     * ===============================
     * 
     * User enters the total value of all checks combined.
     * In real ATM: Machine would scan checks and verify amounts.
     * Here: We trust user's input (simplified simulation).
     * 
     * MINIMUM: 100,000 won (bank policy - won't accept small checks)
     */
    long long check_total;
    if (is_korean) {
      cout << "총 액수 (Total Amount): ";
    } 
    else {
      cout << "Enter total check amount: ";
    }
    cin >> check_total;
    
    long long check_amt = check_total;

    // Validate minimum amount
    if (check_amt < 100000) {
      throw Invalid_Input_Exception("Checks must be at least 100,000 won");
    }

    /*
     * STEP 4: COLLECT FEE (if applicable)
     * ====================================
     * 
     * Similar to cash deposit - fee must be paid separately.
     * User inserts cash to cover the fee.
     */
    if (fee > 0) {
      if (is_korean) {
        cout << "수수료를 내어 주세요 (" << fee << "): ";
      } 
      else {
        cout << "Insert fee (" << fee << "): ";
      }

      int fee_input;
      cin >> fee_input;

      if (fee_input < fee) {
        throw Invalid_Input_Exception("Insufficient fee provided");
      }
    }

    /*
     * STEP 5: UPDATE ACCOUNT BALANCE
     * ===============================
     * 
     * Add check amount to user's account.
     * 
     * NOTE: In real banking, checks take days to clear (verify funds).
     * Here: We add immediately (simplified simulation).
     * 
     * IMPORTANT: Checks DON'T affect ATM cash inventory!
     * Unlike cash deposit, no bills are added to ATM.
     */
    account->add_to_balance(check_amt);

    /*
     * STEP 6: DISPLAY SUCCESS MESSAGE
     * ================================
     * 
     * Confirm deposit and show new balance.
     */
    long long new_balance = account->get_balance();
    if (is_korean) {
      cout << "입금 성공. 합계: " << check_amt << " won" << endl;
      cout << "현재 잔액: " << new_balance << " won" << endl;
    } 
    else {
      cout << "Deposit Successful. Total: " << check_amt << endl;
      cout << "Current Balance: " << new_balance << endl;
    }

    /*
     * STEP 7: RECORD TRANSACTION
     * ===========================
     * 
     * Log this check deposit in transaction history.
     * Description format: "CheckDeposit 100000"
     */
    string check_desc = "CheckDeposit ";
    string amt_str = to_string(check_amt);
    string desc = check_desc + amt_str;
    record_transaction(account->get_debit_card_number(), desc);

  } 
  /*
   * EXCEPTION HANDLING
   * ==================
   * 
   * Catch and display validation errors in appropriate language.
   * Three possible errors:
   *   1. Too many checks (>30)
   *   2. Amount too small (<100,000)
   *   3. Insufficient fee payment
   */
  catch (const Invalid_Input_Exception &e) {
    if (is_korean) {
      // Parse error message to show specific Korean message
      if (string(e.what()).find("30") != string::npos) {
        cout << "Max 30 장." << endl;
      } else if (string(e.what()).find("100,000") != string::npos) {
        cout << "수표는 최소 100,000 원 이상이어야 합니다." << endl;
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
  // Method ends, returns to menu
}
