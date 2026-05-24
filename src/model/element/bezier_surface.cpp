#include "bezier_surface.h"

#include "bezier_curve.h"

BezierSurface::BezierSurface(int N, int M) {
    controlPoints.resize(N);
    for (int i = 0; i < N; ++i) {
        controlPoints[i].resize(M);
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            float x = static_cast<float>(i) / (N - 1) * 2 - 1;
            float y = 0;
            float z = static_cast<float>(j) / (M - 1) * 2 - 1;

            controlPoints[i][j] = std::make_shared<Point>(-x, y, z);
            controlPoints[i][j]->PointChanged.AddListener(this, &BezierSurface::OnPointChanged);
        }
    }
}

// --- 1. DIA ALAPJÁN: KIÉRTÉKELÉS BERNSTEIN POLINOMOKKAL ---
// Kiszámolja a felület egy pontját adott (u, v) paramétereknél [0, 1] intervallumon
glm::vec3 BezierSurface::Evaluate(float u, float v) const {
    if (controlPoints.empty() || controlPoints[0].empty()) return glm::vec3(0.0f);

    int n = controlPoints.size() - 1;    // u irányú fokszám
    int m = controlPoints[0].size() - 1; // v irányú fokszám

    glm::vec3 surfacePoint(0.0f);

    for (int i = 0; i <= n; ++i) {
        float Bu = Bernstein(n, i, u);
        for (int j = 0; j <= m; ++j) {
            float Bv = Bernstein(m, j, v);

            glm::vec3 P_ij = controlPoints[i][j]->GetPosition();
            surfacePoint += P_ij * Bu * Bv;
        }
    }

    return surfacePoint;
}

// --- 2. DIA ALAPJÁN: NORMÁLVEKTOR KISZÁMÍTÁSA ---
// Kiszámolja a felület normálvektorát a fényeléshez
glm::vec3 BezierSurface::EvaluateNormal(float u, float v) const {
    if (controlPoints.empty() || controlPoints[0].empty()) return glm::vec3(0.0f, 1.0f, 0.0f);

    int n = controlPoints.size() - 1;
    int m = controlPoints[0].size() - 1;

    glm::vec3 du(0.0f); // Derivált 'u' szerint
    glm::vec3 dv(0.0f); // Derivált 'v' szerint

    // u szerinti derivált (Su)
    if (n > 0) {
        for (int i = 0; i <= n - 1; ++i) {
            float Bu = Bernstein(n - 1, i, u);
            for (int j = 0; j <= m; ++j) {
                float Bv = Bernstein(m, j, v);
                glm::vec3 diff = controlPoints[i + 1][j]->GetPosition() - controlPoints[i][j]->GetPosition();
                du += diff * Bu * Bv;
            }
        }
        du *= n;
    }

    // v szerinti derivált (Sv)
    if (m > 0) {
        for (int i = 0; i <= n; ++i) {
            float Bu = Bernstein(n, i, u);
            for (int j = 0; j <= m - 1; ++j) {
                float Bv = Bernstein(m - 1, j, v);
                glm::vec3 diff = controlPoints[i][j + 1]->GetPosition() - controlPoints[i][j]->GetPosition();
                dv += diff * Bu * Bv;
            }
        }
        dv *= m;
    }

    // Keresztszorzat adja a normálvektort (N = Su x Sv)
    glm::vec3 normal = glm::cross(du, dv);

    // Ha a normálvektor hossza 0 (pl. egybeeső pontoknál), visszaadunk egy default vektort
    if (glm::length(normal) < 0.0001f) return glm::vec3(0.0f, 1.0f, 0.0f);

    return glm::normalize(normal);
}

// --- 3. DIA ALAPJÁN: DE CASTELJAU ALGORITMUS (Alternatív kiértékelés) ---
// Ezt akkor érdemes használni, ha nagyon magas fokszámú a felület és a Bernstein instabillá válna
glm::vec3 BezierSurface::EvaluateDeCasteljau(float u, float v) const {
    if (controlPoints.empty() || controlPoints[0].empty()) return glm::vec3(0.0f);

    int n = controlPoints.size();
    int m = controlPoints[0].size();

    // 1. lépés: Minden soron (u irány) futtatunk egy 1D de Casteljau-t a 'v' paraméterrel
    std::vector<glm::vec3> columnPoints(n);
    for (int i = 0; i < n; ++i) {
        std::vector<glm::vec3> tempRow(m);
        for (int j = 0; j < m; ++j) {
            tempRow[j] = controlPoints[i][j]->GetPosition();
        }
        columnPoints[i] = DeCasteljau1D(tempRow, v);
    }

    // 2. lépés: A kapott oszlopon lefuttatjuk a de Casteljau-t az 'u' paraméterrel
    return DeCasteljau1D(columnPoints, u);
}

// --- SEGÉDFÜGGVÉNYEK ---

// Binomiális együttható (n alatt a k)
int BezierSurface::BinomialCoefficient(int n, int k) const {
    if (k < 0 || k > n) return 0;
    if (k == 0 || k == n) return 1;
    if (k > n / 2) k = n - k;

    int res = 1;
    for (int i = 1; i <= k; ++i) {
        res = res * (n - i + 1) / i;
    }
    return res;
}

// Bernstein polinom (B^n_i(t))
float BezierSurface::Bernstein(int n, int i, float t) const {
    return BinomialCoefficient(n, i) * std::pow(t, i) * std::pow(1.0f - t, n - i);
}

// 1D de Casteljau algoritmus egy pontlistára
glm::vec3 BezierSurface::DeCasteljau1D(std::vector<glm::vec3> points, float t) const {
    int count = points.size();
    for (int r = 1; r < count; ++r) {
        for (int i = 0; i < count - r; ++i) {
            // Lineáris interpoláció: (1-t)*P0 + t*P1
            points[i] = (1.0f - t) * points[i] + t * points[i + 1];
        }
    }
    return points[0];
}

void BezierSurface::OnPointChanged(glm::vec3 offset) {
    CoonsSurfaceChanged.Invoke();
}
