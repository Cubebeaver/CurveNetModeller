#include "i_Spline.hpp"
#include "point.h"

class LineSegment : public ICurve {
private:
    std::shared_ptr<Point> start;
    std::shared_ptr<Point> end;

public:
    LineSegment(std::shared_ptr<Point> start, std::shared_ptr<Point> end)
        : start(start), end(end) {
        this->start->PointChanged.AddListener(this, &LineSegment::OnChange);
        this->end  ->PointChanged.AddListener(this, &LineSegment::OnChange);
    }

    virtual ~LineSegment() override {
        start->PointChanged.RemoveListener(this, &LineSegment::OnChange);
        end  ->PointChanged.RemoveListener(this, &LineSegment::OnChange);
    }

    // ICurve override-ok
    glm::vec3 EvaluatePosition(float t) const override {
        return start->GetPosition() + t * (end->GetPosition() - start->GetPosition());
    }
    glm::vec3 EvaluateVelocity(float t) const override {
        return end->GetPosition() - start->GetPosition();
    }
    glm::vec3 EvaluateAcceleration(float t) const override {
        return glm::vec3(0, 0, 0);
    }
    float EvaluateCurveCurvature(float t) const override {
        return 0;
    }
    glm::vec3 EvaluateCurvePrincipalNormal(float t) const override {
        return glm::vec3(0, 0, 0);
    }

    void InitializeAfterLoad() override {
        start->PointChanged.AddListener(this, &LineSegment::OnChange);
        end  ->PointChanged.AddListener(this, &LineSegment::OnChange);
    }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(start), CEREAL_NVP(end));
    }

private:
    void OnChange(glm::vec3 offset) {
        CurveChanged.Invoke();
    }
};
