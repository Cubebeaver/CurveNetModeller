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

public:
    RotationMinimizingFrame(std::shared_ptr<ICurve> c, const glm::vec3& initialNormal)
        : curve(std::move(c)), startNormal(initialNormal) {}

    // Legenerálja az RMF kereteket a görbe mentén egy megadott felbontással
    std::vector<RMFFrame> GenerateFrames(int resolution) const {
        if (!curve || resolution < 2) return {};

        std::vector<RMFFrame> frames(resolution);
        float t_step = 1.0f / (resolution - 1);

        // --- Kezdeti keret (U_0) beállítása ---
        glm::vec3 x0 = curve->EvaluatePosition(0.0f);
        glm::vec3 t0 = glm::normalize(curve->EvaluateVelocity(0.0f));

        // Gram-Schmidt ortogonalizáció: biztosítjuk, hogy a startNormal
        // merőleges legyen a kezdő érintőre.
        glm::vec3 r0 = glm::normalize(startNormal - glm::dot(startNormal, t0) * t0);
        glm::vec3 s0 = glm::cross(t0, r0);

        frames[0] = { x0, r0, s0, t0 };

        // --- Double Reflection Algoritmus Iterációja ---
        for (int i = 0; i < resolution - 1; ++i) {
            float t_next = (i + 1) * t_step;

            // Következő pont és érintő lekérése a görbétől
            glm::vec3 x_next = curve->EvaluatePosition(t_next);
            glm::vec3 t_next_vec = glm::normalize(curve->EvaluateVelocity(t_next));

            const glm::vec3& x_curr = frames[i].position;
            const glm::vec3& t_curr = frames[i].t;
            const glm::vec3& r_curr = frames[i].r;

            // 1) v1 := x_{i+1} - x_i
            glm::vec3 v1 = x_next - x_curr;

            // 2) c1 := v1 . v1
            float c1 = glm::dot(v1, v1);

            // Nullosztás elleni védelem: ha a pontok túl közel vannak, átlépjük a tükrözést
            if (c1 < 1e-6f) {
                frames[i + 1] = { x_next, r_curr, glm::cross(t_next_vec, r_curr), t_next_vec };
                continue;
            }

            // 3) r_i^L := r_i - (2/c1) * (v1 . r_i) * v1
            glm::vec3 r_i_L = r_curr - (2.0f / c1) * glm::dot(v1, r_curr) * v1;

            // 4) t_i^L := t_i - (2/c1) * (v1 . t_i) * v1
            glm::vec3 t_i_L = t_curr - (2.0f / c1) * glm::dot(v1, t_curr) * v1;

            // 5) v2 := t_{i+1} - t_i^L
            glm::vec3 v2 = t_next_vec - t_i_L;

            // 6) c2 := v2 . v2
            float c2 = glm::dot(v2, v2);

            glm::vec3 r_next;
            if (c2 < 1e-6f) {
                // Ha az érintők egybeesnek, a második tükrözés elhagyható
                r_next = r_i_L;
            } else {
                // 7) r_{i+1} := r_i^L - (2/c2) * (v2 . r_i^L) * v2
                r_next = r_i_L - (2.0f / c2) * glm::dot(v2, r_i_L) * v2;
            }

            // Normalizálás a lebegőpontos hibák felhalmozódásának elkerülésére
            r_next = glm::normalize(r_next);

            // 8) s_{i+1} := t_{i+1} x r_{i+1}
            glm::vec3 s_next = glm::cross(t_next_vec, r_next);

            // 9) U_{i+1} := (r_{i+1}, s_{i+1}, t_{i+1})
            frames[i + 1] = { x_next, r_next, s_next, t_next_vec };
        }

        return frames;
    }
};