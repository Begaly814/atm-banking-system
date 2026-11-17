# Multi-bank ATM banking system

A console simulation of a network of banks, accounts, debit cards and ATMs, written in C++ for the
object-oriented programming course at DGIST. Three-person team project; ~3,500 lines.

The interesting part is not the menus — it is that an ATM is a **physically constrained** machine.
It holds a finite number of bills in four denominations, so a withdrawal can be refused not because
the account lacks money but because the machine lacks the right notes. Fees depend on whether the
card belongs to the bank that owns the ATM. A single-bank ATM rejects foreign cards outright. Every
one of those rules has to hold simultaneously, and the state has to stay consistent when a
transaction fails halfway.

| | |
|---|---|
| **Course** | CSE201 Object-Oriented Programming — term project |
| **Institution** | DGIST, Daegu, South Korea · Fall 2025 |
| **Team** | 44 — three members (see [Authorship](#6-authorship)) |
| **Language** | C++17, standard library only — no third-party dependencies |
| **Size** | 12 files, 3,473 lines |
| **Verified** | Builds clean with `g++ -std=c++17 -Wall -Wextra`; graded live against the course test routine |

---

## 1. Build and run

No dependencies beyond a C++17 compiler. The program reads `sample_initial_condition.txt` from the
**current working directory**, so run it from the repository root.

```bash
make
./atm
```

Or without `make`:

```bash
g++ -std=c++17 -O2 -Isrc src/*.cpp -o atm && ./atm
```

On Windows with MSVC, open the folder in Visual Studio — `CppProperties.json` is included — or:

```
cl /std:c++17 /EHsc /Fe:atm.exe src\*.cpp
```

### Trying it out

At the prompt, enter an ATM serial number, or `/` for a snapshot of the whole system:

```
Select an ATM (Enter Serial Number, or '/' for snapshot): /

=== SNAPSHOT ===
Kakao ATM [SN:000111] Remaining cash : 138000
Daegu ATM [SN:000222] Remaining cash : 60000
Account [Bank: Kakao, No. 100-100-111111, Owner: Chris] Balance : 4000
Account [Bank: Daegu, No. 200-200-222222, Owner: Andor] Balance : 6000
================
```

Then pick `000111`, insert card `1111-1111-1111`, password `1234`, and the session menu opens.
Typing `ADMIN` at the ATM-selection prompt dumps the full transaction history.

---

## 2. The domain rules

### Accounts, cards and ATMs

| Concept | Behaviour |
|---|---|
| **Bank** | Owns accounts. Each account has an owner, account number, balance, debit card number and password. |
| **Single-bank ATM** | Accepts cards from its primary bank only; others are rejected at insertion. |
| **Multi-bank ATM** | Accepts any bank's card, at a higher fee. |
| **Unilingual ATM** | English only. |
| **Bilingual ATM** | English or Korean, chosen at the start of the session; every subsequent prompt is localised. |
| **Cash inventory** | Four denominations — KRW 50,000 / 10,000 / 5,000 / 1,000 — tracked as bill *counts*, not a total. |
| **Authentication** | Three password attempts, then the session ends. |

### Fees

| Transaction | Primary-bank card | Other bank |
|---|---:|---:|
| Cash deposit | 0 | 1,000 |
| Check deposit | 0 | 1,000 |
| Withdrawal | 1,000 | 2,000 |
| Account transfer, same bank | 1,000 | — |
| Account transfer, different banks | 2,000 | 4,000 |
| Cash transfer | 2,000 | 2,000 |

### Limits

- Withdrawal: at most **KRW 500,000** per transaction, at most **3 withdrawals per session**.
- Cash deposit: at most **50 bills** per transaction.
- Check deposit: at most **30 checks**, each at least **KRW 100,000**.
- A withdrawal needs balance ≥ amount **+ fee**, and the ATM must be able to make the amount from
  the bills it actually holds.

### Transactions

Balance enquiry · cash deposit · check deposit · withdrawal · cash transfer · account transfer.
Every transaction is recorded twice: in the ATM's permanent history, and in a per-session list that
is printed as a summary when the card is ejected.

---

## 3. Design

```
main.cpp              loads the configuration file, owns the banks/ATMs vectors, drives the
                      ATM-selection loop and the snapshot command

ATM.h                 the ATM class: cash inventory, session state, transaction history
ATM_Core.cpp          construction, cash accounting, fee rules, snapshot output
ATM_Session.cpp       session lifecycle — language choice, card validation, password retries,
                      the menu loop, per-session withdrawal cap, session summary
ATM_CashDeposit.cpp   bill-count validation, inventory update
ATM_CheckDeposit.cpp  check-count and minimum-amount rules
ATM_Withdrawal.cpp    greedy bill dispensing from the largest denomination down
ATM_Transfer.cpp      cash transfers and account-to-account transfers, with the fee matrix

Bank.h / Bank.cpp     the Bank class and account lookup by card number or account number
DataStructures.h      Cash_Denomination, Bank_Account_Information, Transaction_History_Record
Exceptions.h          exception hierarchy
```

### Dispensing, and why it can fail

A withdrawal is not a subtraction. The machine works down from KRW 50,000 notes, taking as many of
each as it has and the remainder needs. If it reaches the end with a remainder left over, the amount
is **unmakeable** and the whole transaction is rejected — ATM 2 in the test configuration holds only
50,000 notes, so it cannot dispense 70,000 even with plenty of cash inside.

### Exceptions

```cpp
class ATM_Exception : public std::exception
    ├── Insufficient_Funds_Exception     // account balance too low
    ├── Insufficient_Cash_Exception      // machine cannot make the amount
    ├── Invalid_Input_Exception          // limit violated, malformed entry
    ├── File_IO_Exception                // configuration file missing or corrupt
    └── Account_Not_Found_Exception      // unknown card or account number
```

Each failure path is designed to leave the account balance and the cash inventory untouched, so a
rejected transaction cannot half-apply.

---

## 4. Configuration file format

`sample_initial_condition.txt` is whitespace-separated:

```
<num_banks> <num_accounts> <num_atms>

<bank_name>                                                   × num_banks

<bank_name> <owner> <account_no> <balance> <card_no> <password>   × num_accounts

<primary_bank> <serial> <type> <language> <n50k> <n10k> <n5k> <n1k>   × num_atms
```

where `type` is `Single` or `Multi` and `language` is `Unilingual` or `Bilingual`. The committed file
defines two banks (Kakao, Daegu), two accounts and two ATMs — one single/unilingual, one
multi/bilingual. Edit it to build any scenario; the course demo used a three-bank, three-account,
three-ATM setup.

---

## 5. Grading and verification

The project was graded in a live demonstration against a course test routine: each team set up the
specified initial conditions, ran a sequence of actions, and captured a system snapshot after each
one, with a supervising student from another team confirming the source was not modified mid-demo.
The cases covered a single cash deposit, withdrawal from a non-primary bank, mixed deposit-then-
withdrawal sequences, and transfers across banks — checking both the resulting account balances and
the resulting per-denomination bill counts in each ATM.

Independently of grading, the code compiles with no errors under `g++ 15.2 -std=c++17 -Wall -Wextra`
(warnings only: a handful of signed/unsigned loop comparisons and two unused variables).

### Known limitations

Honest list, since this was a time-boxed course project:

- **The ATM-selection loop does not exit on EOF.** Piping input instead of typing it leaves the
  prompt spinning. Interactive use is unaffected.
- Signed/unsigned comparison warnings in the `for(int i = 0; i < vec.size(); ...)` loops.
- Account lookup is a linear scan — fine at this scale, wrong at any real one.
- State lives in memory; nothing is persisted back to disk except the `ADMIN` history dump.
- Balances are `long long` won rather than a decimal money type. Adequate for KRW integers, not a
  pattern to copy.

---

## 6. Authorship

A three-person team project. Every source file carries a `// Written by:` header, and they are
accurate — this table is just a summary.

| Component | Author |
|---|---|
| **`ATM_Transfer.cpp`** — cash and account transfers, fee matrix | **Begaly Berimbay** |
| **`Bank.cpp`** — bank class, account lookup | **Begaly Berimbay** |
| `main.cpp`, `ATM_Core.cpp`, `ATM_Session.cpp` | Munkhtemuulen Khashbolor |
| `ATM.h`, `Bank.h`, `DataStructures.h`, `Exceptions.h` — class design | Munkhtemuulen Khashbolor |
| `ATM_Withdrawal.cpp`, `ATM_CashDeposit.cpp`, `ATM_CheckDeposit.cpp` | Moosa |

This repository is published as part of **Begaly Berimbay's** portfolio. The work of the other two
team members is included because the program does not run without it, and it is credited to them
here and in each file header. Teammates: if you would prefer your files removed or the repository
taken down, open an issue and it will be done.

The term-project specification and the test routine are course material by the CSE201 teaching staff
and are deliberately **not** redistributed here; section 2 describes the requirements in our own
words.

---

## 7. Licence

MIT — see [`LICENSE`](LICENSE). It covers the contributions of the three team members named above
and does not extend to any course-supplied material.

---

**Begaly Berimbay** — B.S. Electronics Engineering & Computer Science, DGIST
