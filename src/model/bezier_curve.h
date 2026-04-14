#pragma once

#include <vector>

#include "model/bezier_node.h"
#include "i_Curve.hpp"
#include "i_Spline.hpp"

#include <glm/glm.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>



class BezierCurve : public ISpline {
private:
    std::vector<std::shared_ptr<BezierNode>> Nodes;
public:
    Event<> BezierCurveChanged;
    
    int GetSegmentCount() const;
    const std::vector<std::shared_ptr<BezierNode>>& GetNodes() const { return Nodes; }

    void AddNode(std::shared_ptr<BezierNode> node);

    void RemoveNodeAt(int idx);
    void RemoveNodeLast() { RemoveNodeAt(Nodes.size() - 1); }

    [[nodiscard]] glm::vec3 EvaluateSegment(int segmentIndex, float t) const override;
    [[nodiscard]] float EvaluateSegmentCurvature(int segmentIndex, float t) const override;
    [[nodiscard]] glm::vec3 EvaluateSegmentPrincipalNormal(int segmentIndex, float t) const override;
    [[nodiscard]] glm::vec3 EvaluateSegmentCameraNormal(int segmentIndex, float t, glm::vec3 cam) const;

    [[nodiscard]] glm::vec3 EvaluatePosition(float t) const override;
    [[nodiscard]] float EvaluateCurveCurvature(float t) const override;
    [[nodiscard]] glm::vec3 EvaluateCurvePrincipalNormal(float t) const override;

    [[nodiscard]] std::vector<glm::vec3> GenerateRenderNormals(int resolution = 50) const;
    [[nodiscard]] std::vector<glm::vec3> GenerateRenderCameraNormals(int resolution = 50, glm::vec3 cam = glm::vec3(0, 0, 1)) const;
    [[nodiscard]] std::vector<float> GenerateRenderCurvatures(int resolution = 50) const;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(Nodes));
    }
};