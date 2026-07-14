#include"NewCache.h"
//Cache.ipp не подключается т.к. он подключен в хидере
#include<iostream>
#include<sstream>
#include<string>
struct HeavyData {
	private:
	std::string name;
	std::vector<int>Myvec;
public:
	HeavyData(std::string n, int smth) :name(n) {
		Myvec.push_back(smth);
	}

	std::string serialize() const {//Метод для чтения данных из вектора(в т.ч. для внесения в БД)
		std::stringstream ss;//Создание "буфера"
		ss<< name << "|";
		for (int i = 0; i < Myvec.size(); i++) {
			ss << Myvec[i];
			if (i < Myvec.size() - 1) ss << ",";
		}
		return ss.str();
	}
	~HeavyData() {
	}
};
void Interface(Cache<std::string, std::unique_ptr<HeavyData>>& cash) {//Передали полностью "тип данных",все что указано в шаблоне ниже(в myCache)
	try {
		bool work = true;
		while (work == true) {
			int choice;
			std::cout << "1-Добавить элемент, 2-Найти элемент, 0-выход" << std::endl;
			std::cin >> choice;
			if (choice == 1) {
				std::string key;
				std::cout << " Key: " << std::endl;
				std::cin >> std::ws;
				getline(std::cin, key);
				std::cout << " Value: " << std::endl;
				std::string value;
				std::cin >> std::ws;
				getline(std::cin, value);
				int chislo;
				std::cout << "Число для вектора (тяжёлый объект)" << std::endl;
				std::cin >> chislo;
				HeavyData data(value, chislo);
				cash.put(key, make_unique<HeavyData>(value, chislo));//Через мейк_уникью создан сам объект с введёнными значениями
			}
			else if (choice == 2) {
				std::string find;
				std::cout << " Key " << std::endl;
				std::cin >> std::ws;
				getline(std::cin, find);
				cash.get(find);
			}
			else if (choice == 3|| choice==0) {
				work = false;
			}
		}
	}
	catch (std::exception& e) {
		std::cout << " ВНИМАНИЕ! " << e.what() << std::endl;
	}
}
int main() {
	system("chcp 65001 > nul");
	try {
		pqxx::connection connect("dbname=New_Cache user=postgres password=1234 host=localhost port=5432");
		int sizechoice;
		std::cout << "Введите размер кэша " << std::endl;
		std::cin >> sizechoice;
		Cache < std::string, std::unique_ptr<HeavyData>>myCache(sizechoice, connect);
		pqxx::work tx(connect);
		std::string query_table = "CREATE TABLE IF NOT EXISTS Cache(key VARCHAR (150) PRIMARY KEY NOT NULL, value VARCHAR (150) NOT NULL,add_tab TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";
		tx.exec(query_table);
		tx.commit();
		Interface(myCache);
	}
	catch (std::exception&e) {
		std::cout << " Внимание " << e.what() << std::endl;
	}
	system("Pause");
	return 0;
	}