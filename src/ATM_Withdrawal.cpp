// Written by: Moosa

/*
 * ========================================================================
 * FILE: ATM_Withdrawal.cpp
 * PURPOSE: Implements cash withdrawal functionality for ATM
 * 
 * WHAT THIS FILE DOES:
 * Allows users to withdraw cash from their account. This is the most
 * complex transaction because:
 *   1. Must calculate which bills to dispense
 *   2. Must verify ATM has enough bills
 *   3. Must verify account has sufficient balance
 *   4. Uses greedy algorithm to minimize number of bills dispensed
 * 
 * PROCESS:
 *   1. Get withdrawal amount from user
 *   2. Calculate fee (depends on bank relationship)
 *   3. Verify account has enough balance (amount + fee)
 *   4. Calculate which bills to dispense (greedy: largest first)
 *   5. Verify ATM has those bills available
 *   6. Deduct from account and ATM inventory
 *   7. Display bill breakdown
 *   8. Record transaction
 * 
 * CONNECTS TO:
 *   - ATM.h: Implements perform_withdrawal method
 *   - Bank_Account_Information: Deducts from balance
 *   - Exceptions.h: Throws multiple exception types
 * ========================================================================
 */

#include "ATM.h"
#include "Exceptions.h"
#include <iostream>

using namespace std;

/*
 * METHOD: perform_withdrawal
 * PURPOSE: Handles complete cash withdrawal process with bill dispensing
 * 
 * PARAMETERS:
 *   account: Pointer to user's account (to deduct money)
 *   is_primary_bank: true if user's bank = ATM's primary bank
 *   is_korean: true if user selected Korean language
 * 
 * FEE STRUCTURE (NOTE: Withdrawals charge fees even for primary bank!):
 *   Primary bank: 1,000 won
 *   Other banks: 2,000 won
 * 
 * CONSTRAINTS:
 *   - Maximum 500,000 won per transaction (ATM security policy)
 *   - Amount must be divisible into available bills
 *   - Account must have balance >= (amount + fee)
 *   - ATM must have enough bills to make exact change
 * 
 * ERROR HANDLING:
 *   - Amount too large: Invalid_Input_Exception
 *   - Insufficient balance: Insufficient_Funds_Exception
 *   - ATM out of bills: Insufficient_Cash_Exception
 * 
 * CALLED BY: ATM_Session.cpp when user selects withdrawal option
 */
void ATM::perform_withdrawal(Bank_Account_Information *account,
                             bool is_primary_bank, bool is_korean) 
{
  // Display operation header
  if (is_korean) {
    cout << "--- 출금 (WITHDRAWAL) ---" << endl;
  } 
  else {
    cout << "--- WITHDRAWAL ---" << endl;
  }

  /*
   * Entire operation wrapped in try-catch for comprehensive error handling
   */
  try {
    /*
     * STEP 1: GET WITHDRAWAL AMOUNT
     * ==============================
     * 
     * User enters how much cash they want.
     * 0 or negative = cancel operation
     */
    if (is_korean) {
      cout << "금액 (Amount) 0 to cancel: ";
    } 
    else {
      cout << "Enter amount (0 to cancel): ";
    }

    int withdraw_amt;
    cin >> withdraw_amt;

    // Handle cancellation
    if (withdraw_amt <= 0) {
      return;  // Exit without withdrawing
    }

    /*
     * STEP 2: VALIDATE MAXIMUM AMOUNT
     * ================================
     * 
     * ATM policy: Max 500,000 won per transaction
     * Reasons: Security, prevent ATM depletion, fraud protection
     */
    int max_withdraw = 500000;
    if (withdraw_amt > max_withdraw) {
      throw Invalid_Input_Exception("Maximum 500,000 won per transaction");
    }

    /*
     * STEP 3: CALCULATE AND DISPLAY FEE
     * ==================================
     * 
     * Withdrawals ALWAYS have a fee (even for primary bank customers).
     * This is typical in real banking - withdrawal costs more to process.
     * 
     * Fee structure:
     *   Primary bank: 1,000 won
     *   Other banks: 2,000 won (double the fee)
     */
    int fee_amt;
    if (is_primary_bank) {
      fee_amt = 1000;  // Lower fee for primary bank
    } 
    else {
      fee_amt = 2000;  // Higher fee for other banks
    }

    if (is_korean) {
      cout << "수수료: " << fee_amt << " won" << endl;
    } 
    else {
      cout << "Fee: " << fee_amt << " won" << endl;
    }

    /*
     * STEP 4: CHECK ACCOUNT BALANCE
     * ==============================
     * 
     * User needs enough balance for BOTH withdrawal amount AND fee.
     * Total needed = withdrawal amount + fee
     * 
     * Example: Withdraw 100,000 won with 1,000 fee = need 101,000 balance
     */
    long long total_needed = withdraw_amt + fee_amt;
    long long curr_balance = account->get_balance();
    
    bool sufficient_funds = (curr_balance >= total_needed);
    if (!sufficient_funds) {
      // Not enough money in account
      throw Insufficient_Funds_Exception("Account balance too low");
    }

    /*
     * STEP 5: BILL DISPENSING ALGORITHM (GREEDY APPROACH)
     * ====================================================
     * 
     * CHALLENGE: Calculate which bills to dispense to make exact amount.
     * 
     * ALGORITHM: Greedy (use largest bills first)
     *   1. Start with 50,000 won bills - dispense as many as possible
     *   2. Then 10,000 won bills for remaining amount
     *   3. Then 5,000 won bills
     *   4. Finally 1,000 won bills
     *   5. If any amount still remaining after all denominations = FAIL
     * 
     * WHY GREEDY?
     *   - Minimizes number of bills dispensed (faster, less mechanical wear)
     *   - Works for Korean won denominations (each is divisible into smaller ones)
     *   - Simple and efficient
     * 
     * NOTE: Greedy doesn't always work for all currency systems!
     * For example, with bills [1, 3, 4], greedy fails for amount 6.
     * But for [1, 5, 10, 50] (Korean system), greedy is optimal.
     */
    
    int remaining = withdraw_amt;  // Track how much we still need to dispense

    // These track how many bills of each type we'll dispense
    int bills_50k = 0;
    int bills_10k = 0;
    int bills_5k = 0;
    int bills_1k = 0;

    /*
     * DISPENSE 50,000 WON BILLS
     * ==========================
     * 
     * Check: ATM has 50k bills AND remaining amount >= 50,000
     * Calculate: How many 50k bills needed
     * Limit: Can't dispense more than available
     * Update: Subtract dispensed amount from remaining
     */
    int avail_50k = cash_inventory[0].number_of_bills;  // How many 50k bills in ATM?
    if (avail_50k > 0 && remaining >= 50000) {
      int need = remaining / 50000;  // How many 50k bills do we need?

      if (need > avail_50k) {
        // Need more than available - use all we have
        need = avail_50k;
      }

      int dispensed = need * 50000;  // Total value dispensed
      remaining = remaining - dispensed;  // Update remaining amount
      bills_50k = need;  // Remember how many we're dispensing
    }

    /*
     * DISPENSE 10,000 WON BILLS
     * ==========================
     * 
     * Same logic as above, but for 10,000 won bills
     */
    int avail_10k = cash_inventory[1].number_of_bills;
    if (avail_10k > 0 && remaining >= 10000) {
      int need = remaining / 10000;

      if (need > avail_10k) {
        need = avail_10k;
      }

      int dispensed = need * 10000;
      remaining = remaining - dispensed;
      bills_10k = need;
    }

    /*
     * DISPENSE 5,000 WON BILLS
     * =========================
     * 
     * Same logic for 5,000 won bills
     */
    int avail_5k = cash_inventory[2].number_of_bills;
    if (avail_5k > 0 && remaining >= 5000) {
      int need = remaining / 5000;

      if (need > avail_5k) {
        need = avail_5k;
      }

      remaining = remaining - (need * 5000);
      bills_5k = need;
    }

    /*
     * DISPENSE 1,000 WON BILLS
     * =========================
     * 
     * Last denomination - this should handle any remaining amount
     * since 1,000 is our smallest bill
     */
    int avail_1k = cash_inventory[3].number_of_bills;
    if (avail_1k > 0 && remaining >= 1000) {
      int need = remaining / 1000;

      if (need > avail_1k) {
        need = avail_1k;
      }

      remaining = remaining - (need * 1000);
      bills_1k = need;
    }

    /*
     * VERIFY WE CAN MAKE EXACT CHANGE
     * ================================
     * 
     * After trying all denominations, check if anything left.
     * If remaining > 0, ATM can't make exact change.
     * 
     * EXAMPLES OF FAILURE:
     *   - Want 7,000 but ATM only has 10,000 bills
     *   - Want 3,000 but ATM only has 5,000 bills
     *   - Want 500 (below smallest denomination)
     */
    if (remaining > 0) {
      // Can't dispense exact amount with available bills
      throw Insufficient_Cash_Exception("ATM does not have enough cash");
    }
    // If we reach here: Successfully calculated bill breakdown!

    /*
     * STEP 6: UPDATE ACCOUNT BALANCE
     * ===============================
     * 
     * Deduct BOTH withdrawal amount AND fee from account.
     * This happens AFTER we verify bills are available
     * (don't deduct if we can't dispense!)
     */
    account->subtract_from_balance(total_needed);

    /*
     * STEP 7: UPDATE ATM CASH INVENTORY
     * ==================================
     * 
     * Remove dispensed bills from ATM's inventory.
     * These bills are physically dispensed to the user.
     * 
     * IMPORTANT: This is opposite of deposit!
     *   Deposit: ADD bills to inventory
     *   Withdrawal: SUBTRACT bills from inventory
     */
    cash_inventory[0].number_of_bills = cash_inventory[0].number_of_bills - bills_50k;
    cash_inventory[1].number_of_bills = cash_inventory[1].number_of_bills - bills_10k;
    cash_inventory[2].number_of_bills = cash_inventory[2].number_of_bills - bills_5k;
    cash_inventory[3].number_of_bills = cash_inventory[3].number_of_bills - bills_1k;

    /*
     * STEP 8: DISPLAY SUCCESS MESSAGE
     * ================================
     * 
     * Show success and tell user how many bills of each type they're receiving.
     * This helps user verify they got correct amount.
     */
    if (is_korean) {
      cout << "출금 성공." << endl;
    } 
    else {
      cout << "Withdrawal Successful." << endl;
    }

    // Show bill breakdown (same format for both languages)
    if (is_korean) {
      cout << "50k:" << bills_50k << ", 10k:" << bills_10k
           << ", 5k:" << bills_5k << ", 1k:" << bills_1k << endl;
    } 
    else {
      cout << "50k:" << bills_50k << ", 10k:" << bills_10k
           << ", 5k:" << bills_5k << ", 1k:" << bills_1k << endl;
    }

    // Show new balance after withdrawal
    long long new_balance = account->get_balance();
    if (is_korean) {
      cout << "현재 잔액: " << new_balance << " won" << endl;
    } 
    else {
      cout << "Current Balance: " << new_balance << endl;
    }

    /*
     * STEP 9: RECORD TRANSACTION
     * ===========================
     * 
     * Log this withdrawal in transaction history.
     * Note: Records withdrawal AMOUNT (not amount + fee)
     * Description format: "Withdraw 100000"
     */
    string withdraw_text = "Withdraw ";
    string amt_str = to_string(withdraw_amt);
    string desc = withdraw_text + amt_str;
    record_transaction(account->get_debit_card_number(), desc);

  } 
  /*
   * EXCEPTION HANDLING
   * ==================
   * 
   * Three types of errors can occur during withdrawal:
   *   1. Invalid_Input_Exception: Amount > 500,000
   *   2. Insufficient_Funds_Exception: Not enough money in account
   *   3. Insufficient_Cash_Exception: ATM can't make exact change
   * 
   * Each error has specific message in both languages.
   */
  catch (const Invalid_Input_Exception &e) {
    // Amount validation failed
    if (is_korean) {
      cout << "1회 max 500,000 won" << endl;
    } else {
      cout << "Error: " << e.what() << endl;
    }
  } 
  catch (const Insufficient_Funds_Exception &e) {
    // Account doesn't have enough balance
    if (is_korean) {
      cout << "잔액 부족." << endl;
    } else {
      cout << "Error: Insufficient funds." << endl;
    }
  } 
  catch (const Insufficient_Cash_Exception &e) {
    // ATM can't dispense exact amount with available bills
    if (is_korean) {
      cout << "ATM 금고 부족." << endl;
    } else {
      cout << "Error: Not enough cash in ATM." << endl;
    }
  } 
  catch (const exception &e) {
    // Any other unexpected error
    cout << "Error: " << e.what() << endl;
  }
  // Method ends, returns to menu
}
