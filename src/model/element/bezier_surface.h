#pragma once
#include <memory>

#include "i_Node.hpp"
#include "i_Surface.hpp"
#include "point.h"

class BezierSurface : public ISurface {
public:
    // A kontrollpontok rácsa (N x M-es mátrix)
    // i (sorok) felelnek meg az 'u' iránynak, j (oszlopok) a 'v' iránynak
    int N = 0, M = 0;
    std::vector<std::vector<std::shared_ptr<Point>>> controlPoints;

    const std::vector<std::vector<std::shared_ptr<Point>>>& GetPoints() const { return controlPoints; }

    BezierSurface() = default;
    BezierSurface(int N, int M);

    virtual glm::vec3 Evaluate(float u, float v) const override;
    virtual glm::vec3 EvaluateNormal(float u, float v) const override;
    glm::vec3 EvaluateDeCasteljau(float u, float v) const;

    void InitializeAfterLoad() override;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(controlPoints));
    }
private:
    int BinomialCoefficient(int n, int k) const;
    float Bernstein(int n, int i, float t) const;
    glm::vec3 DeCasteljau1D(std::vector<glm::vec3> points, float t) const;

    void OnPointChanged(glm::vec3 offset);
};