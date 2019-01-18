/*
Copyright (c) 2010, Yuuki Takano <ytakanoster@gmail.com>, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef RADIX_TREE_NODE_HPP
#define RADIX_TREE_NODE_HPP

#include <map>
#include <functional>

template <typename K, typename T, typename Compare>
class radix_tree_node {
    friend class radix_tree<K, T, Compare>;
    friend class radix_tree_it<K, T, Compare>;

    typedef std::pair<const K, T> value_type;
    typedef typename std::map<K, radix_tree_node<K, T, Compare>*, Compare >::iterator it_child;

private:
	radix_tree_node(Compare& pred) : m_children(std::map<K, radix_tree_node<K, T, Compare>*, Compare>(pred)), m_parent(NULL), m_value(NULL), m_depth(0), m_is_leaf(false), m_key(), m_pred(pred) { }
    radix_tree_node(const value_type &val, Compare& pred);
    radix_tree_node(const radix_tree_node&); // delete
    radix_tree_node& operator=(const radix_tree_node&); // delete

    ~radix_tree_node();

    std::map<K, radix_tree_node<K, T, Compare>*, Compare> m_children;
    radix_tree_node<K, T, Compare> *m_parent;
    value_type *m_value;
    int m_depth;
    bool m_is_leaf;
    K m_key;
	Compare& m_pred;
};

template <typename K, typename T, typename Compare>
radix_tree_node<K, T, Compare>::radix_tree_node(const value_type &val, Compare& pred) :
    m_children(std::map<K, radix_tree_node<K, T, Compare>*, Compare>(pred)),
    m_parent(NULL),
    m_value(NULL),
    m_depth(0),
    m_is_leaf(false),
    m_key(),
	m_pred(pred)
{
    m_value = new value_type(val);
}

template <typename K, typename T, typename Compare>
radix_tree_node<K, T, Compare>::~radix_tree_node()
{
    it_child it;
    for (it = m_children.begin(); it != m_children.end(); ++it) {
        delete it->second;
    }
    delete m_value;
}


#endif // RADIX_TREE_NODE_HPP
