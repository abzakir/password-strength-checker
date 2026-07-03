# Password Strength Checker

A lightweight Windows desktop app built in C++ that evaluates the strength of a password based on multiple security criteria.

The program analyzes a password and provides a score along with a strength rating such as **Weak**, **Medium**, or **Strong**.

## Features

* Checks password length
* Detects uppercase letters
* Detects lowercase letters
* Detects numbers
* Detects special characters
* Calculates a password strength score
* Classifies passwords as Weak, Medium, or Strong
* Native Windows GUI
* Optional password visibility toggle
* Detailed result panel with improvement suggestions

## How It Works

The program evaluates the password using the following criteria:

| Criteria          | Requirement                                   |
| ----------------- | --------------------------------------------- |
| Length            | Password meets the minimum length requirement |
| Uppercase         | Contains at least one uppercase letter        |
| Lowercase         | Contains at least one lowercase letter        |
| Number            | Contains at least one numeric digit           |
| Special Character | Contains at least one special character       |

Each satisfied condition contributes to the final password strength score.

## Example

```text
Enter password: Zakir@2026

Password Analysis
-----------------
Length: Good
Uppercase: Yes
Lowercase: Yes
Number: Yes
Special Character: Yes

Score: 5/5
Strength: STRONG
```

## Technologies Used

* C++
* Standard Template Library (STL)
* Win32 API
* Native Windows controls

## Getting Started

### Prerequisites

Make sure you have a C++ compiler installed on Windows, such as MinGW or MSYS2.

### Clone the Repository

```bash
git clone <your-repository-url>
cd password-strength-checker
```

### Compile

```bash
g++ main.cpp -o password_checker -mwindows
```

### Run

```bash
password_checker.exe
```

The app opens a window where you can enter a password, analyze it, and review the score, strength rating, and suggestions.

## Project Structure

```text
password-strength-checker/
├── main.cpp
└── README.md
```

## Future Improvements

Possible improvements for future versions include:

* Common password detection
* Password entropy calculation
* Secure password generator
* Custom password rules
* Detailed improvement suggestions
* Graphical user interface
* Password history comparison

## Purpose

This project was created to practice fundamental C++ programming concepts, including:

* Strings
* Loops
* Conditional statements
* Functions
* Character analysis
* Basic input and output

It is a small learning project focused on writing clean logic and building a practical desktop utility.

## Contributing

Contributions, suggestions, and improvements are welcome. Feel free to fork the repository and submit a pull request.

## License

This project is open source and available for educational and personal use.
# password-strength-checker
