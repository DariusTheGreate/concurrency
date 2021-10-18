#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <memory>
#include <vector>
#include <unistd.h>

std::mutex writer_mutex;

class Fork{
public:
	Fork() noexcept = default;
	std::mutex& getFork() noexcept { return owner; }
//private:
	std::mutex owner;	
	~Fork(){
		std::cout << "fork ded\n";
	}
};

using ForkPointer = std::shared_ptr<Fork>;

class Philosopher{
public:
	Philosopher(const size_t& meals_count_in, const std::string& name_in, const ForkPointer& left_in, const ForkPointer& right_in) noexcept : meals_count(meals_count_in), name(name_in), left(left_in), right(right_in), runner(&Philosopher::live, this) {
		//runner.join();
	}


	void joinRunner(){
		runner.join();
	}

	const std::string& getName() const noexcept {
		return name;
	}

	void eat()  {
		//auto l_fork = left->owner;
		//auto& r_fork = right->owner;
		std::lock(left->owner, right->owner);
		//std::cout << "well\n";
		std::lock_guard<std::mutex> left_lock(left->owner, std::adopt_lock);
		std::lock_guard<std::mutex> right_lock(right->owner, std::adopt_lock);
		writer_mutex.lock();
		std::cout << name << " start eating\n";
		writer_mutex.unlock();
		int i = 0;
		sleep(1);
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
		
		while(meals_count-- > 0){
			think();
			eat();
			writer_mutex.lock();
			std::cout << name << " done\n";
			writer_mutex.unlock();
			//joinRunner();
		}
	}

private:
	std::string name;
	ForkPointer left;
	ForkPointer right;
	size_t meals_count;
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
	
	void TakeCare(std::string&& name){
		
		size_t left_fork = current_spot == forks_amount ? 0 : current_spot+1;//branch prediction is bad...
		size_t right_fork = current_spot;
		//std::cout << left_fork << " " << right_fork << " " << forks_amount <<  "\n";
		thinkers.push_back(std::move(std::make_shared<Philosopher>(3, name, forks[left_fork], forks[right_fork])));
		current_spot++;
	}

private:
	size_t current_spot = 0;
	size_t forks_amount = 0;
	std::vector<ForkPointer> forks;
	std::vector<PhilosopherPointer> thinkers;
};

int main(){
	Officiant ofnik(5);
	ofnik.TakeCare("Socrat");
	ofnik.TakeCare("Plato");
	ofnik.TakeCare("Decart");
	ofnik.TakeCare("Kamu");
	ofnik.TakeCare("Kant");
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
