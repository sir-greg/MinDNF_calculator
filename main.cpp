#include "Header.h"

const float Width = sf::VideoMode::getDesktopMode().width;
const float Hight = sf::VideoMode::getDesktopMode().height;
const float VariableCountWindowHight = .1;
const float VariableCountWindowWidth = 0.3;
const int CharacterSize = 16;
const int InterWindowGap = 20;
const int GapForCombs = 0;
const int NumberÎfVariablesLimit = 6;
bool once = true;
bool onetime = false;
bool done = true;
sf::Color white(255, 255, 255), black(0, 0, 0), BackGroundColor(211, 255, 235), red(255, 0, 0), InputColor(129, 193, 164);

std::vector<sf::Color> StageColor = {
	sf::Color(0, 0, 0),
	sf::Color(0, 55, 255),
	sf::Color(253, 0, 128),
	sf::Color(19, 223, 67),
	sf::Color(253, 3, 45)
};

sf::Font font;
sf::Music phrase;

sf::Vector2f SubWindow = {
	(double)InterWindowGap,
	VariableCountWindowHight * Hight + InterWindowGap
};

std::vector<char> alphabet('Z' - 'A' + 1);

std::vector<std::pair<sf::Vector2f, sf::Vector2f>> InputWindows;
std::vector<std::string> Combs;
std::vector<std::pair<bool, std::vector<std::pair<int, std::string>>>> FunctionValsToCombinations;

std::vector<std::string> slot;
std::set<std::string> possibleCombination;
std::vector<std::vector<std::string>> PossibleCombinationsForLine;
std::set<std::set<std::string>> MinDnfs;
int64_t minDnfLen = 2e9;

int getNumberOfBits(int64_t);
int factorial(int64_t);
void MainPanelFrontend(sf::RenderWindow&, int, int&, std::string, std::string);
void setTableTextSettings(sf::Text&);
void FillCombs(std::vector<std::string>&, int, int, std::string);
void generateTable(int);
void Backend(int, int);
bool subset(int, int, int);
void FindMinDNFs(int);
int64_t countCombSize(std::vector<std::string>&);

int64_t countCombSize(std::set<std::string>& comb) {
	int64_t count = 0;
	for (std::string str : comb) {
		for (char& c : str) {
			count += (c != '!');
		}
	}
	return count;
}

void FindMinDNFs(int LineLevel = 0) {
	if (LineLevel == PossibleCombinationsForLine.size()) {
		possibleCombination.clear();
		for (std::string& comb : slot) {
			possibleCombination.insert(comb);
		}
		int combSize = countCombSize(possibleCombination);
		if (combSize < minDnfLen) {
			minDnfLen = combSize;
			MinDnfs.clear();
		}
		if (combSize == minDnfLen) {
			MinDnfs.insert(possibleCombination);
		}
		return;
	}

	for (int elem = 0; elem < PossibleCombinationsForLine[LineLevel].size(); ++elem) {
		slot[LineLevel] = PossibleCombinationsForLine[LineLevel][elem];
		FindMinDNFs(LineLevel + 1);
	}
}

bool subset(int line, int col_left, int col_right) {
	for (int pos_left = 0; pos_left < Combs[col_left].size(); ++pos_left) {
		bool found = false;
		for (int pos_right = 0; pos_right < Combs[col_right].size(); ++pos_right) {
			if (Combs[col_left][pos_left] == Combs[col_right][pos_right]) {
				found = true;
				if (FunctionValsToCombinations[line].second[col_left].second[pos_left] ==
					FunctionValsToCombinations[line].second[col_right].second[pos_right]) {
					break;
				}
				else {
					return false;
				}
			}
		}
		if (!found) {
			return false;
		}
		found = false;
	}
	return true;
}

void generateTable(int NumberOfVariables) {
	FunctionValsToCombinations.clear();
	Combs.clear();
	for (int CombLen = 1; CombLen <= NumberOfVariables; ++CombLen) {
		FillCombs(Combs, CombLen, NumberOfVariables, "");
	}
	FunctionValsToCombinations.resize(1LL << NumberOfVariables);
	for (int64_t mask = 0, row = 0; mask < (1LL << NumberOfVariables); ++mask, ++row) {
		FunctionValsToCombinations[row].second.resize(1LL << NumberOfVariables);
		for (int letter = 0; letter < NumberOfVariables; ++letter) {
			FunctionValsToCombinations[row].second[letter].second = std::string();
			FunctionValsToCombinations[row].second[letter].second = std::to_string((mask >> NumberOfVariables - 1 - letter) & 1);
			FunctionValsToCombinations[row].second[letter].first = 0;
		}
	}

	for (int64_t row = 0; row < (1LL << NumberOfVariables); ++row) {
		for (int64_t col = NumberOfVariables; col < (1LL << NumberOfVariables) - 1; ++col) {
			std::string mask;
			for (char c : Combs[col]) {
				mask += FunctionValsToCombinations[row].second[c - 'A'].second;
			}
			FunctionValsToCombinations[row].second[col].second = mask;
			FunctionValsToCombinations[row].second[col].first = 0;
		}
	}
}

void FillCombs(std::vector<std::string>& Combis, int CurCombLength, int NumberOfVariables, std::string Combination = "") {
	if (CurCombLength == 0) {
		Combis.push_back(Combination);
		Combination.pop_back();
		return;
	}
	for (int letter = (Combination.empty()) ? 0 : Combination.back() - 'A' + 1; letter < NumberOfVariables; ++letter) {
		Combination.push_back(alphabet[letter]);
		FillCombs(Combis, CurCombLength - 1, NumberOfVariables, Combination);
		Combination.pop_back();
	}
}

void setTableTextSettings(sf::Text& text) {
	text.setCharacterSize(CharacterSize);
	text.setFillColor(black);
	text.setFont(font);
}

int factorial(int64_t Number) {
	int res = 1;
	while (Number > 1) {
		res *= Number;
		--Number;
	}
	return res;
}

int getNumberOfBits(int64_t Number) {
	int count = 0;
	while (Number >> 1 > 0) {
		++count;
		Number >>= 1;
	}
	return count + 1;
}

void ReverseBackend(int CurrentStage, int NumberOfVariables) {
	for (int NumberLine = 0; NumberLine < (1LL << NumberOfVariables); ++NumberLine) {
		if (FunctionValsToCombinations[NumberLine].first == 0) continue;
		for (int NumberPos = 0; NumberPos < (1LL << NumberOfVariables) - 1; ++NumberOfVariables) {
			if (FunctionValsToCombinations[NumberLine].second[NumberPos].first == CurrentStage) {
				FunctionValsToCombinations[NumberLine].second[NumberPos].first = 0;
			}
		}
	}
}

void Backend(int CurrentStage, int NumberOfVariables) {
	switch (CurrentStage) {
	case 2:
		for (int NumberLineFirst = 0; NumberLineFirst < (1LL << NumberOfVariables); ++NumberLineFirst) {
			if (!FunctionValsToCombinations[NumberLineFirst].first) {
				for (int NumberLineSecond = 0; NumberLineSecond < (1LL << NumberOfVariables); ++NumberLineSecond) {
					if (FunctionValsToCombinations[NumberLineSecond].first) {
						for (int col = 0; col < (1LL << NumberOfVariables) - 1; ++col) {
							if (FunctionValsToCombinations[NumberLineFirst].second[col].second ==
								FunctionValsToCombinations[NumberLineSecond].second[col].second) {
								FunctionValsToCombinations[NumberLineSecond].second[col].first = 2;
							}
						}
					}
				}
			}
		}
		break;
	case 3:
		for (int NumberLine = 0; NumberLine < (1LL << NumberOfVariables); ++NumberLine) {
			if (FunctionValsToCombinations[NumberLine].first != 0) {
				for (int NumberColFirst = 0; NumberColFirst < (1LL << NumberOfVariables) - 1; ++NumberColFirst) {
					if (FunctionValsToCombinations[NumberLine].second[NumberColFirst].first == 0) {
						for (int NumberColSecond = NumberColFirst + 1; NumberColSecond < (1LL << NumberOfVariables) - 1; ++NumberColSecond) {
							if (subset(NumberLine, NumberColFirst, NumberColSecond)) {
								FunctionValsToCombinations[NumberLine].second[NumberColSecond].first = 3;
							}
						}
					}
				}
			}
		}
		break;
	case 4:
		PossibleCombinationsForLine.clear();
		for (int64_t Line = 0; Line < (1 << NumberOfVariables); ++Line) {
			if (FunctionValsToCombinations[Line].first == 0) continue;
			int64_t count = 0;
			int64_t oneStringPos;
			for (int64_t Pos = 0; Pos < (1 << NumberOfVariables) - 1; ++Pos) {
				if (FunctionValsToCombinations[Line].second[Pos].first == 0) {
					++count;
					oneStringPos = Pos;
				}
			}
			if (count == 1) {
				FunctionValsToCombinations[Line].second[oneStringPos].first = 4;
			}
		}
		break;
	case 5:
		PossibleCombinationsForLine.clear();
		for (int64_t Line = 0; Line < (1LL << NumberOfVariables); ++Line) {
			if (FunctionValsToCombinations[Line].first == 0) continue;
			std::vector<std::string> stringInLine;
			for (int64_t pos = 0; pos < (1LL << NumberOfVariables) - 1; ++pos) {
				if (FunctionValsToCombinations[Line].second[pos].first == 0 ||
					FunctionValsToCombinations[Line].second[pos].first == 4) {
					stringInLine.push_back(std::string());
					for (int elem = 0; elem < Combs[pos].size(); ++elem) {
						if (FunctionValsToCombinations[Line].second[pos].second[elem] == '0') {
							stringInLine.back() += '!';
						}
						stringInLine.back() += Combs[pos][elem];
					}
				}
			}
			PossibleCombinationsForLine.push_back(stringInLine);
			stringInLine.clear();
		}
		slot.resize(PossibleCombinationsForLine.size());
		minDnfLen = 2e9;
		bool allOnes = true;
		for (int line = 0; line < (1 << NumberOfVariables); ++line) {
			if (FunctionValsToCombinations[line].first == 0) {
				allOnes = false;
				break;
			}
		}
		MinDnfs.clear();
		if (allOnes) {
			for (int var = 0; var < NumberOfVariables; ++var) {
				std::set<std::string> combination;
				std::string s;
				s += alphabet[var];
				combination.insert(s);
				combination.insert("!" + s);
				MinDnfs.insert(combination);
			}
			return;
		}
		FindMinDNFs();
		int cur = 1;
		/*
		for (std::vector<std::string> v : PossibleCombinationsForLine) {
			std::cout << cur << ") ";
			for (std::string s : v) {
				std::cout << s << " ";
			}
			std::cout << "\n";
			++cur;
		}
		*/
		break;
	}
}

void MainPanelFrontend(sf::RenderWindow& MainPanel, int InputPlace, int& Stage,
	std::string NumberOfVariables, std::string NumberOfFunction) {
	const int LineWidth = 2;

	sf::RectangleShape CurrentInputWindow;
	CurrentInputWindow.setFillColor(InputColor);
	switch (InputPlace) {
	case 0:
		CurrentInputWindow.setSize(sf::Vector2f(VariableCountWindowWidth * Width, VariableCountWindowHight * Hight));
		CurrentInputWindow.setPosition(sf::Vector2f(0, 0));
		break;
	case 1:
		CurrentInputWindow.setSize(sf::Vector2f((1 - VariableCountWindowWidth) * Width, VariableCountWindowHight * Hight));
		CurrentInputWindow.setPosition(sf::Vector2f(VariableCountWindowWidth * Width, 0));
		break;
	}
	MainPanel.draw(CurrentInputWindow);
	
	sf::Text text;
	text.setFont(font);
	text.setFillColor(black);

	if (!InputPlace && Stage == 0) {
		text.setString("Press -> or <- to change the input window...");
		text.setCharacterSize(15);
		text.setPosition(sf::Vector2f(5, Hight - 20));
		MainPanel.draw(text);
	}
	else if (Stage == 0) {
		text.setString("Press <Enter> to start the calculation");
		text.setCharacterSize(15);
		text.setPosition(sf::Vector2f(5, Hight - 20));
		MainPanel.draw(text);
	}
	else if (Stage < 5) {
		text.setString("Press <Enter> to contniue the calculation or <Backspace> to go a step backwards");
		text.setCharacterSize(15);
		text.setPosition(sf::Vector2f(5, Hight - 20));
		MainPanel.draw(text);
	}
	else {
		text.setString("Press <Backspace> to go a step backwards");
		text.setCharacterSize(15);
		text.setPosition(sf::Vector2f(5, Hight - 20));
		MainPanel.draw(text);
	}

	sf::RectangleShape Line(sf::Vector2f(Width, LineWidth));
	Line.setFillColor(black);
	Line.setPosition(sf::Vector2f(0, Hight * VariableCountWindowHight));
	MainPanel.draw(Line);

	Line.setSize(sf::Vector2f(LineWidth, Hight * VariableCountWindowHight));
	Line.setPosition(sf::Vector2f(Width * VariableCountWindowWidth, 0));
	MainPanel.draw(Line);

	std::string String;
	String = "Input the number of\nvariables of the function: \n";
	if (!NumberOfVariables.empty()) {
		String += NumberOfVariables;
		/*
		if (stoll(NumberOfVariables) > NumberÎfVariablesLimit) {
			
			if (!phrase.openFromFile("resources/Music/ah-shit.ogg")) {
				return;
			}
			if (onetime) phrase.play();
			
			String += " (c'mon man...)";
			
			onetime = false;
			
		}
		*/
	}
	else String += "...";
	
	text.setString(String);
	text.setCharacterSize(20);
	text.setPosition(sf::Vector2f(25, 25));

	MainPanel.draw(text);

	String.clear();
	String = "Input the number\nof the function (press -> for manual input): ";
	if (!NumberOfFunction.empty()) {
		String += NumberOfFunction;
	}
	else {
		String += "...";
	}
	text.setString(String);
	text.setPosition(sf::Vector2f(Width * VariableCountWindowWidth + 25, 25));
	text.setFillColor(black);

	MainPanel.draw(text);

	if (NumberOfVariables.size() > 0 && stoll(NumberOfVariables) <= NumberÎfVariablesLimit) {

		if (InputPlace >= 2) {
			CurrentInputWindow.setSize(sf::Vector2f(10 * CharacterSize - LineWidth, InterWindowGap - LineWidth));
			CurrentInputWindow.setPosition(sf::Vector2f(SubWindow.x + LineWidth, SubWindow.y + 3 * CharacterSize +
				(InputPlace - 2) * InterWindowGap + LineWidth));
			MainPanel.draw(CurrentInputWindow);
		}

		int NumberOfVariablesInt = stoll(NumberOfVariables);

		Line.setPosition(SubWindow);
		Line.setSize(sf::Vector2f(LineWidth, 3 * CharacterSize + InterWindowGap * (1LL << NumberOfVariablesInt)));
		MainPanel.draw(Line);

		int CurCombLength = 1;
		int Characters = 0;
		int CombNumber = 0;
		int FirstNCombinationsSum = NumberOfVariablesInt;
		std::vector<std::string> CurrentCombs;
		int Intend = 2;
		FillCombs(CurrentCombs, 1, NumberOfVariablesInt);
		for (int NumberCol = 1; NumberCol < (1LL << NumberOfVariablesInt) + 1; ++NumberCol) {
			sf::Vector2f Position = sf::Vector2f(SubWindow.x + 10 * CharacterSize + (CharacterSize * Characters), SubWindow.y);

			Line.setPosition(Position);
			MainPanel.draw(Line);

			if (CurrentCombs.size() > CombNumber) {
				setTableTextSettings(text);
				text.setString(CurrentCombs[CombNumber]);
				text.setPosition(sf::Vector2f(Position.x + Intend, Position.y + CharacterSize));
				MainPanel.draw(text);
			}

			if (NumberCol == (1LL << NumberOfVariablesInt)) break;

			for (int NumberLine = 1; NumberLine < (1LL << NumberOfVariablesInt) + 1; ++NumberLine) {
				sf::Vector2f TextPos = sf::Vector2f(Position.x + Intend, SubWindow.y + 3 * CharacterSize + (NumberLine - 1) * InterWindowGap);
				if (FunctionValsToCombinations[NumberLine - 1].second[NumberCol - 1].first > 1) {
					if (FunctionValsToCombinations[NumberLine - 1].second[NumberCol - 1].first == 2 && Stage >= 2) {
						sf::RectangleShape ScoreOut(sf::Vector2f(CurCombLength * CharacterSize, LineWidth));
						ScoreOut.setFillColor(StageColor[2]);
						ScoreOut.setPosition(sf::Vector2f(TextPos.x, TextPos.y + InterWindowGap / 2.0));
						MainPanel.draw(ScoreOut);
					}
					else if (FunctionValsToCombinations[NumberLine - 1].second[NumberCol - 1].first == 3 && Stage >= 3) {
						sf::RectangleShape ScoreOut(sf::Vector2f(CurCombLength * CharacterSize, LineWidth));
						ScoreOut.setFillColor(StageColor[3]);
						ScoreOut.setPosition(sf::Vector2f(TextPos.x, TextPos.y + InterWindowGap / 2.0));
						MainPanel.draw(ScoreOut);
					}
					else if (FunctionValsToCombinations[NumberLine - 1].second[NumberCol - 1].first == 4 && Stage >= 4) {
						sf::RectangleShape Mark(sf::Vector2f(CurCombLength * CharacterSize, InterWindowGap));
						Mark.setFillColor(StageColor[4]);
						Mark.setPosition(TextPos);
						MainPanel.draw(Mark);
					}
				}
				text.setString(FunctionValsToCombinations[NumberLine - 1].second[NumberCol - 1].second);
				text.setPosition(TextPos);
				setTableTextSettings(text);
				MainPanel.draw(text);
			}

			if (CombNumber + 1 < CurrentCombs.size()) {
				Characters += CurCombLength;
				++CombNumber;
			}
			else {
				Characters += CurCombLength;
				++CurCombLength;
				CombNumber = 0;
				FirstNCombinationsSum += CurCombLength * factorial(NumberOfVariablesInt) / factorial(CurCombLength) /
					factorial(NumberOfVariablesInt - CurCombLength);
				CurrentCombs.clear();
				FillCombs(CurrentCombs, CurCombLength, NumberOfVariablesInt);
			}

		}

		Line.setPosition(SubWindow);
		Line.setSize(sf::Vector2f((10 * CharacterSize + CharacterSize * Characters) + 2, LineWidth));
		MainPanel.draw(Line);

		for (int NumberLine = 0; NumberLine <= (1LL << NumberOfVariablesInt); ++NumberLine) {
			sf::Vector2f Position(SubWindow.x, SubWindow.y + 3 * CharacterSize + NumberLine * InterWindowGap);
			Line.setPosition(Position);
			MainPanel.draw(Line);
			if (NumberLine == (1LL << NumberOfVariablesInt)) break;
			if (Stage >= 1) {
				if (!FunctionValsToCombinations[NumberLine].first && !NumberOfFunction.empty() && stoll(NumberOfFunction) != 0) {
					Line.setFillColor(StageColor[1]);
					Line.setPosition(sf::Vector2f(Position.x, Position.y + InterWindowGap / 2.0));
					MainPanel.draw(Line);
					Line.setFillColor(StageColor[0]);
				}
			}
			bool Value = 0;
			if (NumberLine < FunctionValsToCombinations.size()) Value = FunctionValsToCombinations[NumberLine].first;
			text.setString(std::to_string(Value));
			text.setPosition(sf::Vector2f(Position.x + 5 * CharacterSize, Position.y));
			text.setCharacterSize(CharacterSize);
			text.setFont(font);
			text.setFillColor(black);
			MainPanel.draw(text);
		}

		text.setString("Function");
		text.setPosition(sf::Vector2f(SubWindow.x + CharacterSize, SubWindow.y + CharacterSize));
		MainPanel.draw(text);
	}
	if (Stage == 5) {
		sf::Vector2f MinDnfWindowSize = {
			1000,
			800
		};
		int MinDnfIntendation = 10;
		int CenterViewMove = 10;
		sf::Vector2f ViewCenter = {
			(float)(MinDnfWindowSize.x / 2.0),
			(float)(MinDnfWindowSize.y / 2.0)
		};
		sf::RenderWindow MinDnfWindow(sf::VideoMode(MinDnfWindowSize.x, MinDnfWindowSize.y), "MinDNFs");
		sf::View view(ViewCenter, MinDnfWindowSize);
		MinDnfWindow.setView(view);
		while (MinDnfWindow.isOpen()) {
			sf::Event event;
			while (MinDnfWindow.pollEvent(event)) {
				switch (event.type) {
				case sf::Event::Closed:
					MinDnfWindow.close();
					break;
				case sf::Event::TextEntered:
					if (event.text.unicode == '\b') {
						MinDnfWindow.close();
						--Stage;
					}
					break;
				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::Enter) {
						MinDnfWindow.close();
						--Stage;
					}
					else if (event.key.code == sf::Keyboard::Escape) {
						MinDnfWindow.close();
						--Stage;
					}
					else if (event.key.code == sf::Keyboard::Down) {
						ViewCenter.y += CenterViewMove;
						view.setCenter(ViewCenter);
						MinDnfWindow.setView(view);
					}
					else if (event.key.code == sf::Keyboard::Up && ViewCenter.y - CenterViewMove >= MinDnfWindowSize.y / 2.0) {
						ViewCenter.y -= CenterViewMove;
						view.setCenter(ViewCenter);
						MinDnfWindow.setView(view);
					}
					else if (event.key.code == sf::Keyboard::Left && ViewCenter.x - CenterViewMove >= MinDnfWindowSize.x / 2.0) {
						ViewCenter.x -= CenterViewMove;
						view.setCenter(ViewCenter);
						MinDnfWindow.setView(view);
					}
					else if (event.key.code == sf::Keyboard::Right) {
						ViewCenter.x += CenterViewMove;
						view.setCenter(ViewCenter);
						MinDnfWindow.setView(view);
					}
				}
			}
			MinDnfWindow.clear(BackGroundColor);

			std::string output = "The minimal DNFs of your function: \n";
			int DnfNumber = 1, cur = 0;
			for (std::set<std::string> set_of_combinations : MinDnfs) {
				output += std::to_string(DnfNumber) + ") ";
				cur = 0;
				for (std::string combination : set_of_combinations) {
					output += combination;
					++cur;
					if (cur != set_of_combinations.size()) {
						output += " || ";
					}
				}
				output += "\n";
				++DnfNumber;
			}
			text.setString(output);
			setTableTextSettings(text);
			text.setPosition(sf::Vector2f(MinDnfIntendation, MinDnfIntendation));
			MinDnfWindow.draw(text);
			MinDnfWindow.display();
			if (Stage != 5) {
				MinDnfWindow.close();
			}
		}
	}
}

int main()
{
	FunctionValsToCombinations.reserve(10);

	for (int letter = 'A'; letter <= 'Z'; ++letter) {
		alphabet[letter - 'A'] = letter;
	}

	sf::RenderWindow MainPanel(sf::VideoMode(Width, Hight), "Minimum DNF");
	if (!font.loadFromFile("resources/DroidSansMono.ttf")) {
		return 0;
	}

	InputWindows.push_back({ {0, 0}, {VariableCountWindowWidth * Width, VariableCountWindowHight * Hight} });
	InputWindows.push_back({ {VariableCountWindowWidth * Width, 0}, 
		{(1 - VariableCountWindowWidth) * Width, VariableCountWindowHight * Hight} });

	phrase.setVolume(100);
	std::string NumberOfVariablesString;
	std::string FunctionNumber = "0";
	int CurrentStage = 0;
	int InputPlaceNumber = 0;
	int InputPlaces = 2;
	while (MainPanel.isOpen()) {
		sf::Event event;
		while (MainPanel.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				MainPanel.close();
				break;
			case sf::Event::TextEntered:
				switch (InputPlaceNumber) {
				case 0: {
					if (CurrentStage != 0) break;
					if (event.text.unicode == '\b') {
						if (!NumberOfVariablesString.empty()) {
							NumberOfVariablesString.pop_back();
							break;
						}
					}
					if (!(event.text.unicode >= '0' && event.text.unicode <= '9')) break;
					NumberOfVariablesString += event.text.unicode;
					onetime = true;
					if (stoi(NumberOfVariablesString) >= NumberÎfVariablesLimit) break;
					if (!FunctionNumber.empty() && stoll(FunctionNumber) > (1LL << (1LL << stoll(NumberOfVariablesString)))) {
						FunctionNumber = std::to_string((1LL << (1LL << stoll(NumberOfVariablesString))) - 1);
						for (int Bit = (1LL << stoll(NumberOfVariablesString)) - 1; Bit > -1; --Bit) {
							FunctionValsToCombinations[(1LL << stoll(NumberOfVariablesString)) - 1 - Bit].first =
								(stoll(FunctionNumber) >> Bit) & 1;
						}
					}
					InputPlaces = 2 + (1LL << stoll(NumberOfVariablesString));
					generateTable(stoi(NumberOfVariablesString));
					break;
				}
				case 1: {
					if (CurrentStage != 0) break;
					if (event.text.unicode == '\b') {
						if (!FunctionNumber.empty()) {
							FunctionNumber.pop_back();
						}
					}
					else {
						if (!(event.text.unicode >= '0' && event.text.unicode <= '9')) break;
						bool CanInput;

						CanInput = NumberOfVariablesString.empty() &&
							(FunctionNumber.empty() || !FunctionNumber.empty() &&
								stoll(FunctionNumber) * 10 + event.text.unicode - '0' < (1LL << (1LL << NumberÎfVariablesLimit)));
						CanInput = CanInput || (!NumberOfVariablesString.empty()
							&& (FunctionNumber.empty() || !FunctionNumber.empty() &&
								stoll(FunctionNumber) * 10 + event.text.unicode - '0' < (1LL << (1LL << stoll(NumberOfVariablesString)))));
						if (CanInput) {
							if (FunctionNumber[0] == '0') {
								FunctionNumber.erase(0, 1);
							}
							FunctionNumber += event.text.unicode;
						}
					}
					if (NumberOfVariablesString.empty()) NumberOfVariablesString = std::to_string(getNumberOfBits(stoll(FunctionNumber)));
					int FunctionNumberInt = 0;
					if (!FunctionNumber.empty()) FunctionNumberInt = stoll(FunctionNumber);
					if ((1LL << stoll(NumberOfVariablesString)) - 1 >= FunctionValsToCombinations.size()) {
						FunctionValsToCombinations.resize((1LL << stoll(NumberOfVariablesString)));
					}
					for (int Bit = (1LL << stoll(NumberOfVariablesString)) - 1; Bit > -1; --Bit) {
						FunctionValsToCombinations[(1LL << stoll(NumberOfVariablesString)) - 1 - Bit].first =
							(FunctionNumberInt >> Bit) & 1;
					}
					break;
				}
				default: {
					if (CurrentStage != 0) break;
					if (FunctionNumber.empty() && NumberOfVariablesString.empty()) {
						break;
					}
					else {
						InputPlaces = 2 + (1LL << stoll(NumberOfVariablesString));
						if (InputPlaces - 2 >= FunctionValsToCombinations.size()) {
							FunctionValsToCombinations.resize(InputPlaces - 2);
						}
						if (event.text.unicode == '0' || event.text.unicode == '1') {
							FunctionValsToCombinations[InputPlaceNumber - 2].first = event.text.unicode - '0';
						}
						int64_t FunctionNumberInt = 0;
						for (int bitNumber = 0; bitNumber < FunctionValsToCombinations.size(); ++bitNumber) {
							if (FunctionValsToCombinations[bitNumber].first == 1) {
								FunctionNumberInt += ((int64_t)FunctionValsToCombinations[bitNumber].first << (1 << stoll(NumberOfVariablesString)) - bitNumber - 1);
							}
						}
						FunctionNumber = std::to_string(FunctionNumberInt);
					}
					break;
				}
				}
			case sf::Event::KeyPressed:
				if ((event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Up)
					&& InputPlaceNumber > 0) {
					--InputPlaceNumber;
				}
				else if ((event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Down)
					&& (InputPlaceNumber + 1 < InputPlaces)) {
					++InputPlaceNumber;
				}
				else if (event.key.code == sf::Keyboard::Enter && CurrentStage + 1 < 6 && !NumberOfVariablesString.empty() 
					&& stoll(NumberOfVariablesString) != 0 && !FunctionNumber.empty() && stoll(FunctionNumber) != 0) {
					++CurrentStage;
					done = true;
				}
				else if (event.key.code == sf::Keyboard::BackSpace && CurrentStage - 1 > -1) {
					--CurrentStage;
					done = true;
				}
				else if (event.key.code == sf::Keyboard::Escape) {
					MainPanel.close();
					break;
				}
			}
		}
		MainPanel.clear(BackGroundColor);
		if (!NumberOfVariablesString.empty() && !FunctionNumber.empty() && stoll(FunctionNumber) != 0 && done) {
			Backend(CurrentStage, stoi(NumberOfVariablesString));
			done = false;
		}
		MainPanelFrontend(MainPanel, InputPlaceNumber, CurrentStage, NumberOfVariablesString, FunctionNumber);
		MainPanel.display();
		while (phrase.getStatus());
		if (!NumberOfVariablesString.empty() && stoll(NumberOfVariablesString) > NumberÎfVariablesLimit) NumberOfVariablesString.pop_back();
	}

	return EXIT_SUCCESS;
}