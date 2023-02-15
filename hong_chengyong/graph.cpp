#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using VertexId = int;
using EdgeId = int;

struct Vertex
{
    const VertexId id;

    explicit Vertex(const VertexId &_id) : id(_id) {}
};

struct Edge
{
    enum class Colors
    {
        Grey,
        Green,
        Blue,
        Yellow,
        Red
    };

    const EdgeId id;
    const VertexId vertex_id1;
    const VertexId vertex_id2;
    const Colors color;
    explicit Edge(const EdgeId &_id,
                  const VertexId &_vertex_id1,
                  const VertexId &_vertex_id2,
                  const Colors &_color)
        : id(_id),
          vertex_id1(_vertex_id1),
          vertex_id2(_vertex_id2),
          color(_color) {}
};

class Graph
{
public:
    bool hasVertex(const VertexId &vertex_id) const
    {
        for (const auto &vertex : vertexes_)
        {
            if (vertex.id == vertex_id)
            {
                return true;
            }
        }
        return false;
    }

    bool hasEdge(const EdgeId &edge_id) const
    {
        for (const auto &edge : edges_)
        {
            if (edge.id == edge_id)
            {
                return true;
            }
        }
        return false;
    }

    bool areConnected(const VertexId &vertex_id1,
                      const VertexId &vertex_id2) const
    {
        assert(hasVertex(vertex_id1) && "Vertex1 index is out of range");
        assert(hasVertex(vertex_id2) && "Vertex2 index is out of range");
        if (vertex_id1 == vertex_id2)
        {
            for (const auto &edge_id : connection_list_.at(vertex_id1))
            {
                const auto &edge = getEdge(edge_id);
                if (edge.vertex_id1 == vertex_id1 && edge.vertex_id2 == vertex_id1)
                {
                    return true;
                }
            }
        }
        else
        {
            for (const auto &edge_id : connection_list_.at(vertex_id1))
            {
                const auto &edge = getEdge(edge_id);
                if (edge.vertex_id1 == vertex_id2 || edge.vertex_id2 == vertex_id2)
                {
                    return true;
                }
            }
        }
        return false;
    }

    VertexId addVertex()
    {
        const auto &new_vertex = vertexes_.emplace_back(getNewVertexId());
        connection_list_.insert({new_vertex.id, std::vector<EdgeId>()});
        layers_list_[0].push_back(new_vertex.id);
        vertexes_depths_[new_vertex.id] = 0;
        return new_vertex.id;
    }

    Edge::Colors calculateEdgeColor(const VertexId &vertex_id1,
                                    const VertexId &vertex_id2) const
    {
        if (connection_list_.at(vertex_id2).size() == 0 ||
            connection_list_.at(vertex_id1).size() == 0)
        {
            return Edge::Colors::Grey;
        }
        else if (vertex_id1 == vertex_id2)
        {
            return Edge::Colors::Green;
        }
        else if (vertexDepth(vertex_id2) == vertexDepth(vertex_id1))
        {
            return Edge::Colors::Blue;
        }
        else if (std::abs(vertexDepth(vertex_id2) - vertexDepth(vertex_id1)) ==
                 1)
        {
            return Edge::Colors::Yellow;
        }
        else
        {
            return Edge::Colors::Red;
        }
    }

    void greyEdgeInitialization(const VertexId &vertex_id1,
                                const VertexId &vertex_id2)
    {
        int new_depth = vertexes_depths_[std::min(vertex_id1, vertex_id2)] + 1;
        vertexes_depths_[std::max(vertex_id1, vertex_id2)] = new_depth;
        depth_ = std::max(depth_, new_depth);
        layers_list_[new_depth].push_back(std::max(vertex_id1, vertex_id2));
        for (auto it = layers_list_[0].begin(); it != layers_list_[0].end(); it++)
        {
            if (*it == std::max(vertex_id1, vertex_id2))
            {
                layers_list_[0].erase(it);
                break;
            }
        }
    }

    void addEdge(const VertexId &vertex_id1, const VertexId &vertex_id2)
    {
        assert(hasVertex(vertex_id1) && "Vertex1 index is out of range");
        assert(hasVertex(vertex_id2) && "Vertex2 index is out of range");
        assert(!areConnected(vertex_id1, vertex_id2) &&
               "These vertexes are already connected");
        auto color = calculateEdgeColor(vertex_id1, vertex_id2);
        if (color == Edge::Colors::Grey)
        {
            greyEdgeInitialization(vertex_id1, vertex_id2);
        }
        const auto &new_edge =
            edges_.emplace_back(getNewEdgeId(), vertex_id1, vertex_id2, color);
        connection_list_[vertex_id1].push_back(new_edge.id);
        if (vertex_id1 != vertex_id2)
        {
            connection_list_[vertex_id2].push_back(new_edge.id);
        }
    }

    const Edge &getEdge(const EdgeId &edge_id) const
    {
        assert(hasEdge(edge_id) && "Edge id is out of range.");
        for (const auto &edge : edges_)
        {
            if (edge.id == edge_id)
            {
                return edge;
            }
        }
        throw std::runtime_error("Cannot be reached.");
    }

    const std::vector<EdgeId> &vertexConnections(const VertexId &id) const
    {
        assert(hasVertex(id) && "Vertex id is out of range");
        return connection_list_.at(id);
    }
    const std::vector<Vertex> &vertexes() const { return vertexes_; }
    const std::vector<Edge> &edges() const { return edges_; }
    const std::vector<VertexId> &vertexIdsAtLayer(int depth) const
    {
        assert(depth <= depth_ && "Graph is not that deep");
        return layers_list_.at(depth);
    }
    int vertexDepth(const VertexId &vertex_id) const
    {
        assert(hasVertex(vertex_id) && "Vertex id is out of range");
        return vertexes_depths_.at(vertex_id);
    }
    int depth() const { return depth_; }

private:
    std::vector<Vertex> vertexes_;
    std::vector<Edge> edges_;
    int depth_ = 0;
    int vertex_new_id_ = 0, edge_new_id_ = 0;
    std::unordered_map<VertexId, std::vector<EdgeId>> connection_list_;
    std::unordered_map<int, std::vector<VertexId>> layers_list_;
    std::unordered_map<VertexId, int> vertexes_depths_;
    VertexId getNewVertexId() { return vertex_new_id_++; }
    EdgeId getNewEdgeId() { return edge_new_id_++; }
};

constexpr float GREEN_PROBABILITY = 0.1, BLUE_PROBABILITY = 0.25,
                RED_PROBABILITY = 0.33;

class GraphGenerator
{
public:
    struct Params
    {
        explicit Params(int _depth = 0, int _new_vertexes_num = 0)
            : depth(_depth), new_vertexes_num(_new_vertexes_num) {}

        const int depth;
        const int new_vertexes_num;
    };

    explicit GraphGenerator(const Params &params = Params()) : params_(params) {}

    Graph generateMainBody() const
    {
        Graph graph;
        float step = 1.0 / params_.depth;
        graph.addVertex();
        for (int current_depth = 0; current_depth < params_.depth;
             current_depth++)
        {
            bool vertexes_generated = false;
            const auto previous_layer = graph.vertexIdsAtLayer(
                current_depth); // ссылка ломается при вызове addVertex()
            for (const auto &vertex_id : previous_layer)
            {
                for (int j = 0; j < params_.new_vertexes_num; j++)
                {
                    if (randomValue(getGreyProbability(step, current_depth)))
                    {
                        graph.addEdge(vertex_id, graph.addVertex());
                        vertexes_generated = true;
                    }
                }
            }
            if (!vertexes_generated)
            {
                break;
            }
        }
        if (graph.depth() < params_.depth)
        {
            std::cout << "Depth of the graph is less than given. Depth is "
                      << graph.depth() << std::endl;
        }
        return graph;
    }

    void generateColorEdges(Graph &graph) const
    {
        for (const auto &vertex : graph.vertexes())
        {
            int vertex_depth = graph.vertexDepth(vertex.id);
            if (randomValue(GREEN_PROBABILITY))
            {
                graph.addEdge(vertex.id, vertex.id);
            }
            const auto &next_vertex_id = vertex.id + 1;
            if (randomValue(BLUE_PROBABILITY) && graph.hasVertex(next_vertex_id) &&
                graph.vertexDepth(next_vertex_id) == vertex_depth)
            {
                graph.addEdge(vertex.id, next_vertex_id);
            }
            if (vertex_depth < graph.depth() &&
                randomValue(getYellowProbability(graph, vertex.id)))
            {
                std::vector<VertexId> next_layer;
                for (const auto &vertex_id : graph.vertexIdsAtLayer(vertex_depth + 1))
                {
                    if (!graph.areConnected(vertex.id, vertex_id))
                    {
                        next_layer.push_back(vertex_id);
                    }
                }
                graph.addEdge(vertex.id, getRandomVertexId(next_layer));
            }
            if (randomValue(RED_PROBABILITY) && vertex_depth < (graph.depth() - 1))
            {
                graph.addEdge(vertex.id, getRandomVertexId(
                                             graph.vertexIdsAtLayer(vertex_depth + 2)));
            }
        }
    }

    Graph generate() const
    {
        Graph graph = generateMainBody();
        generateColorEdges(graph);
        return graph;
    }

private:
    const Params params_ = Params();
    bool randomValue(float probability) const
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::bernoulli_distribution distribution(probability);
        return distribution(gen);
    }
    int getRandomVertexId(const std::vector<VertexId> vertex_ids) const
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> random_number(0, vertex_ids.size() - 1);
        return vertex_ids.at(random_number(gen));
    }
    float getGreyProbability(float step, int depth) const
    {
        return 1.0 - step * depth;
    }
    float getYellowProbability(const Graph &graph,
                               const VertexId &vertex_id) const
    {
        return 1.0 * graph.vertexDepth(vertex_id) / (graph.depth() - 1);
    }
};

class GraphPrinter
{
public:
    explicit GraphPrinter(const Graph &graph) : graph_(graph) {}
    std::string print() const
    {
        std::string graph_string;
        graph_string += "{\n \"vertices\": [\n  ";
        for (const auto &vertex : graph_.vertexes())
        {
            graph_string += printVertex(vertex.id);
        }
        if (graph_.vertexes().size() > 0)
        {
            graph_string.pop_back();
            graph_string.pop_back();
        }
        graph_string += "\n ],\n \"edges\": [\n  ";
        for (const auto &edge : graph_.edges())
        {
            graph_string += printEdge(edge);
        }
        if (graph_.edges().size() > 0)
        {
            graph_string.pop_back();
            graph_string.pop_back();
        }
        graph_string += "\n ]\n}\n";
        return graph_string;
    }
    std::string printVertex(const VertexId &id) const
    {
        std::string vertex_string =
            "{\n   \"id\": " + std::to_string(id) + ",\n   \"edge_ids\": [";
        for (const auto &edge_id : graph_.vertexConnections(id))
        {
            vertex_string += std::to_string(edge_id) + ", ";
        }
        if (graph_.vertexConnections(id).size() > 0)
        {
            vertex_string.pop_back();
            vertex_string.pop_back();
        }
        vertex_string +=
            "],\n   \"depth\": " + std::to_string(graph_.vertexDepth(id)) +
            "\n  }, ";
        return vertex_string;
    }
    std::string printEdge(const Edge &edge) const
    {
        std::string edge_string = "{\n   \"id\": " + std::to_string(edge.id);
        edge_string += ",\n   \"vertex_ids\": [";
        edge_string += std::to_string(edge.vertex_id1) + ", ";
        edge_string += std::to_string(edge.vertex_id2) + "],\n   ";
        edge_string += "\"color\": " + colorToString(edge) + "\n  }, ";
        return edge_string;
    }

private:
    const Graph &graph_;
    std::string colorToString(const Edge &edge) const
    {
        switch (edge.color)
        {
        case Edge::Colors::Grey:
            return "\"gray\"";
        case Edge::Colors::Green:
            return "\"green\"";
        case Edge::Colors::Blue:
            return "\"blue\"";
        case Edge::Colors::Yellow:
            return "\"yellow\"";
        case Edge::Colors::Red:
            return "\"red\"";
        }
    }
};

void write_to_file(const std::string &string, const std::string &file_name)
{
    std::ofstream file(file_name);
    file << string;
    file.close();
}

int main()
{
    int depth, new_vertexes_num;
    std::cout << "Enter depth:" << std::endl;
    std::cin >> depth;
    while (depth < 0)
    {
        std::cout << "Depth must be not negative. Enter depth again:" << std::endl;
        std::cin >> depth;
    }
    std::cout << "Enter number of new vertices:" << std::endl;
    std::cin >> new_vertexes_num;
    while (depth < 0)
    {
        std::cout << "Number of new vertices must be not negative. Enter number of "
                     "new vertexes again:"
                  << std::endl;
        std::cin >> new_vertexes_num;
    }
    const GraphGenerator::Params params =
        GraphGenerator::Params(depth, new_vertexes_num);
    const Graph graph = GraphGenerator(params).generate();
    const auto graph_printer = GraphPrinter(graph);
    const auto graph_json = graph_printer.print();
    std::cout << graph_json << std::endl;
    write_to_file(graph_json, "graph.json");
    return 0;
}
