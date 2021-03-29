#include <vector>
#include <ctime>
#include <random>
#include <memory>
template<typename K, typename V>
struct Data_
{
    Data_(K key, V val) : key_(key), val_(val){}

    K key_;
    V val_;
};

template<typename K, typename V>
class Node
{
public:
    Node(K key, V val, int level) :
        data_(Data_<K, V>(key, val)), level_(level-1), level_next_(std::vector<std::shared_ptr<Node<K, V>>>(level, nullptr)){}
    ~Node() = default;

    K get_key() const {return data_.key_;}
    V get_value() const {return data_.val_;}
    V& get_value() {return data_.val_;}
    void set_val(V val) { data_.val_ = val;}

public:
    std::vector<std::shared_ptr<Node<K, V>>> level_next_;     //  level_next_[0] --> level_next_[level-1]
private:
    int level_;
    Data_<K, V> data_;
};

template<typename K, typename V>
class SkipList
{
public:
    explicit SkipList(int max_level) : generator(time(nullptr))
    {
        level_ = 0;
        count_ = 0;
        max_level_ = max_level;

        head_ = std::make_shared<Node<K, V>>(Node<K, V>(K(), V(), max_level_));
    }
    ~SkipList() = default;

public:
    bool insert(K key, V val);
    bool contains(K key);
    bool erase(K key);
    int count();
    V& operator[] (K key);

    void show();


private:
    std::shared_ptr<Node<K,V>> find(int key, std::vector<std::shared_ptr<Node<K,V>>>&);
    int roulette_level();

private:
    int max_level_;
    int level_;
    int count_;

    std::shared_ptr<Node<K,V>> head_;
    std::default_random_engine generator;
};


template<typename K, typename V>
std::shared_ptr<Node<K,V>> SkipList<K, V>::find(int key, std::vector<std::shared_ptr<Node<K,V>>>& bucket)
{
    auto p = this->head_;

    for (int i = bucket.size()-1; i >= 0; i--)
    {
        while (p->level_next_[i] != nullptr && p->level_next_[i]->get_key() < key)
            p = p->level_next_[i];

        bucket[i] = p;
    }

    p = p->level_next_[0];
    return p;
}

template<typename K, typename V>
bool SkipList<K, V>::insert(K key, V val)
{
    std::vector<std::shared_ptr<Node<K,V>>> bucket(this->max_level_, nullptr);
    auto p = this->find(key, bucket);

    if(p != nullptr && p->get_key() == key)
        return false;

    // insert new node between bucket[i] and p
    if(p == nullptr || p->get_key() != key)
    {
        int new_level = roulette_level();

        // add new level
        if (new_level > level_)
        {
            for (int i = level_; i < new_level; i++)
                bucket[i] = head_;
            level_ = new_level;
        }

        std::shared_ptr<Node<K, V>> new_node = std::make_shared<Node<K, V>>(Node<K, V>(key, val, new_level));
        for (int i = 0; i < new_level; i++)
        {
            new_node->level_next_[i] = bucket[i]->level_next_[i];
            bucket[i]->level_next_[i] = new_node;
        }
        count_++;
    }
    return true;
}

template<typename K, typename V>
bool SkipList<K, V>::contains(K key)
{
    std::vector<std::shared_ptr<Node<K,V>>> bucket(this->max_level_, nullptr);
    auto p = this->find(key, bucket);

    if(p != nullptr && p->get_key() == key)
        return true;
    return false;
}

template<typename K, typename V>
bool SkipList<K, V>::erase(K key)
{
    std::vector<std::shared_ptr<Node<K,V>>> bucket(this->max_level_, nullptr);
    auto p = this->find(key, bucket);

    if (p != nullptr && p->get_key() == key)
    {
        for (int i = 0; i < level_; i++)
        {
            if (bucket[i]->level_next_[i] != p)
                break;

            bucket[i]->level_next_[i] = p->level_next_[i];
        }
        count_--;
    }
    return true;
}

template<typename K, typename V>
int SkipList<K, V>::count()
{
    return count_;
}


template<typename K, typename V>
int SkipList<K, V>::roulette_level()
{
    int level = 1;
    std::uniform_int_distribution<int> distribution(0,1);

    while (distribution(generator))
        level++;

    return (level < max_level_) ? level : max_level_;
}

template<typename K, typename V>
void SkipList<K, V>::show()
{
    for (int i = level_-1; i >= 0; i--)
    {
        auto node = this->head_->level_next_[i];
        std::cout << "Level " << i << ": ";

        while (node != nullptr)
        {
            std::cout << node->get_key() << ":" << node->get_value() <<" ";
            node = node->level_next_[i];
        }

        std::cout << std::endl;
    }
}

template<typename K, typename V>
V &SkipList<K, V>::operator[](K key)
{
    std::vector<std::shared_ptr<Node<K,V>>> bucket(this->max_level_, nullptr);
    auto p = this->find(key, bucket);
    if(p != nullptr && p->get_key() == key)
        return p->get_value();
}
