// Boost.Geometry Index
//
// R-tree initial packing
//
// Copyright (c) 2011-2013 Adam Wulkiewicz, Lodz, Poland.
//
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_INDEX_DETAIL_RTREE_PACK_CREATE_HPP
#define BOOST_GEOMETRY_INDEX_DETAIL_RTREE_PACK_CREATE_HPP

namespace boost { namespace geometry { namespace index { namespace detail { namespace rtree {

namespace pack_utils {

template <std::size_t Dimension>
struct biggest_edge
{
    BOOST_STATIC_ASSERT(0 < Dimension);
    template <typename Box>
    static inline void apply(Box const& box, typename coordinate_type<Box>::type & length, std::size_t & dim_index)
    {
        biggest_edge<Dimension-1>::apply(box, length, dim_index);
        typename coordinate_type<Box>::type curr
            = geometry::get<max_corner, Dimension-1>(box) - geometry::get<min_corner, Dimension-1>(box);
        if ( length < curr )
        {
            dim_index = Dimension - 1;
            length = curr;
        }
    }
};

template <>
struct biggest_edge<1>
{
    template <typename Box>
    static inline void apply(Box const& box, typename coordinate_type<Box>::type & length, std::size_t & dim_index)
    {
        dim_index = 0;
        length = geometry::get<max_corner, 0>(box) - geometry::get<min_corner, 0>(box);
    }
};

template <std::size_t I>
struct point_entries_comparer
{
    template <typename PointEntry>
    bool operator()(PointEntry const& e1, PointEntry const& e2) const
    {
        return geometry::get<I>(e1.first) < geometry::get<I>(e2.first);
    }
};

template <std::size_t I, std::size_t Dimension>
struct partial_sort_and_half_boxes
{
    template <typename EIt, typename Box>
    static inline void apply(EIt first, EIt median, EIt last, Box const& box, Box & left, Box & right, std::size_t dim_index)
    {
        if ( I == dim_index )
        {
            std::partial_sort(first, median, last, point_entries_comparer<I>());

            geometry::convert(box, left);
            geometry::convert(box, right);
            typename coordinate_type<Box>::type edge_len
                = geometry::get<max_corner, I>(box) - geometry::get<min_corner, I>(box);
            typename coordinate_type<Box>::type median
                = geometry::get<min_corner, I>(box) + edge_len / 2;
            geometry::set<max_corner, I>(left, median);
            geometry::set<min_corner, I>(right, median);
        }
        else
            partial_sort_and_half_boxes<I+1, Dimension>::apply(first, median, last, box, left, right, dim_index);
    }
};

template <std::size_t Dimension>
struct partial_sort_and_half_boxes<Dimension, Dimension>
{
    template <typename EIt, typename Box>
    static inline void apply(EIt , EIt , EIt , Box const& , Box & , Box & , std::size_t ) {}
};

} // namespace pack_utils

// STR leafs number are calculated as rcount/max
// and the number of splitting planes for each dimension as (count/max)^(1/dimension)
// <-> for dimension==2 -> sqrt(count/max)
//
// The main flaw of this algorithm is that the resulting tree will have bad structure for:
// 1. non-uniformly distributed elements
//      Statistic check could be performed, e.g. based on variance of lengths of elements edges for each dimension
// 2. elements distributed mainly along one axis
//      Calculate bounding box of all elements and then number of dividing planes for a dimension
//      from the length of BB edge for this dimension (more or less assuming that elements are uniformly-distributed squares)
//
// Another thing is that the last node may have less elements than Max or even Min.
// The number of splitting planes must be chosen more carefully than count/max
//
// This algorithm is something between STR and TGS
// it is more similar to the top-down recursive kd-tree creation algorithm
// using object median split and split axis of greatest BB edge
// BB is only used as a hint (assuming objects are distributed uniformly)

template <typename Value, typename Options, typename Translator, typename Box, typename Allocators>
class packer
{
    typedef typename rtree::node<Value, typename Options::parameters_type, Box, Allocators, typename Options::node_tag>::type node;
    typedef typename rtree::internal_node<Value, typename Options::parameters_type, Box, Allocators, typename Options::node_tag>::type internal_node;
    typedef typename rtree::leaf<Value, typename Options::parameters_type, Box, Allocators, typename Options::node_tag>::type leaf;

    typedef typename Allocators::node_pointer node_pointer;

    typedef typename traits::point_type<Box>::type point_type;
    typedef typename traits::coordinate_type<point_type>::type coordinate_type;
    typedef typename detail::default_content_result<Box>::type content_type;
    typedef typename Options::parameters_type parameters_type;
    static const std::size_t dimension = traits::dimension<point_type>::value;

    typedef typename rtree::container_from_elements_type<
        typename rtree::elements_type<leaf>::type,
        std::size_t
    >::type values_counts_container;

    typedef typename rtree::elements_type<internal_node>::type internal_elements;
    typedef typename internal_elements::value_type internal_element;

public:
    packer(parameters_type const& p, Translator const& tr, Allocators & al) : m_parameters(p), m_translator(tr), m_allocators(al) {}

    // Arbitrary iterators
    template <typename InIt>
    node_pointer pack(InIt first, InIt last)
    {
        typedef std::pair<point_type, InIt> entry_type;
        std::vector<entry_type> entries;

        // TODO if RandomAccess
        // count = distance(first, last);
        // reserve(count);

        Box hint_box;
        geometry::assign_inverse(hint_box);

        std::size_t count = 0;
        for ( ; first != last ; ++first, ++count )
        {
            geometry::expand(hint_box, m_translator(*first));

            point_type pt;
            geometry::centroid(m_translator(*first), pt);
            entries.push_back(std::make_pair(pt, first));
        }

        subtree_elements_counts subtree_counts = calculate_subtree_elements_counts(count);
        internal_element el = per_level(entries.begin(), entries.end(), hint_box, count, subtree_counts);
        return el.second;
    }

private:
    struct subtree_elements_counts
    {
        subtree_elements_counts(std::size_t ma, std::size_t mi) : maxc(ma), minc(mi) {}
        std::size_t maxc;
        std::size_t minc;
    };

    template <typename EIt>
    internal_element per_level(EIt first, EIt last, Box const& hint_box, std::size_t count, subtree_elements_counts const& subtree_counts)
    {
        // remove it later
        BOOST_ASSERT(first <= last); BOOST_ASSERT(last - first == typename std::iterator_traits<EIt>::difference_type(count));

        if ( subtree_counts.maxc <= 1 )
        {
            // ROOT or LEAF
            BOOST_ASSERT(count <= m_parameters.get_max_elements());
            // if !root check m_parameters.get_min_elements() <= count

            // create new leaf node
            node_pointer n = rtree::create_node<Allocators, leaf>::apply(m_allocators);                     // MAY THROW (A)
            leaf & l = rtree::get<leaf>(*n);
            // reserve space for values
            rtree::elements(l).reserve(count);                                                              // MAY THROW (A)
            // calculate values box and copy values
            Box elements_box;
            geometry::assign_inverse(elements_box);
            for ( ; first != last ; ++first )
            {
                rtree::elements(l).push_back(*(first->second));                                             // MAY THROW (C)
                geometry::expand(elements_box, m_translator(*(first->second)));
            }
            return internal_element(elements_box, n);
        }

        // calculate next max and min subtree counts
        subtree_elements_counts next_subtree_counts = subtree_counts;
        next_subtree_counts.maxc /= m_parameters.get_max_elements();
        next_subtree_counts.minc /= m_parameters.get_max_elements();

        // create new internal node
        node_pointer n = rtree::create_node<Allocators, internal_node>::apply(m_allocators);                // MAY THROW (A)
        internal_node & in = rtree::get<internal_node>(*n);
        // reserve space for values
        rtree::elements(in).reserve(calculate_nodes_count(count, subtree_counts));                          // MAY THROW (A)
        // calculate values box and copy values
        Box elements_box;
        geometry::assign_inverse(elements_box);

        per_level_packets(first, last,
                          hint_box,
                          subtree_counts,
                          next_subtree_counts,
                          rtree::elements(in), elements_box);

        return internal_element(elements_box, n);
    }

    template <typename EIt>
    void per_level_packets(EIt first, EIt last,
                           Box const& hint_box,
                           subtree_elements_counts const& subtree_counts,
                           subtree_elements_counts const& next_subtree_counts,
                           internal_elements & elements, Box & elements_box)
    {
        std::size_t values_count = last - first;

        BOOST_ASSERT_MSG( subtree_counts.minc <= values_count );

        // only one packet
        if ( values_count <= subtree_counts.maxc )
        {
            // the end, move to the next level
            internal_element el = per_level(first, last, hint_box, values_count, next_subtree_counts);
            // this container should have memory allocated, reserve() called outside
            elements.push_back(el);                                                                         // SHOULDN'T THROW (C)
            geometry::expand(elements_box, el.first);
            return;
        }
        
        EIt median = first + calculate_median_count(values_count, subtree_counts);

        coordinate_type greatest_length;
        std::size_t greatest_dim_index = 0;
        pack_utils::biggest_edge<dimension>::apply(hint_box, greatest_length, greatest_dim_index);
        Box left, right;
        pack_utils::partial_sort_and_half_boxes<0, dimension>
            ::apply(first, median, last, hint_box, left, right, greatest_dim_index);
        
        per_level_packets(first, median, left, subtree_counts, next_subtree_counts, elements, elements_box);
        per_level_packets(median, last, right, subtree_counts, next_subtree_counts, elements, elements_box);
    }

    subtree_elements_counts calculate_subtree_elements_counts(std::size_t elements_count)
    {
        subtree_elements_counts res(1, 1);

        std::size_t smax = m_parameters.get_max_elements();
        for ( ; smax < elements_count ; smax *= m_parameters.get_max_elements() )
            res.maxc = smax;

        res.minc = m_parameters.get_min_elements() * (res.maxc / m_parameters.get_max_elements());

        return res;
    }

    std::size_t calculate_nodes_count(std::size_t count,
                                      subtree_elements_counts const& subtree_counts)
    {
        std::size_t n = count / subtree_counts.maxc;
        std::size_t r = count % subtree_counts.maxc;

        if ( 0 < r && r < subtree_counts.minc )
        {
            std::size_t count_minus_min = count - subtree_counts.minc;
            n = count_minus_min / subtree_counts.maxc;
            r = count_minus_min % subtree_counts.maxc;
            ++n;
        }

        if ( 0 < r )
            ++n;

        return n;
    }

    std::size_t calculate_median_count(std::size_t count,
                                       subtree_elements_counts const& subtree_counts)
    {
        // e.g. for max = 5, min = 2, count = 52, subtree_max = 25, subtree_min = 10

        std::size_t n = count / subtree_counts.maxc; // e.g. 52 / 25 = 2
        std::size_t r = count % subtree_counts.maxc; // e.g. 52 % 25 = 2
        std::size_t median_count = (n / 2) * subtree_counts.maxc; // e.g. 2 / 2 * 25 = 25

        if ( 0 != r ) // e.g. 0 != 2
        {
            if ( subtree_counts.minc <= r ) // e.g. 10 <= 2 == false
            {
                //BOOST_ASSERT_MSG(0 < n, "unexpected value");
                median_count = ((n+1)/2) * subtree_counts.maxc; // if calculated ((2+1)/2) * 25 which would be ok, but not in all cases
            }
            else // r < subtree_counts.second  // e.g. 2 < 10 == true
            {
                std::size_t count_minus_min = count - subtree_counts.minc; // e.g. 52 - 10 = 42
                n = count_minus_min / subtree_counts.maxc; // e.g. 42 / 25 = 1
                r = count_minus_min % subtree_counts.maxc; // e.g. 42 % 25 = 17
                if ( r == 0 )                               // e.g. false
                {
                    // n can't be equal to 0 because then there wouldn't be any element in the other node
                    //BOOST_ASSERT_MSG(0 < n, "unexpected value");
                    median_count = ((n+1)/2) * subtree_counts.maxc;     // if calculated ((1+1)/2) * 25 which would be ok, but not in all cases
                }
                else
                {
                    if ( n == 0 )                                        // e.g. false
                        median_count = r;                                // if calculated -> 17 which is wrong!
                    else
                        median_count = ((n+2)/2) * subtree_counts.maxc; // e.g. ((1+2)/2) * 25 = 25
                }
            }
        }

        return median_count;
    }

    parameters_type const& m_parameters;
    Translator const& m_translator;
    Allocators & m_allocators;
};

}}}}} // namespace boost::geometry::index::detail::rtree

#endif // BOOST_GEOMETRY_INDEX_DETAIL_RTREE_PACK_CREATE_HPP
