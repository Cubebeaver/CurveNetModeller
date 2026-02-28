#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "model/bezier_node.h"



class BezierCurve {
public:
    std::vector<BezierNode> Nodes;
    
    int GetSegmentCount() const;

    void AddNode(const BezierNode& node);
    void AddNode(const glm::vec3& position);

    void RemoveNode(const BezierNode& node);

    glm::vec3 EvaluateSegment(int segmentIndex, float t) const;
    std::vector<glm::vec3> GenerateRenderPoints(int resolution = 50) const;
};