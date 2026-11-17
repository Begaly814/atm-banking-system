// Written by: Begaly

/*
 * ========================================================================
 * FILE: ATM_Transfer.cpp
 * PURPOSE: Implements money transfer functionality for ATM
 * 
 * WHAT THIS FILE DOES:
 * Allows users to transfer money to another account. Two modes:
 *   1. Cash Transfer: User inserts physical cash to send
 *   2. Account Transfer: Deduct from user's account balance
 * 
 * PROCESS:
 *   1. Choose transfer type (cash or account)
 *   2. Enter recipient's account number
 *   3. Search all banks for recipient
 *   4. Calculate fee based on transfer type and bank relationships
 *   5. For cash: Get bills, verify enough for fee
 *   6. For account: Get amount, verify sufficient balance
 *   7. Deduct from sender, add to recipient
 *   8. Record transaction
 * 
 * FEE COMPLEXITY:
 * Transfer fees are most complex because they depend on:
 *   - Transfer type (cash vs account)
 *   - Sender's bank vs ATM's bank
 *   - Recipient's bank vs ATM's bank
 * 
 * CONNECTS TO:
 *   - ATM.h: Implements perform_transfer method
 *   - Bank classes: Searches for recipient account across all banks
 *   - Exceptions.h: Throws various exception types
 * ========================================================================
 */

#include "ATM.h"
#include "Exceptions.h"
#include <iostream>

using namespace std;

/*
 * METHOD: perform_transfer
 * PURPOSE: Handles complete money transfer process (cash or account)
 * 
 * PARAMETERS:
 *   account: Pointer to sender's account (user logged in)
 *   banks: Reference to vector of all banks (to find recipient)
 *   is_primary_bank: true if sender's bank = ATM's primary bank
 *   is_korean: true if user selected Korean language
 * 
 * TWO TRANSFER MODES:
 *   1. Cash Transfer (Type 1):
 *      - User inserts physical cash at ATM
 *      - Cash goes to recipient's account
 *      - Sender doesn't need an account
 *      - Fixed fee: 2,000 won
 *   
 *   2. Account Transfer (Type 2):
 *      - Money deducted from sender's account
 *      - Added to recipient's account
 *      - Variable fee based on bank relationships
 * 
 * FEE STRUCTURE (Account Transfer):
 *   - Sender primary + Recipient primary: 1,000 won
 *   - Sender primary + Recipient other: 2,000 won
 *   - Sender other bank: 4,000 won (regardless of recipient)
 * 
 * ERROR HANDLING:
 *   - Recipient not found: Account_Not_Found_Exception
 *   - Insufficient balance: Insufficient_Funds_Exception
 *   - Cash too small for fee: Invalid_Input_Exception
 * 
 * CALLED BY: ATM_Session.cpp when user selects transfer option
 */
void ATM::perform_transfer(Bank_Account_Information *account,
                           vector<Bank> &banks, bool is_primary_bank,
                           bool is_korean){
  // Display operation header
  cout << (is_korean ? "--- 이체 (TRANSFER) ---" : "--- TRANSFER ---") << endl;

  /*
   * Entire operation wrapped in try-catch for error handling
   */
  try{
    /*
     * STEP 1: CHOOSE TRANSFER TYPE
     * =============================
     * 
     * Two options:
     *   1. Cash Transfer - Send cash directly
     *   2. Account Transfer - Send from account balance
     */
    cout << (is_korean ? "1. 현금 (Cash)\n2. 계좌 (Account)\n선택: "
                       : "1. Cash Transfer\n2. Account Transfer\nChoose: ");
    int transferType;
    cin >> transferType;

    /*
     * STEP 2: GET RECIPIENT'S ACCOUNT NUMBER
     * =======================================
     * 
     * User enters account number (not card number!) of recipient.
     * We need to search all banks to find this account.
     */
    cout << (is_korean ? "받는 분 계좌 (Dest Account): "
                       : "Recipient's Account: ");
    string destAccNum;
    cin >> destAccNum;

    /*
     * STEP 3: SEARCH FOR RECIPIENT ACCOUNT
     * =====================================
     * 
     * Search through ALL banks to find account with matching account number.
     * Need both account pointer and bank pointer for fee calculation.
     */
    Bank_Account_Information *destAcc = NULL;  // Recipient's account
    Bank *destBank = NULL;  // Bank that owns recipient's account
    bool found = false;
    
    // Loop through all banks
    for(int i = 0; i < banks.size(); i++){
      Bank_Account_Information *acc =
          banks[i].find_account_by_account_number(destAccNum);
      if(acc != NULL){
        // Found the recipient!
        destAcc = acc;
        destBank = &banks[i];
        found = true;
        break;  // Stop searching
      }
    }
    
    // Verify recipient was found
    if(found == false){
      throw Account_Not_Found_Exception("Account not found");
    }
    // If we reach here: Recipient account found successfully

    /*
     * ========================================
     * BRANCH: CASH TRANSFER (Type 1)
     * ========================================
     * 
     * User inserts physical cash to send to recipient.
     * Process:
     *   1. Show fixed fee (2,000 won)
     *   2. Get bills from user
     *   3. Calculate total cash
     *   4. Verify cash > fee (need some to actually send!)
     *   5. Send amount = cash - fee
     *   6. Add bills to ATM inventory
     *   7. Add amount to recipient's account
     */
    if(transferType == 1){
      /*
       * CASH TRANSFER FEE
       * Fixed at 2,000 won (simpler than account transfer)
       */
      int cashFee = 2000;
      cout << (is_korean ? "수수료: " : "Fee is ") << cashFee
           << (is_korean ? ". 현금 투입." : ". Insert Cash.") << endl;
      
      /*
       * GET BILLS FROM USER
       * Ask for count of each denomination
       */
      int bills50k;
      int bills10k;
      int bills5k;
      int bills1k;
      
      cout << (is_korean ? "50k (0 to cancel): " : "50k (0 to cancel): ");
      cin >> bills50k;
      if(bills50k < 0)
        return;  // Cancel operation
      
      cout << "10k: ";
      cin >> bills10k;
      cout << "5k: ";
      cin >> bills5k;
      cout << "1k: ";
      cin >> bills1k;
      
      /*
       * CALCULATE TOTAL CASH INSERTED
       */
      long long amt50 = (long long)bills50k * 50000;
      long long amt10 = (long long)bills10k * 10000;
      long long amt5 = (long long)bills5k * 5000;
      long long amt1 = (long long)bills1k * 1000;
      long long totalCash = amt50 + amt10 + amt5 + amt1;
      
      /*
       * VERIFY ENOUGH CASH FOR FEE
       * Must have more than just the fee (need something to send!)
       */
      if(totalCash <= cashFee){
        throw Invalid_Input_Exception(
            "Insufficient cash to cover transfer fee");
      }
      
      /*
       * CALCULATE SEND AMOUNT
       * Amount sent = Total cash - Fee
       * Example: Insert 50,000, fee 2,000 → send 48,000
       */
      long long sendAmount = totalCash - cashFee;
      
      /*
       * ADD TO RECIPIENT'S ACCOUNT
       * Recipient receives sendAmount (after fee deducted)
       */
      destAcc->add_to_balance(sendAmount);
      
      /*
       * ADD BILLS TO ATM INVENTORY
       * ATM keeps the physical cash (both fee and sent amount)
       */
      cash_inventory[0].number_of_bills += bills50k;
      cash_inventory[1].number_of_bills += bills10k;
      cash_inventory[2].number_of_bills += bills5k;
      cash_inventory[3].number_of_bills += bills1k;
      
      /*
       * DISPLAY SUCCESS
       */
      cout << (is_korean ? "이체 성공: " : "Transferred ")
           << sendAmount << (is_korean ? " won" : "") << endl;
      
      /*
       * RECORD TRANSACTION
       * Note: Card number is "CASH" (no sender account involved)
       * Description includes recipient account number
       */
      string trans_text = "Transfer ";
      string amt_text = to_string(sendAmount);
      string to_text = " to ";
      string desc = trans_text + amt_text + to_text + destAccNum;
      record_transaction("CASH", desc);
    } 
    /*
     * ========================================
     * BRANCH: ACCOUNT TRANSFER (Type 2)
     * ========================================
     * 
     * Deduct from sender's account, add to recipient's account.
     * Process:
     *   1. Get transfer amount
     *   2. Calculate fee based on bank relationships
     *   3. Verify sender has enough balance
     *   4. Deduct (amount + fee) from sender
     *   5. Add amount to recipient (recipient doesn't pay fee)
     */
    else{
      /*
       * GET TRANSFER AMOUNT
       */
      cout << (is_korean ? "금액 (Amount): " : "Enter Amount: ");
      int transAmt;
      cin >> transAmt;
      
      /*
       * CALCULATE FEE BASED ON BANK RELATIONSHIPS
       * ==========================================
       * 
       * Three factors:
       *   1. Is sender's bank the ATM's primary bank?
       *   2. Is recipient's bank the ATM's primary bank?
       *   3. Combination determines fee
       * 
       * FEE TABLE:
       *   Sender Primary + Recipient Primary: 1,000 won (best case)
       *   Sender Primary + Recipient Other: 2,000 won
       *   Sender Other: 4,000 won (worst case - double penalty)
       */
      string primaryBank = get_primary_bank();
      bool destIsPrimary = (destBank->get_name() == primaryBank);

      int accFee;
      if(is_primary_bank){
        // Sender is from primary bank - lower fees
        if(destIsPrimary){
          // Both from primary bank - lowest fee
          accFee = 1000;
        }else{
          // Sender primary, recipient other - medium fee
          accFee = 2000;
        }
      }else{
        // Sender from other bank - highest fee (4,000 regardless of recipient)
        accFee = 4000;
      }
      
      /*
       * VERIFY SUFFICIENT BALANCE
       * Sender needs: transfer amount + fee
       */
      long long totalNeed = transAmt + accFee;
      long long currBal = account->get_balance();
      bool enoughBalance = (currBal >= totalNeed);
      
      if(!enoughBalance){
        throw Insufficient_Funds_Exception("Insufficient funds for transfer");
      }
      
      /*
       * EXECUTE TRANSFER
       * ================
       * 
       * Step 1: Deduct from sender (amount + fee)
       * Step 2: Add to recipient (only amount, not fee)
       * 
       * NOTE: Fee is kept by ATM/bank system
       */
      account->subtract_from_balance(totalNeed);  // Sender pays amount + fee
      destAcc->add_to_balance(transAmt);  // Recipient receives amount only
      
      /*
       * DISPLAY SUCCESS
       */
      cout << (is_korean ? "이체 성공." : "Transfer Successful.") << endl;
      long long remBal = account->get_balance();
      cout << (is_korean ? "현재 잔액: " : "Current Balance: ")
           << remBal << "won" << endl;
      
      /*
       * RECORD TRANSACTION
       * Uses sender's card number and includes recipient account
       */
      string descText = "Transfer " + to_string(transAmt) + " to " + destAccNum;
      record_transaction(account->get_debit_card_number(), descText);
    }

  } 
  /*
   * EXCEPTION HANDLING
   * ==================
   * 
   * Three main error types for transfers:
   *   1. Account_Not_Found_Exception: Recipient doesn't exist
   *   2. Insufficient_Funds_Exception: Sender doesn't have enough balance
   *   3. Invalid_Input_Exception: Cash transfer amount too small for fee
   */
  catch(const Account_Not_Found_Exception &e){
    // Recipient account number not found in any bank
    cout << (is_korean ? "계좌 찾을 수 없음." : "Error: Account not found.")
         << endl;
  } 
  catch(const Insufficient_Funds_Exception &e){
    // Sender doesn't have enough balance for transfer + fee
    cout << (is_korean ? "잔액 부족." : "Error: Insufficient funds.")
         << endl;
  } 
  catch(const Invalid_Input_Exception &e){
    // Cash transfer: inserted cash <= fee (nothing left to send)
    cout << (is_korean ? "부족합니다." : "Error: Not enough for fee.")
         << endl;
  } 
  catch(const exception &e){
    // Any other unexpected error
    cout << "Error: " << e.what() << endl;
  }
  // Method ends, returns to menu
  
  /*
   * ========================================
   * TRANSFER SUMMARY (For Understanding)
   * ========================================
   * 
   * CASH TRANSFER:
   *   - User inserts cash at ATM
   *   - Fixed fee: 2,000 won
   *   - Recipient gets: cash - fee
   *   - ATM keeps all the cash
   *   - No sender account needed
   * 
   * ACCOUNT TRANSFER:
   *   - Money moves between accounts
   *   - Variable fee: 1,000-4,000 won
   *   - Sender pays: amount + fee
   *   - Recipient gets: amount only
   *   - No physical cash involved
   * 
   * WHY DIFFERENT FEES?
   *   - Encourage using same bank (lower fees)
   *   - Penalize cross-bank transactions (higher fees)
   *   - Cash transfers harder to process (fixed high fee)
   */
}
