#include <glm/vec3.hpp>

#include "i_Curve.hpp"
#include "i_Element.hpp"

// A keretet (frame) reprezentáló struktúra az algoritmus alapján
struct RMFFrame {
    glm::vec3 position; // x_i
    glm::vec3 r;        // r_i (referencia normális)
    glm::vec3 s;        // s_i (binormális / bitangent)
    glm::vec3 t;        // t_i (érintő)
};

// aka Normal Fence
// Forrás: https://scispace.com/pdf/computation-of-rotation-minimizing-frames-2ig7mkgj0j.pdf
//       : 7. oldal : pszeudo kód (Table I. Algorithm—Double Reflection)
// Ha szeretnék végpont-kényszert is alkalmazni, akkor ezeket tudom tenni:
//  - Twist Distribution: megmérem a végén az eltérést, és "elcsavarom annyival a végét (lineárisan interpolálva a start fele)"
//  - Dual RMF Blending: Számolok egyet az elejétől a végéig és a végéről az elejéig, és ezeket quaternióként kezelve SLERP-elek köztük
//  - Globális energia-minimalizálás: Egyenletrendszer-megoldód módszer, pontosabb és globális, de számításigények
class RotationMinimizingFrame {
private:
    std::shared_ptr<ICurve> curve;
    glm::vec3 startNormal;
    glm::vec3 endNormal;
    bool useEndNormal;

    // Segédfüggvény egy normálvektor biztonságos merőlegesítéséhez az érintőre
    static glm::vec3 OrthonormalizeNormal(const glm::vec3& normal, const glm::vec3& tangent) {
        glm::vec3 proj = normal - glm::dot(normal, tangent) * tangent;
        if (glm::dot(proj, proj) < 1e-6f) {
            glm::vec3 fallback = (std::abs(tangent.x) < 0.9f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
            proj = fallback - glm::dot(fallback, tangent) * tangent;
        }
        return glm::normalize(proj);
    }

public:
    // Konstruktor csak kezdő normállal
    RotationMinimizingFrame(std::shared_ptr<ICurve> c, const glm::vec3& initialNormal)
        : curve(std::move(c)), startNormal(initialNormal), endNormal(0.0f), useEndNormal(false) {}

    // Konstruktor kezdő ÉS vég normállal
    RotationMinimizingFrame(std::shared_ptr<ICurve> c, const glm::vec3& initialNormal, const glm::vec3& finalNormal)
        : curve(std::move(c)), startNormal(initialNormal), endNormal(finalNormal), useEndNormal(true) {}

    std::vector<RMFFrame> GenerateFrames(int resolution) const {
        if (!curve || resolution < 2) return {};

        std::vector<RMFFrame> frames(resolution);
        std::vector<float> arcLengths(resolution, 0.0f); // Ívhosszak tárolása a sima csavarodáshoz
        float t_step = 1.0f / (resolution - 1);

        glm::vec3 x0 = curve->EvaluatePosition(0.0f);
        glm::vec3 t0 = glm::normalize(curve->EvaluateVelocity(0.0f));
        glm::vec3 r0 = OrthonormalizeNormal(startNormal, t0);
        glm::vec3 s0 = glm::cross(t0, r0);

        frames[0] = { x0, r0, s0, t0 };

        // 1. FÁZIS: Standard Double Reflection RMF
        for (int i = 0; i < resolution - 1; ++i) {
            float t_next = (i + 1) * t_step;

            glm::vec3 x_next = curve->EvaluatePosition(t_next);
            glm::vec3 t_next_vec = glm::normalize(curve->EvaluateVelocity(t_next));

            const glm::vec3& x_curr = frames[i].position;
            const glm::vec3& t_curr = frames[i].t;
            const glm::vec3& r_curr = frames[i].r;

            glm::vec3 v1 = x_next - x_curr;
            float c1 = glm::dot(v1, v1);

            // Kumulatív ívhossz frissítése
            arcLengths[i + 1] = arcLengths[i] + std::sqrt(c1);

            if (c1 < 1e-6f) {
                frames[i + 1] = { x_next, r_curr, glm::cross(t_next_vec, r_curr), t_next_vec };
                continue;
            }

            glm::vec3 r_i_L = r_curr - (2.0f / c1) * glm::dot(v1, r_curr) * v1;
            glm::vec3 t_i_L = t_curr - (2.0f / c1) * glm::dot(v1, t_curr) * v1;

            glm::vec3 v2 = t_next_vec - t_i_L;
            float c2 = glm::dot(v2, v2);

            glm::vec3 r_next = (c2 < 1e-6f)
                ? r_i_L
                : r_i_L - (2.0f / c2) * glm::dot(v2, r_i_L) * v2;

            r_next = glm::normalize(r_next);
            glm::vec3 s_next = glm::cross(t_next_vec, r_next);

            frames[i + 1] = { x_next, r_next, s_next, t_next_vec };
        }

        // 2. FÁZIS: Vég-normálvektor korrekció (ha meg van adva)
        if (useEndNormal && arcLengths.back() > 1e-6f) {
            const RMFFrame& lastFrame = frames.back();

            // A kívánt endNormal bevetítése az utolsó érintő normálsíkjába
            glm::vec3 r_target = OrthonormalizeNormal(endNormal, lastFrame.t);

            // Előjeles szögeltérés az RMF utolsó bázisában (r_n, s_n)
            float cosTheta = glm::dot(r_target, lastFrame.r);
            float sinTheta = glm::dot(r_target, lastFrame.s);
            float totalAngle = std::atan2(sinTheta, cosTheta);

            float totalLength = arcLengths.back();

            // Szögeltérés elosztása az ívhossz mentén
            for (int i = 1; i < resolution; ++i) {
                float weight = arcLengths[i] / totalLength; // Vagy simán: (float)i / (resolution - 1)
                float angle = weight * totalAngle;

                float c = std::cos(angle);
                float s = std::sin(angle);

                glm::vec3 r_old = frames[i].r;
                glm::vec3 s_old = frames[i].s;

                // Forgatás a lokális (r, s) síkban t_i körül
                frames[i].r = c * r_old + s * s_old;
                frames[i].s = -s * r_old + c * s_old;
            }
        }

        return frames;
    }
};