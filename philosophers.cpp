#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <memory>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

std::mutex writer_mutex;

class Fork{
public:
	Fork() noexcept = default;
	std::mutex& getFork() { return owner; }
	bool busy = false;
private:
	std::mutex owner;
};

using ForkPointer = std::shared_ptr<Fork>;

class Philosopher{
public:
	Philosopher(const size_t& meals_count_in, const std::string& name_in, const size_t& delay = 20) noexcept : meals_count(meals_count_in), name(name_in), runner(&Philosopher::live, this), delay(delay) {
		//runner.join();
	}

	void joinRunner(){
		runner.join();
	}

	const std::string& getName() const noexcept {
		return name;
	}

	void eat()  {
		while(!acess_to_meal)
			continue; //is this busy wait or not?
		
		//auto l_fork = left->owner;
		//auto& r_fork = right->owner;
		std::lock(left->getFork(), right->getFork());
		std::lock_guard<std::mutex> left_lock(left->getFork(), std::adopt_lock);
		std::lock_guard<std::mutex> right_lock(right->getFork(), std::adopt_lock);
		writer_mutex.lock();
		
		std::cout << name << " start eating\n";
		writer_mutex.unlock();
		int i = 0;
		sleep(rand() % delay * 0.1);
		//while(i++ < 1000000000)
		//	continue;
		//random delay
	}

	void think() {
		writer_mutex.lock();
		std::cout << name << " satrt thinking\n";
		writer_mutex.unlock();
	}

	void live() {	
		think();
		eat();
		writer_mutex.lock();
		std::cout << name << " done\n";
		writer_mutex.unlock();
		//joinRunner();
		unset_acess_to_meal();
	}

	void set_acess_to_meal(bool acess, const ForkPointer& left_in, const ForkPointer& right_in){
		//std::cout << name << " setted\n";
		acesser.lock();
		left = left_in;
		right = right_in;
		acess_to_meal = acess; // mutex here?
		acesser.unlock();
	}
	
	void unset_acess_to_meal(){
		//std::cout << name << " unseted\n";
		acesser.lock();
		left -> busy = false;
		right -> busy = false;
		left = nullptr;
		right = nullptr;
		acess_to_meal = false;
		acesser.unlock();
	}

private:
	std::string name;
	size_t meals_count;
	size_t delay;
	std::mutex acesser;
	ForkPointer left;
	ForkPointer right;
	bool acess_to_meal = false;
	std::thread runner;
public:
};

using PhilosopherPointer = std::shared_ptr<Philosopher>;

class Officiant{
public:
	Officiant(size_t forks_amount_in){
		forks_amount = forks_amount_in - 1;
		for(size_t i = 0; i < forks_amount_in; ++i){
			forks.emplace_back(std::move(new Fork()));
		}
	}
	
	void TakeCare(std::string&& name, const size_t& delay){
		//add threshold for current spot
		size_t left_fork = current_spot == forks_amount ? 0 : current_spot+1;//branch prediction is bad...
		size_t right_fork = current_spot;
		//std::cout << left_fork << " " << right_fork << " " << forks_amount <<  "\n";

		thinkers.push_back(std::move(std::make_shared<Philosopher>(11, name, delay)));
		//thinkers.back() -> set_acess_to_meal(true);
		current_spot++;
		//call sheduler here would be gooood
	}

	size_t getForksAmount(){
		size_t res = 0;
		for(size_t i = 0; i < forks_amount; ++i){
			if((forks[i] -> busy))
				res++;
		}

		return res;
	}

	void schedulePhilosophers(){
		for(int i = 0; i < current_spot; ++i){
			size_t left_fork = i == forks_amount ? 0 : i+1;
			size_t right_fork = i;
			while(getForksAmount() >= current_spot - 1)
				continue;
			//if(forks[left_fork] -> busy == true || forks[right_fork] -> busy == true)
			//	continue;
			
			forks[left_fork] -> busy = true;
			forks[right_fork] -> busy = true;
			thinkers[i] -> set_acess_to_meal(true, forks[left_fork], forks[right_fork]);
		}
	}


private:
	std::mutex sheduler_lock;
	size_t current_spot = 0;
	size_t forks_amount = 0;
	std::vector<ForkPointer> forks;
	std::vector<PhilosopherPointer> thinkers;
};

int main(){
	srand(time(NULL));

	Officiant ofnik(6);
	ofnik.TakeCare("Socrat", 200);
	ofnik.TakeCare("Plato", 20);
	ofnik.TakeCare("Decart", 200);
	ofnik.TakeCare("Kamu", 20);
	ofnik.TakeCare("Kant", 200);
	ofnik.TakeCare("Sereja", 20);
	ofnik.schedulePhilosophers();
	//ofnik.TakeCare("Serega");
	int i = 0;
	//while(i++ < 1000000000)
	//	continue;
	sleep(10);
	std::cout << "application ends well\n";
	

	while(1)
		continue;
	return 0;
}
