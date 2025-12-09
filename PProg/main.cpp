#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>

using namespace std;


// Funkcja pomocnicza do dzielenia stringa 
// Argumenty: 's' to ca³y string do pociêcia, 'delimiter' to znak, na którym dzieli; tutaj '|'
vector<string> split(const string& s, char delimiter) {
	vector<string> tokens;
	string token; // zmienna pomocnicza do przechowywania pojedynczego wyciêtego kawa³ka (tokenu)

	// stringstream pozwala traktowwaæ string jako strumieñ. Dziekiu temu mozemy czytaæ z niego jak z pliku lub cin
	istringstream tokenStream(s);  

	// getline czyta z tokenStream do `token` a¿ do napotkania `delimiter`. Normalnie czyta do koñca linii,
	// ale tu jest trzeci argument, wiêc czyta do napotkania '|'
	while (getline(tokenStream, token, delimiter)) { 

		// Dodawanie wyciêtego kawa³ka (tokenu) do wektora / listy wyników
		tokens.push_back(token);
	}
	return tokens; // Zwraca pe³en wektor kawa³ków tekstu
}

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
			//cout << "Brak pliku bazy lub blad otwarcia. Tworze nowa baze." << endl;
			return;
		}

		string line;
		while (getline(file, line)) {
			if (line.empty()) continue; // Pomiñ puste linie

			// Rozdzielanie linii na czêœci z u¿yciem '|'
			vector<string> data = split(line, '|');

			// Walidacja
			if (data.size() < 6) continue;

			try {
				string type = data[0];
				string title = data[1];
				int year = stoi(data[2]); // konwersja string na int
				bool isRented = stoi(data[5]) != 0; // konwersja string na bool

				Element* newElement = nullptr;
				
				// SprawdŸ czy to ksi¹¿ka "K" cyz film "F"

				if (type == "K") {  // KSI¥¯KA
					string author = data[3];
					int pages = stoi(data[4]);
					newElement = new Book(title, year, author, pages); // Utwórz obiekt ksi¹¿ki
				}
				else if (type == "F") { // FLIM
					string director = data[3];
					int duration = stoi(data[4]);
					newElement = new Movie(title, year, director, duration); // Utwórz obiekt filmu
				}

				// Je¿eli uda³o siê utworzyæ - ustaw status wypo¿yczenia i dodaj do bazy
				if (newElement != nullptr) {
					if (isRented) {
						newElement->rent();
					} 
					addElement(newElement);
				}
			} 
			catch (...) {
				// Zabezpieczenie przed b³êdami konwersji stoi, albo jeœli w pliku s¹ jakieœ nieprawid³owe dane
				// Po prostu pomijamy takie linie i tyle, program siê dzieki temu nie wywali
				continue;
			}
		} 
		file.close();
		cout << "Wczytano dane z pliku. Liczba elemetow: " << elementBase.size() << endl;
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
		case 1: {
			string title, author; int year, pages;

			cin.ignore();

			cout << "Podaj tytul ksiazki: "; getline(cin, title);
			cout << "Podaj autora: "; getline(cin, author);
			cout << "Podaj rok wydania: "; cin >> year;
			cout << "Podaj liczbe stron: "; cin >> pages;

			// na razie hard-code dla testu
			//manager.addElement(new Book("Wiedzmin", 1993, "A. Sapkowski", 350));

			manager.addElement(new Book(title, year, author, pages));
			cout << "Dodano ksiazke.\n";
			break;
		}

		case 2: {
			string title, director; int year, duration;

			cin.ignore();

			cout << "Podaj tytul filmu: "; getline(cin, title);
			cout << "Podaj re¿ysera: "; getline(cin, director);
			cout << "Podaj rok wydania: "; cin >> year;
			cout << "Podaj czas trwania [min]: "; cin >> duration;

			// na razie hard-code dla testu
			//manager.addElement(new Movie("Matrix", 1999, "Wachowski", 136));

			manager.addElement(new Movie(title, year, director, duration));
			cout << "Dodano film.\n";
			break;
		}

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