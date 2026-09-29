#pragma once

#include <vector>

#include "bezier_node.h"
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

    // Segédfüggvények a kódduplikáció elkerülésére
    bool GetSegmentControlPoints(int segmentIndex, glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3) const;
    void GetLocalT(float t, int& segmentIdx, float& localT) const;

    template<typename T, typename Func>
    std::vector<T> GenerateRenderData(int resolution, const T& fallback, Func evaluator) const {
        std::vector<T> result;
        int segments = GetSegmentCount();

        if (segments == 0 && !Nodes.empty()) {
            result.push_back(fallback);
            return result;
        }

        result.reserve(segments * resolution);

        for (int i = 0; i < segments; ++i) {
            int steps = (i == segments - 1) ? resolution : resolution - 1;

            for (int step = 0; step <= steps; ++step) {
                float localT = static_cast<float>(step) / static_cast<float>(resolution);
                result.push_back(evaluator(i, localT));
            }
        }

        return result;
    }

public:
    virtual int GetSegmentCount() const override;
    const std::vector<std::shared_ptr<BezierNode>>& GetNodes() const { return Nodes; }

    void AddNode(std::shared_ptr<BezierNode> node);
    void AddNodeAt(std::shared_ptr<BezierNode> node, int index);

    void RemoveNodeAt(int idx);
    void RemoveNodeLast() { RemoveNodeAt(Nodes.size() - 1); }
    void RemoveNode(std::weak_ptr<BezierNode> node);

    int IndexOf(std::weak_ptr<BezierNode> node) const;

    // ISpline override-ok
    glm::vec3 EvaluateSegment(int segmentIndex, float t) const override;
    glm::vec3 EvaluateSegmentVelocity(int segmentIndex, float t) const override;
    glm::vec3 EvaluateSegmentAcceleration(int segmentIndex, float t) const override;
    float EvaluateSegmentCurvature(int segmentIndex, float t) const override;
    glm::vec3 EvaluateSegmentPrincipalNormal(int segmentIndex, float t) const override;

    // Extrák a Bezier-hez
    glm::vec3 EvaluateSegmentCameraNormal(int segmentIndex, float t, glm::vec3 cam) const;

    // ICurve override-ok
    glm::vec3 EvaluatePosition(float t) const override;
    glm::vec3 EvaluateVelocity(float t) const override;
    glm::vec3 EvaluateAcceleration(float t) const override;
    float EvaluateCurveCurvature(float t) const override;
    glm::vec3 EvaluateCurvePrincipalNormal(float t) const override;

    // Generátorok
    std::vector<glm::vec3> GenerateRenderPoints(int resolution = 50) const;
    std::vector<glm::vec3> GenerateRenderNormals(int resolution = 50) const;
    std::vector<glm::vec3> GenerateRenderCameraNormals(int resolution = 50, glm::vec3 cam = glm::vec3(0, 0, 1)) const;
    std::vector<float> GenerateRenderCurvatures(int resolution = 50) const;

    void InitializeAfterLoad() override;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(Nodes));
    }

    virtual ~BezierCurve();

private:
    void OnChange();
};