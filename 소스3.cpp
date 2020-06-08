#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <iterator>
#include <chrono>

using namespace std;

const int theNumberOfPlayer{ 10'0000 };
const unsigned int bRange{ 2'905'887'026 };
const unsigned int cRange{ 1'112'670'384 };

random_device rd;
uniform_int_distribution<> uid('a', 'z');
uniform_int_distribution<> len(3, 7);
uniform_int_distribution<> gameChoicer(0, 1);
normal_distribution<> bScore(bRange/2, bRange / 6);
normal_distribution<> cScore(cRange / 2, cRange / 6);

static int cnt{ 0 };

unsigned int randomBreakout() {
	unsigned int tmp = (unsigned int)bScore(rd);
	tmp = clamp(tmp, (unsigned int)0, bRange);
	return tmp;
}

unsigned int randomChampionsLeague() {
	unsigned int tmp = (unsigned int)cScore(rd);
	tmp = clamp(tmp, (unsigned int)0, cRange);
	return tmp;
}

class Player { // Player의 예
	string id{};
	unsigned int breakout{}; // 떼탈출 점수
	unsigned int championsLeague{}; // 챔피언스리그 점수
public:
	Player() {
		for (int i = 0; i < len(rd); ++i)
			id += uid(rd);
		breakout = randomBreakout();
		championsLeague = randomChampionsLeague();
	}
	Player(string i) :id(i) {
		breakout = randomBreakout();
		championsLeague = randomChampionsLeague();
	}
	virtual ~Player() {}

	string getID() const { return id; }
	unsigned int getBreakout() const { return breakout; }
	unsigned int getChampionsLeague() const { return championsLeague; }

	void setBreakout(unsigned int n) { breakout = n; }
	void setChampionsLeague(unsigned int n) { championsLeague = n; }

	friend ostream& operator<<(ostream& os, const Player& p);
	friend istream& operator>>(istream& is, Player& p);

	/*bool operator>(const Player& p)const {
		++cnt;
		return breakout > p.breakout;
	}

	bool operator>=(const Player& p)const {
		++cnt;
		return breakout >= p.breakout;
	}

	bool operator<(const Player& p)const {
		++cnt;
		return breakout < p.breakout;
	}

	bool operator<=(const Player& p)const {
		++cnt;
		return breakout <= p.breakout;
	}

	bool operator==(const Player& p)const {
		++cnt;
		return breakout == p.breakout;
	}

	bool operator!=(const Player& p)const {
		++cnt;
		return breakout != p.breakout;
	}*/
};

ostream& operator<<(ostream& os, const Player& p) {
	os << p.id << " " << p.breakout << " " << p.championsLeague << endl;
	return os;
}

istream& operator>>(istream& is, Player& p) {
	is >> p.id >> p.breakout >> p.championsLeague;
	return is;
}

inline bool isExist(const string& filename) {
	ifstream in(filename);
	if (in)
		return true;
	return false;
}

int main() {
	if (!isExist("STL과제.txt")) {
		ofstream out("STL과제.txt");
		for (int i = 0; i < theNumberOfPlayer - 1; ++i) {
			Player p;
			out << p;
		}
		out << Player("aa");
	}

	ifstream in("STL과제.txt");
	vector<Player> players{ istream_iterator<Player>(in), {} };
	in.close();

	vector<int>tmpVec;
	tmpVec.reserve(100);

	int button{};
	int season{ 0 };
	string name{};
	unsigned int s{};
	bool isBChanged = false;
	bool isCChanged = false;
	int gameCounter{ 0 };
	int myRank{ 1 };
	unsigned int myScore{};
	unsigned int upScore = bRange+1;
	unsigned int downScore{ 0 };

	while (true) {
		cout << "<SEASON> : " << ++season << endl;
		cout << "<MENU> 시즌 진행: 0 / 종료: 1 " << endl;
		//cin >> button;
		if (button || tmpVec.size() == 100)
			break;
		else {
			gameCounter = 0;
			myRank = 1;
			upScore = bRange + 1;
			downScore = 0;
			for (auto it = players.begin(); it != players.end(); ++it) {
				if (gameChoicer(rd) && gameCounter < theNumberOfPlayer / 2) {
					++gameCounter;
					s = randomBreakout();
					if (it->getBreakout() < s) {
						it->setBreakout(s);
						if (!isBChanged)
							isBChanged = true;
					}
				}
				else {
					s = randomChampionsLeague();
					if (it->getChampionsLeague() < s) {
						it->setChampionsLeague(s);
						if (!isCChanged)
							isCChanged = true;
					}
				}
			}
			cout << "게임종료!" << endl;
			cout << "찾을사람을 입력하세요: ";
			//cin >> name;  
			name = "aa";
			auto start = chrono::steady_clock::now();
			auto myIter = find_if(players.begin(), players.end(), [=](const Player& p) {
				return p.getID() == name;
				});
			myScore = myIter->getBreakout();
			unsigned int tmp{};
			for (auto it = players.begin(); it != players.end(); ++it) {
				tmp = it->getBreakout();
				if (tmp > myScore)
					++myRank;
				if (tmp > myScore && tmp < upScore)
					upScore = tmp;
				if (tmp < myScore && tmp > downScore)
					downScore = tmp;
			}
			auto upIter = find_if(players.begin(), players.end(), [=](const Player& p) {
				return p.getBreakout() == upScore;
				});
			auto downIter = find_if(players.begin(), players.end(), [=](const Player& p) {
				return p.getBreakout() == downScore;
				});
			cout << endl << "탈출결과: " << endl;
			if (upIter != players.end())
				cout << upIter->getID() << "\t" << myRank - 1 << "등 상위" << (float)((myRank - 1) / (float)theNumberOfPlayer * 100) << "% 점수" << upIter->getBreakout() <<endl;
			cout << myIter->getID() << "\t" << myRank << "등 상위" << (float)(myRank / (float)theNumberOfPlayer * 100) << "% 점수" << myIter->getBreakout() << endl;
			if(downIter != players.end())
				cout << downIter->getID() << "\t" << myRank + 1 << "등 상위" << (float)((myRank + 1) / (float)theNumberOfPlayer * 100) << "% 점수" << downIter->getBreakout() << endl;
			myRank = 1;
			upScore = cRange + 1;
			downScore = 0;
			myScore = myIter->getChampionsLeague();
			for (auto it = players.begin(); it != players.end(); ++it) {
				tmp = it->getChampionsLeague();
				if (tmp > myScore)
					++myRank;
				if (tmp > myScore && tmp < upScore)
					upScore = tmp;
				if (tmp < myScore && tmp > downScore)
					downScore = tmp;
			}
			upIter = find_if(players.begin(), players.end(), [=](const Player& p) {
				return p.getChampionsLeague() == upScore;
				});
			downIter = find_if(players.begin(), players.end(), [=](const Player& p) {
				return p.getChampionsLeague() == downScore;
				});
			cout << endl << endl << "챔스결과: " << endl;
			if (upIter != players.end())
				cout << upIter->getID() << "\t" << myRank - 1 << "등 상위" << (float)((myRank - 1) / (float)theNumberOfPlayer * 100) << "% 점수" << upIter->getChampionsLeague() << endl;
			cout << myIter->getID() << "\t" << myRank << "등 상위" << (float)(myRank / (float)theNumberOfPlayer * 100) << "% 점수" << myIter->getChampionsLeague() << endl;
			if (downIter != players.end())
				cout << downIter->getID() << "\t" << myRank + 1 << "등 상위" << (float)((myRank + 1) / (float)theNumberOfPlayer * 100) << "% 점수" << downIter->getChampionsLeague() << endl;
			auto time = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start);
			tmpVec.push_back(time.count());
		}
	}
	cout << "good bye" << endl;
	int sum{ 0 };
	for (auto e : tmpVec)
		sum += e;
	cout << "평균 속도: " << sum / 100 << endl;
	{
		ofstream out("STL과제.txt");
		for (auto it = players.begin(); it != players.end(); ++it) {
			out << *it;
		}
	}
}