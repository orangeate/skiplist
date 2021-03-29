#include <iostream>
#include <string>
#include "skiplist.h"

int main()
{
    SkipList<int, std::string> skipList(10);
    std::default_random_engine generator(0);
    std::uniform_int_distribution<int> distribution(0,500);

    // 80 88 126 142 194 240 290 325 380 481
    for(int i = 10 ; i > 0; i--)
    {
        int key = distribution(generator);
        skipList.insert(key, std::to_string(key));
    }

    skipList.erase(240);
    skipList[290] = "TT";

    skipList.show();

    std::cout << "elements_count: " << skipList.size() << std::endl;
    skipList.contains(126) ? std::cout<< "Found" : std::cout<< "Not found";
    std::cout<<std::endl;
    skipList.contains(240) ? std::cout<< "Found" : std::cout<< "Not found";

    return 0;
}
