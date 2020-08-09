#include <thread>
#include <iostream>
#include <signal.h>
#include <chrono>
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/creation_tags.hpp>

using namespace boost::interprocess;


using namespace::std;

void sighup(int i);
void sigint(int i);
void sigquit();
class Car{

    private: static int LEFT_END; static int RIGHT_END; static managed_shared_memory sharedMap;static void* address; string NAME;int SPEED; bool DIRECTION; int* LOCATION; int NEXT_NODE; bool FLAG;const char* locName;
    public:
    Car(string name, int speed, int direction, int location, int next_node, bool flag){
        std::cout<<"Car Initialization in Process:";
        this->NAME = name;
        this->SPEED = speed;
        this->DIRECTION = direction;
        this->NEXT_NODE = next_node;
        this->FLAG = flag;
        std::cout<<"Car Initialization in Process:";
        locName = (name+="Location").c_str();
        std::cout<<locName;
        LOCATION = sharedMap.find_or_construct<int>(locName)();
        *LOCATION = location;
    }
    static void initializeEnds(int left, int right){
        LEFT_END = left;
        RIGHT_END = right;
        address = sharedMap.get_address();
    }
    int getNextLocation(int time){
        return (*LOCATION)*time;
    }
    void setLocation(int location){
        std::cout<<"Enabling Lock:"+NAME+"\n";
        named_mutex named_mtx{open_only,(NAME+="Lock").c_str() };
        named_mtx.lock();
        std::cout<<"Updating Value :"+NAME+"\n";
        *LOCATION = location;
        named_mtx.unlock();
        std::cout<<"Released Lock:"+NAME+"\n";
    }
    int getspeed()
    {
        return this->SPEED;
    }
    int getLocation(){
        return *LOCATION;
    }
    void changeDirection(){
        this->DIRECTION = this->DIRECTION==0?1:0;
    }
    int getDirection(){
        return this->DIRECTION;
    }
    bool isEnd(){
        if(*LOCATION>=RIGHT_END) return 1;
        else if(*LOCATION<=LEFT_END) return 1;
        else return 0;
    }
    static void cleanup(){
        sharedMap.deallocate(address);
    }
};

void run(Car c, int counter, int sleeptime){
    int i=0, loc;
    while (i<counter)
    {
        if(c.isEnd()) {
            c.changeDirection();
            // cout<<"direction: "<<c.getDirection()<<endl;
            // break;
        }
        cout<<std::this_thread::get_id()<<"\t";
        std::this_thread::sleep_for(std::chrono::milliseconds(sleeptime));
        loc = c.getLocation();
        cout<<loc<<endl;
        if(c.getDirection()==1) c.setLocation(++loc);
        else c.setLocation(--loc);
        i++;
    }
    cout<<std::this_thread::get_id()<<"\tended"<<endl;
}
int getpulse(std::vector<Car> cars)
{
    //Function to find shortest rate in milliseconds
    float maxspeed =cars[0].getspeed();
    for(int i =1;i<cars.size();i++)
    {
        if(cars[i].getspeed()>maxspeed)
            maxspeed=cars[i].getspeed();
    }
    return (1/maxspeed)*1000;
    return maxspeed;
}

int Car::LEFT_END;
int Car::RIGHT_END;
void* Car::address;
managed_shared_memory Car::sharedMap{open_or_create, "Road", 1024};

//// sighup() function definition
//void sighup(int i)
//
//{
//    signal(SIGHUP, sighup); /* reset signal */
//    printf("CHILD: I have received a SIGHUP\n");
//}
//
//// sigint() function definition
//void sigint(int i)
//
//{
//    signal(SIGINT, sigint); /* reset signal */
//    printf("CHILD: I have received a SIGINT\n");
//}
//
//// sigquit() function definition
//void sigquit(int i)
//{
//    printf("My DADDY has Killed me!!!\n");
//    exit(0);
//}

int main(){
    std::cout<<"Initializing Ends:\n";
    Car::initializeEnds(0,100);
    std::cout<<"Initializing Cars:\n";
    Car car1 = Car("Car1",2,1,95,5,0);
    std::cout<<"Successfully created Car:Car1\n";
    Car car2 = Car("Car2",2,0,2,5,0);
    std::cout<<"Successfully created Car:Car2\n";
    //for calculation of shortest time
    vector<Car> v1;
    std::cout<<"Adding cars in vector for pulse calculation Car\n";
    v1.push_back(car1);
    v1.push_back(car2);
    std::cout<<"Starting calculation\n";
    int refresh_rate=getpulse(v1);
    std::cout<<"RefreshRate(in milliseconds)="<<refresh_rate<<"\n";
//    thread th1(&run, car1, 10, refresh_rate);
//    thread th2(&run, car2, 5, refresh_rate);
//    th1.join();
//    th2.join();

//    basic_managed_shared_memory basicManagedSharedMemory{open_or_create, "shm", 1024};
//    int *i = basic_managed_shared_memory.find_or_construct<int>("Integer")();
//    named_mutex named_mtx{"mtx"};
//    named_mtx.lock();
//    ++(*i);
//    std::cout << *i << '\n';
//    named_mtx.unlock();

//    fork();
//    pid_t pid;
//
//    /* fork a child process */
//    pid = fork();
//
//    printf("\n PID1 %d\n",pid);
//
//    pid = fork();
//
//    printf("\n PID2 %d\n",pid);

//    managed_shared_memory managed_shm{open_or_create, "shm", 1024};
//    std::cout<<managed_shm.get_free_memory()<<"\n"<<managed_shm.get_size()<<"\n";
//    int *i = managed_shm.find_or_construct<int>("Integer1")();
//    named_mutex named_mtx{open_only, "mtx"};
//    named_mtx.unlock();
//    managed_shm.destroy<int>("Integer");
//    std::cout << ++*i << '\n';
//    std::cout<<"Enabling :lock1\n";
//    ++(*i);
//    std::cout<<"Enabled :lock2\n";
//    named_mtx.unlock();
    cout<<"END\n";
//    std::cout<<"Deallocating Memory\n";
    return 0;
}
