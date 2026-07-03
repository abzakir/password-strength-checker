#include <cctype>
#include <iostream>
#include <string>

namespace {

bool containsUppercase(const std::string& password) {
	for (unsigned char ch : password) {
		if (std::isupper(ch)) {
			return true;
		}
	}
	return false;
}

bool containsLowercase(const std::string& password) {
	for (unsigned char ch : password) {
		if (std::islower(ch)) {
			return true;
		}
	}
	return false;
}

bool containsDigit(const std::string& password) {
	for (unsigned char ch : password) {
		if (std::isdigit(ch)) {
			return true;
		}
	}
	return false;
}

bool containsSpecialCharacter(const std::string& password) {
	for (unsigned char ch : password) {
		if (!std::isalnum(ch)) {
			return true;
		}
	}
	return false;
}

std::string strengthLabel(int score) {
	if (score >= 5) {
		return "STRONG";
	}
	if (score >= 3) {
		return "MEDIUM";
	}
	return "WEAK";
}

}  // namespace

int main() {
	std::string password;

	std::cout << "Enter password: ";
	std::getline(std::cin, password);

	const bool hasLength = password.length() >= 8;
	const bool hasUppercase = containsUppercase(password);
	const bool hasLowercase = containsLowercase(password);
	const bool hasDigit = containsDigit(password);
	const bool hasSpecialCharacter = containsSpecialCharacter(password);

	const int score = static_cast<int>(hasLength) + static_cast<int>(hasUppercase) +
					  static_cast<int>(hasLowercase) + static_cast<int>(hasDigit) +
					  static_cast<int>(hasSpecialCharacter);

	std::cout << "\nPassword Analysis\n";
	std::cout << "-----------------\n";
	std::cout << "Length: " << (hasLength ? "Good" : "Poor") << '\n';
	std::cout << "Uppercase: " << (hasUppercase ? "Yes" : "No") << '\n';
	std::cout << "Lowercase: " << (hasLowercase ? "Yes" : "No") << '\n';
	std::cout << "Number: " << (hasDigit ? "Yes" : "No") << '\n';
	std::cout << "Special Character: " << (hasSpecialCharacter ? "Yes" : "No") << '\n';
	std::cout << "\nScore: " << score << "/5\n";
	std::cout << "Strength: " << strengthLabel(score) << '\n';

	return 0;
}
