#ifndef BOOST_LOCKFREE_TEST_HELPERS
#define BOOST_LOCKFREE_TEST_HELPERS

#include <set>
#include <boost/array.hpp>
#include <boost/lockfree/detail/atomic.hpp>
#include <boost/thread.hpp>

template <typename int_type>
int_type generate_id(void)
{
    static boost::lockfree::detail::atomic<int_type> generator(0);
    return ++generator;
}

template <typename int_type, size_t buckets>
class static_hashed_set
{
public:
    // knuth hash
    size_t hash(size_t key)
    {
        if (sizeof(size_t) == 8)
            return key * 11400954310080067584UL;
        else
            return key * 2654435761;
    }

    int calc_index(int_type id)
    {
        return hash((size_t)id) % buckets;
    }

    bool insert(int_type const & id)
    {
        std::size_t index = calc_index(id);

        boost::mutex::scoped_lock lock (ref_mutex[index]);

        std::pair<typename std::set<int_type>::iterator, bool> p;
        p = data[index].insert(id);

        return p.second;
    }

    bool find (int_type const & id)
    {
        std::size_t index = calc_index(id);

        boost::mutex::scoped_lock lock (ref_mutex[index]);

        return data[index].find(id) != data[index].end();
    }

    bool erase(int_type const & id)
    {
        std::size_t index = calc_index(id);

        boost::mutex::scoped_lock lock (ref_mutex[index]);

        if (data[index].find(id) != data[index].end()) {
            data[index].erase(id);
            assert(data[index].find(id) == data[index].end());
            return true;
        }
        else
            return false;
    }

    int count_nodes(void) const
    {
        int ret = 0;
        for (int i = 0; i != buckets; ++i) {
            boost::mutex::scoped_lock lock (ref_mutex[i]);
            ret += data[i].size();
        }
        return ret;
    }

private:
    boost::array<std::set<int_type>, buckets> data;
    mutable boost::array<boost::mutex, buckets> ref_mutex;
};

#endif
