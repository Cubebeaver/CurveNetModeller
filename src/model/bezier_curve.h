#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "model/bezier_node.h"

#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>



class BezierCurve {
public:
    std::vector<BezierNode> Nodes;

    Event<> BezierCurveChanged;
    
    int GetSegmentCount() const;

    void AddNode(const BezierNode& node);
    void AddNode(const glm::vec3& position);

    void RemoveNode(const BezierNode& node);

    glm::vec3 EvaluateSegment(int segmentIndex, float t) const;
    float EvaluateSegmentCurvature(int segmentIndex, float t) const;
    glm::vec3 EvaluateSegmentPrincipalNormal(int segmentIndex, float t) const;
    glm::vec3 EvaluateSegmentCameraNormal(int segmentIndex, float t, glm::vec3 cam) const;

    glm::vec3 EvaluateCurve(float t) const;
    float EvaluateCurveCurvature(float t) const;
    glm::vec3 EvaluateCurvePrincipalNormal(float t) const;

    std::vector<glm::vec3> GenerateRenderPoints(int resolution = 50) const;
    std::vector<glm::vec3> GenerateRenderNormals(int resolution = 50) const;
    std::vector<glm::vec3> GenerateRenderCameraNormals(int resolution = 50, glm::vec3 cam = glm::vec3(0, 0, 1)) const;
    std::vector<float> GenerateRenderCurvatures(int resolution = 50) const;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(Nodes));
    }
};