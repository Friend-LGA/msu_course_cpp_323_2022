#include "graph.hpp"
#include <algorithm>
#include <cassert>

Graph::Edge::Color Graph::define_color(Graph::VertexId from_vertex_id,
                                       Graph::VertexId to_vertex_id) const{
    const auto from_vertex_depth = get_vertex_depth(from_vertex_id);
    const auto to_vertex_depth = get_vertex_depth(to_vertex_id);
    Edge::Color color = Edge::Color::Grey;
    if(from_vertex_id == to_vertex_id){
        color = Edge::Color::Green;
    }else if(to_vertex_depth - from_vertex_depth <= 0){
        color = Edge::Color::Grey;
    }else if(to_vertex_depth - from_vertex_depth == 1 &&
             !is_connected(to_vertex_id, from_vertex_id)){
        color = Edge::Color::Yellow;
    }else if(to_vertex_depth - from_vertex_depth == 2){
        color = Edge::Color::Red;
    }else{
        throw std::runtime_error("Can't define the color");
    }
    
    return color;
}


bool Graph::is_connected(Graph::VertexId from_vertex_id,
                         Graph::VertexId to_vertex_id) const{
    if (from_vertex_id == to_vertex_id){
        for(const auto& edge_id : connections_list_.at(from_vertex_id)){
            if(edges_.at(edge_id).color() == Edge::Color::Green){
                return true;
            }
        }
        return false;
    }
    const auto& pull_edges_from = connections_list_.at(from_vertex_id);
    const auto& pull_edges_to = connections_list_.at(to_vertex_id);
    
    std::set<Graph::EdgeId> intersection;
    
    std::set_intersection(pull_edges_from.begin(), pull_edges_from.end(),
                          pull_edges_to.begin(), pull_edges_to.end(),
                          std::inserter(intersection, intersection.begin()));
    return !intersection.empty();
}

void Graph::set_vertex_depth(VertexId id, Depth depth){
    const auto cur_depth = get_vertex_depth(id);
    const auto graph_depth = get_graph_depth();
    
    if(depth > graph_depth){
        std::vector<VertexId> EmptyVertex = {};
        vertices_of_depth_.emplace_back(EmptyVertex);
    }
    
    depth_of_vertices_[id] = depth;
    vertices_of_depth_[depth].emplace_back(id);
    vertices_of_depth_[cur_depth].erase(std::remove(vertices_of_depth_[cur_depth].begin(),
                                                    vertices_of_depth_[cur_depth].end(), id));
}
}
