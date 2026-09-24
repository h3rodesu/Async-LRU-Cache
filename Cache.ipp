//Это текстовый файл
template<typename K, typename V>
Cache<K, V>::Cache(size_t size, pqxx::connection& con) :razmer(size), conn(con) {
	this->dbworker=std::thread(&Cache::async,this);//При создании кэша создаётся ФИЗИЧЕСКИЙ поток
}
template<typename K, typename V>
void Cache<K, V>::put(const K& key, V value) {
	//Проверка на дубликаты
	{
		std::lock_guard<std::mutex>unilock(this->mtx);
		auto it = Map.find(key);
		if (it != Map.end()) {//Если дубликат найден
			it->second->second = move(value);//Опять же если v-unique_ptr
			this->myList.splice(this->myList.begin(), this->myList, it->second/*адрес*/);
			return;//Точно не переполнен
		}


		if (this->Map.size() >= this->razmer) {
			K oldkey = this->myList.back().first;
			V olddata = std::move(myList.back().second);//т.к. V-может быть unique_ptr
			{
				std::lock_guard<std::mutex>cacheLock(dbMtx);
				que.push({ oldkey,std::move(olddata) });//Обязательно мув
			}
				this->cv.notify_one();
			
			Map.erase(oldkey);
			myList.pop_back();//безопасно,т.к. счетчик ссылок точно будет 0 если тут unqiue_ptr

		}
		this->myList.emplace_front(key, std::move(value));//Список инициализации и move т.к. на месте v возможен unique_ptr
		this->Map[key] = this->myList.begin();//Логика та же,во второй ячейке мапы лежит пара из списка которая объявлена выше,
		//Мы просто присваиваем введённому ключу эти значения
	}
	}
template<typename K,typename V>
const V& Cache<K, V>::get(const K& key) {//Возвращаем значение по ссылке
	{
		std::lock_guard<std::mutex>lock(mtx);	
		auto it = Map.find(key);
		if (it != Map.end()) {
			this->myList.splice(this->myList.begin(), this->myList, it->second);
			std::cout << " По ключу " << key << "Найден результат" << std::endl;
			return it->second->second;//Возвращаем значенеи по ссылке
		}
		else {
			std::cout << "По ключу " << key << " не найдено результатов" << std::endl;
				throw std::runtime_error("Key not found");
		}
	}
}
	template<typename K,typename V>
	void Cache<K, V>::async() {
		while (true) {
			std::string inserttab;
			{

				std::unique_lock<std::mutex>asynclock(this->dbMtx);
				this->cv.wait(asynclock, [this]() {//Усыпление потока 
					return!que.empty() || work == true;
					});
					if (que.empty() && work == true) {
						return;
					}
				auto getval = std::move(this->que.front());
				this->que.pop();
				pqxx::work tx(this->conn);
				inserttab = "INSERT INTO Cache (key,value)Values (" + tx.quote(getval.first) + "," + tx.quote(getval.second->serialize()) + ");";
			}
			try {
				pqxx::work tx(this->conn);
				tx.exec(inserttab);
				tx.commit();
			}
			catch(const std::exception&e){
				std::cout << "WARNING" << e.what() << std::endl;
			}
			}
	}
template<typename K, typename V>
Cache<K, V>::~Cache() {
	{
		std::lock_guard<std::mutex>lock(this->dbMtx);
		work = true;
	}
		this->cv.notify_one();
	
	if (dbworker.joinable()) {//т.е. если он существует
		dbworker.join();
	}
}
