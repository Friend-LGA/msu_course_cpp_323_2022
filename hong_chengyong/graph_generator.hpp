#pragma once

#include <mutex>
#include "graph.hpp"

namespace uni_course_cpp {
class GraphGenerator {
 public:
  struct Params {
    explicit Params(int _depth = 0, int _new_vertexes_num = 0)
        : depth(_depth), new_vertexes_num(_new_vertexes_num) {}

    const int depth;
    const int new_vertexes_num;
  };

  explicit GraphGenerator(const Params& params = Params()) : params_(params) {}
  Graph generate() const;

 private:
  Graph generateMainBody() const;
  void generateGrayBranch(Graph& graph,
                          int branch_depth,
                          const VertexId& root_id,
                          std::mutex& mutex) const;
  const Params params_ = Params();
};
}  // namespace uni_course_cpp