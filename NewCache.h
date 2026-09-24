#pragma once
	#include<iostream>
	#include<memory>
	#include<thread>
	#include<mutex>
	#include<pqxx/pqxx>
	#include<condition_variable>
	#include<queue>
	#include<list>
	#include<unordered_map>
	template<typename K, typename V>// k,v-типы данных
	class Cache {
	private:
		std::string key;
		std::list<std::pair<K, V>>myList;//K-ключ,V-значение
		std::unordered_map<K, typename  std::list<std::pair<K, V>>::iterator>Map;
		size_t razmer;
		pqxx::connection& conn;
		std::mutex mtx;
		std::thread dbworker;//Отдельный ЧЕРТЁЖ потокф для записи в БД
		std::queue < std::pair<K, V>>que;//Поменять на стринг
		std::condition_variable cv;
		std::mutex dbMtx;//Отдельный мьютекс для потока записи в бд
		bool work = false;
		void async();
	public:
		Cache(size_t size, pqxx::connection& con);
	
		void put(const K&key,V value);//V передаю не по ссылке а по значени. т.к. если на его месте окажется unique_ptr,нужен move
		const V& get(const K&key);//т.е. по ссылке возвращаю значение V(т.к. это можт быть unique_ptr)
		~Cache();
	};

#include"Cache.ipp"
