#ifndef GRAPH_GENERATOR_HPP
#define GRAPH_GENERATOR_HPP

class Graph {
public:
    using VertexId = int;
    using EdgeId = int;

    struct Vertex;
    struct Edge;

    void add_vertex();
    const std::vector<Graph::Vertex>& get_vertices() const;
    const std::vector<Graph::Edge>& get_edges() const;

    void add_edge(VertexId from_vertex_id, VertexId to_vertex_id);

private:
    std::vector<Vertex> graphVertices;
    std::vector<Edge> graphEdges;
};

#endif
