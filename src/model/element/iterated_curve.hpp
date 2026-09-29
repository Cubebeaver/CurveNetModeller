#include "i_Curve.hpp"

class IteratedCurve : public ICurve {
public:
    //TODO! VAGY WEAK???? de esélyes hogy shared, mert ha ez az iterátor le fogja cserélni az eredetit,
    //      akkor ennek kell majd életben tartania az eredetit.
    //      VAGY INKÁBB WEAK!!! mert a curve mesh az eredetit fogja eltárolni, max a felület akar majd
    std::shared_ptr<ICurve> curve;
    float from = 0;
    float to = 1;



    void InitializeAfterLoad() override { }
    glm::vec3 EvaluatePosition(float t) const override {
        float newT = Lerp(from, to, t);
        return curve->EvaluatePosition(newT);
    }
    float EvaluateCurveCurvature(float t) const override {
        float newT = Lerp(from, to, t);
        return curve->EvaluateCurveCurvature(newT);
    }
    glm::vec3 EvaluateCurvePrincipalNormal(float t) const override {
        float newT = Lerp(from, to, t);
        return curve->EvaluateCurvePrincipalNormal(newT);
    }

private:
    template<typename T>
    T static Lerp(T a, T b, T t) {
        return a + (b - a) * t;
    }
};