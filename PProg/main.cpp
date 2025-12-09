#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <memory>

using namespace std;

class Element {
protected:
	string title;
	int yearOfRelease;
	bool isRented;

public:
	Element(string t, int y) : title(t), yearOfRelease(y), isRented(false) {}

	// Wirtualny destruktor
	virtual ~Element() {}

	virtual void displayInfo() const = 0;

	string getTitle() const { return title; }
	void rent() { isRented = true; }
	void returnElement() { isRented = false; }

	virtual string toSave() const = 0;
};

class Book : public Element {
private:
	string author;
	int numPages;
public:
	Book(string t, int y, string a, int n) : Element(t, y), author(a), numPages(n) {}

	void displayInfo() const override {
		cout << "[KSIAZKA] Tytul: " << title
			<< ", Autor: " << author
			<< ", Rok: " << yearOfRelease
			<< (isRented ? " [WYPOZYCZONA]" : " [DOSTEPNA]") << endl;
	}

	string toSave() const override {
		return "K|" + title + "|" + to_string(yearOfRelease) + "|" + author + "|" + to_string(numPages) + "|" + to_string(isRented);
	}
};

class Movie : public Element {
private:
	string director;
	int durationMinutes;

public:
	Movie(string t, int r, string dir, int time) : Element(t, r), director(dir), durationMinutes(time) {}

	void displayInfo() const override {
		cout << "[FILM] Tytul: " << title
			<< ", Rezyser: " << director
			<< ", Rok: " << yearOfRelease
			<< (isRented ? " [WYPOZYCZONY]" : " [DOSTEPNY]") << endl;
	}

	string toSave() const override {
		return "F|" + title + "|" + to_string(yearOfRelease) + "|" + director + "|" + to_string(durationMinutes) + "|" + to_string(isRented);
	}
};

class Library {
private:
	vector<Element*> elementBase;

public:
	~Library() {
		for(Element* el : elementBase) {
			delete el;
		}
		elementBase.clear();
	}
	void addElement(Element* newElement) {
		elementBase.push_back(newElement);
	}

	void displayAll() {
		if (elementBase.empty()) {
			cout << "Baza jest pusta." << endl;
			return;
		} 
		cout << "Zawartosc wypozyczalni:" << endl;
		for (const auto& element : elementBase) {
			element->displayInfo();
		} cout << "--------------------------\n";
	}

	void saveToFile(string filename) {
		ofstream file(filename);
		if (file.is_open()) {
			for (const auto& element : elementBase) {
				file << element->toSave() << endl;
			}
			file.close();
			cout << "Dane zapisane do pliku: " << filename << endl;
		}
		else {
			cout << "Nie mozna otworzyc pliku do zapisu: " << filename << endl;
		}
	}

	void loadFromFile(string filename) {
		ifstream file(filename);
		if (!file.is_open()) {
			cout << "Brak pliku bazy lub blad otwarcia. Tworze nowa baze." << endl;
			return;
		}

		string line;
		while (getline(file, line)) {
			// Dodaæ logikê
		}
		file.close();
		cout << "Wczytano dane z pliku." << endl;
	}
};

void displayMenu() {
		cout << "\n----- MENU WYPOZYCZALNI -----\n";
	cout << "1. Dodaj ksiazke\n";
	cout << "2. Dodaj film\n";
	cout << "3. Wyswietl wszystkie elementy\n";
	cout << "4. Zapisz i wyjdz\n";
	cout << "-----------------------------";
	cout << "\nWybierz opcje: ";
}

int main() {
	Library manager;

	manager.loadFromFile("library_db.txt");
	int choice;
	bool running = true;

	while (running) {
		displayMenu();
		cin >> choice;
		
		if (cin.fail()) {
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Nieprawidlowy wybor. Sprobuj ponownie." << endl;
			continue;
		}

		switch (choice) {
		case 1:
			// Tu dodam cin >> tytul >> autor itd.

			// na razie hard-code dla testu
			manager.addElement(new Book("Wiedzmin", 1993, "A. Sapkowski", 350));
			cout << "Dodano ksiazke.\n";
			break;
		case 2:
			manager.addElement(new Movie("Matrix", 1999, "Wachowski", 136));
			cout << "Dodano film.\n";
			break;
		case 3:
			manager.displayAll();
			break;
		case 4:
			manager.saveToFile("library_db.txt");
			running = false;
			break;
		default:
			cout << "Nieznana opcja.\n";
		}
	}
	return 0;
}