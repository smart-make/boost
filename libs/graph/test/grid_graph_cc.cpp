//=======================================================================
// Copyright 2009 Trustees of Indiana University.
// Authors: Michael Hansen, Andrew Lumsdaine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include <boost/graph/graph_archetypes.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/grid_graph.hpp>
#include <boost/concept/assert.hpp>

#define DIMENSIONS 3
using namespace boost;

int main (int, char*[]) {

  typedef grid_graph<DIMENSIONS> Graph;
  typedef graph_traits<Graph>::vertex_descriptor Vertex;
  typedef graph_traits<Graph>::edge_descriptor Edge;

  BOOST_CONCEPT_ASSERT((BidirectionalGraphConcept<Graph> ));
  BOOST_CONCEPT_ASSERT((VertexListGraphConcept<Graph> ));
  BOOST_CONCEPT_ASSERT((EdgeListGraphConcept<Graph> ));
  BOOST_CONCEPT_ASSERT((IncidenceGraphConcept<Graph> ));
  BOOST_CONCEPT_ASSERT((AdjacencyGraphConcept<Graph> ));
  BOOST_CONCEPT_ASSERT((AdjacencyMatrixConcept<Graph> ));
  BOOST_CONCEPT_ASSERT((ReadablePropertyGraphConcept<Graph, Vertex, vertex_index_t> ));
  BOOST_CONCEPT_ASSERT((ReadablePropertyGraphConcept<Graph, Edge, edge_index_t> ));

  return (0);
}
