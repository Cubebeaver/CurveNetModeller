#pragma once
#include <memory>

#include "editor/view/bezier_node_view.h"
#include "editor/view/curve_view.h"
#include "editor/view/surface_view.h"
#include "../../model/object/curve_mesh.h"
#include "../../model/element/i_Node.hpp"
#include "../../model/element/point.h"
#include "command/add_bezier_curve_to_curve_mesh_command.hpp"
#include "command/add_node_to_bezier_curve_command.hpp"
#include "command/add_point_to_curve_mesh_command.hpp"
#include "command/composite_command.hpp"
#include "command/merge_bezier_nodes_command.hpp"
#include "command/remove_point_from_curve_mesh_command.hpp"
#include "command/split_bezier_nodes_command.hpp"
#include "editor/view/bezier_surface_view.h"
#include "editor/view/curve_curvature_comb_view.h"
#include "editor/view/curve_rotation_minimizing_frame_view.h"
#include "editor/workspace/viewport.hpp"
#include "editor/workspace/workspaces.hpp"
#include "model/constraint/same_point_position_constraint.h"
#include "model/element/bezier_surface.h"

class CurveMeshController {
private:
    std::shared_ptr<CurveMesh> curveMesh;

    std::vector<std::weak_ptr<CoonsSurface>> selectedSurfaces;
    std::vector<std::weak_ptr<BezierCurve>> selectedEdges;
    std::vector<std::weak_ptr<BezierNode>> selectedNodes;
    std::vector<std::weak_ptr<Point>> selectedPoints;

    std::vector<std::unique_ptr<BezierNodeView>> nodeViews;
    std::vector<std::unique_ptr<CurveView>> curveViews;
    std::vector<std::unique_ptr<SurfaceView>> surfaceViews;
    //TODO ezt ne így pls
    std::vector<std::unique_ptr<BezierSurfaceView>> beziersurfaceViews;

    //DEBUG
    std::vector<std::unique_ptr<CurveCurvatureCombView>> curveCurvatureCombViews;
    std::vector<std::unique_ptr<CurveRotationMinimizingFrameView>> curveRotationMinimizingFrameViews;
    //DEBUG

public:
    CurveMeshController() : CurveMeshController(std::make_shared<CurveMesh>()) { }
    CurveMeshController(std::shared_ptr<CurveMesh> existing) : curveMesh(existing) {
        existing->CurveMeshChanged.AddListener(this, &CurveMeshController::SyncViews);

        Workspaces::viewport.lock()->OnClick.AddListener(this, &CurveMeshController::OnClick);
        Workspaces::viewport.lock()->OnDrag.AddListener(this, &CurveMeshController::OnDrag);
    }

    std::shared_ptr<CurveMesh> GetModel() { return curveMesh; }

    std::vector<std::weak_ptr<CoonsSurface>>& GetSelectedSurface() { return selectedSurfaces; }
    std::vector<std::weak_ptr<BezierCurve>>& GetSelectedEdge() { return selectedEdges; }
    std::vector<std::weak_ptr<BezierNode>>& GetSelectedNode() { return selectedNodes; }
    std::vector<std::weak_ptr<Point>>& GetSelectedPoint() { return selectedPoints; }

    std::weak_ptr<CoonsSurface> GetActiveSurface() { if (!selectedSurfaces.empty()) return selectedSurfaces.back(); else return { }; }
    std::weak_ptr<BezierCurve> GetActiveEdge() { if (!selectedEdges.empty()) return selectedEdges.back(); else return { }; }
    std::weak_ptr<BezierNode> GetActiveNode() { if (!selectedNodes.empty()) return selectedNodes.back(); else return { }; }
    std::weak_ptr<Point> GetActivePoint() { if (!selectedPoints.empty()) return selectedPoints.back(); else return { }; }

    void ForceSyncViews() {
        SyncViews();
    }

    void AddNewCurve() {
        auto newEdge = std::make_shared<BezierCurve>();
        auto n1 = std::make_shared<BezierNode>(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.5f, -0.5f, 0.0f), glm::vec3(-0.5f, 0.5f, 0.0f), HandleMode::Aligned);
        auto n2 = std::make_shared<BezierNode>(glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3( 0.5f,  0.0f, 0.0f), glm::vec3( 1.5f, 0.0f, 0.0f), HandleMode::Aligned);

        //DEBUG
        curveRotationMinimizingFrameViews.push_back(std::make_unique<CurveRotationMinimizingFrameView>(newEdge));
        //DEBUG

        CommandHistory::Add(CompositeCommand::Create()
        ->ExecuteAdd<AddBezierCurveToCurveMeshCommand>(curveMesh, newEdge)
        ->ExecuteAdd<AddPointToCurveMeshCommand>(curveMesh, n1->GetCenterHandle())
        ->ExecuteAdd<AddPointToCurveMeshCommand>(curveMesh, n1->GetLeftHandle())
        ->ExecuteAdd<AddPointToCurveMeshCommand>(curveMesh, n1->GetRightHandle())
        ->ExecuteAdd<AddNodeToBezierCurveCommand>(newEdge, n1, 0)
        ->ExecuteAdd<AddPointToCurveMeshCommand>(curveMesh, n2->GetCenterHandle())
        ->ExecuteAdd<AddPointToCurveMeshCommand>(curveMesh, n2->GetLeftHandle())
        ->ExecuteAdd<AddPointToCurveMeshCommand>(curveMesh, n2->GetRightHandle())
        ->ExecuteAdd<AddNodeToBezierCurveCommand>(newEdge, n2, 1));
    }

    void AddExistingCurve(std::shared_ptr<BezierCurve> curve) {
        curveMesh->AddEdge(curve);
        //TODO Command
    }

    void AddNewCoonsSurface() {
        auto c1 = std::make_shared<BezierCurve>();
        c1->AddNode(std::make_shared<BezierNode>(glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(-1.5f, 0.0f, -1.0f), glm::vec3(-0.5f, 0.0f, -1.0f), HandleMode::Aligned));
        c1->AddNode(std::make_shared<BezierNode>(glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3( 0.5f, 0.0f, -1.0f), glm::vec3( 1.5f, 0.0f, -1.0f), HandleMode::Aligned));

        auto c2 = std::make_shared<BezierCurve>();
        c2->AddNode(std::make_shared<BezierNode>(glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(-1.5f, 0.0f,  1.0f), glm::vec3(-0.5f, 0.0f,  1.0f), HandleMode::Aligned));
        c2->AddNode(std::make_shared<BezierNode>(glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3( 0.5f, 0.0f,  1.0f), glm::vec3( 1.5f, 0.0f,  1.0f), HandleMode::Aligned));

        auto d1 = std::make_shared<BezierCurve>();
        d1->AddNode(std::make_shared<BezierNode>(glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(-1.0f, 0.0f, -1.5f), glm::vec3(-1.0f, 0.0f, -0.5f), HandleMode::Aligned));
        d1->GetNodes()[0]->SetCenterHandle(c1->GetNodes()[0]->GetCenterHandle());
        d1->AddNode(std::make_shared<BezierNode>(glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(-1.0f, 0.0f,  0.5f), glm::vec3(-1.0f, 0.0f,  1.5f), HandleMode::Aligned));
        d1->GetNodes()[1]->SetCenterHandle(c2->GetNodes()[0]->GetCenterHandle());

        auto d2 = std::make_shared<BezierCurve>();
        d2->AddNode(std::make_shared<BezierNode>(glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3( 1.0f, 0.0f, -1.5f), glm::vec3( 1.0f, 0.0f, -0.5f), HandleMode::Aligned));
        d2->GetNodes()[0]->SetCenterHandle(c1->GetNodes()[1]->GetCenterHandle());
        d2->AddNode(std::make_shared<BezierNode>(glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3( 1.0f, 0.0f,  0.5f), glm::vec3( 1.0f, 0.0f,  1.5f), HandleMode::Aligned));
        d2->GetNodes()[1]->SetCenterHandle(c2->GetNodes()[1]->GetCenterHandle());

        auto newSurface = std::make_shared<CoonsSurface>(c1, c2, d1, d2);
        curveMesh->AddSurface(newSurface);

        curveMesh->AddPoint(c1->GetNodes()[0]->GetCenterHandle());
        curveMesh->AddPoint(c1->GetNodes()[0]->GetLeftHandle());
        curveMesh->AddPoint(c1->GetNodes()[0]->GetRightHandle());
        curveMesh->AddPoint(c1->GetNodes()[1]->GetCenterHandle());
        curveMesh->AddPoint(c1->GetNodes()[1]->GetLeftHandle());
        curveMesh->AddPoint(c1->GetNodes()[1]->GetRightHandle());
        curveMesh->AddEdge(c1);

        curveMesh->AddPoint(c2->GetNodes()[0]->GetCenterHandle());
        curveMesh->AddPoint(c2->GetNodes()[0]->GetLeftHandle());
        curveMesh->AddPoint(c2->GetNodes()[0]->GetRightHandle());
        curveMesh->AddPoint(c2->GetNodes()[1]->GetCenterHandle());
        curveMesh->AddPoint(c2->GetNodes()[1]->GetLeftHandle());
        curveMesh->AddPoint(c2->GetNodes()[1]->GetRightHandle());
        curveMesh->AddEdge(c2);

        //curveMesh->AddPoint(d1->GetNodes()[0]->GetCenterHandle());
        curveMesh->AddPoint(d1->GetNodes()[0]->GetLeftHandle());
        curveMesh->AddPoint(d1->GetNodes()[0]->GetRightHandle());
        //curveMesh->AddPoint(d1->GetNodes()[1]->GetCenterHandle());
        curveMesh->AddPoint(d1->GetNodes()[1]->GetLeftHandle());
        curveMesh->AddPoint(d1->GetNodes()[1]->GetRightHandle());
        curveMesh->AddEdge(d1);

        //curveMesh->AddPoint(d2->GetNodes()[0]->GetCenterHandle());
        curveMesh->AddPoint(d2->GetNodes()[0]->GetLeftHandle());
        curveMesh->AddPoint(d2->GetNodes()[0]->GetRightHandle());
        //curveMesh->AddPoint(d2->GetNodes()[1]->GetCenterHandle());
        curveMesh->AddPoint(d2->GetNodes()[1]->GetLeftHandle());
        curveMesh->AddPoint(d2->GetNodes()[1]->GetRightHandle());
        curveMesh->AddEdge(d2);

        //TODO Command
    }

    void AddNewBezierSurface() {
        auto s = std::make_shared<BezierSurface>(4, 4);

        auto ps = s->GetPoints();
        for (int i = 0; i < ps.size(); ++i) {
            for (int j = 0; j < ps[i].size(); ++j) {
                curveMesh->AddPoint(ps[i][j]);
            }
        }

        curveMesh->AddSurface(s);
    }

    void AddExistingSurface(std::shared_ptr<CoonsSurface> surface) {
        curveMesh->AddSurface(surface);
        //TODO Command
    }

    void ExtrudeSelectedEdge() {
        auto c1 = GetActiveEdge().lock();
        if (!c1) return;

        auto c10 = c1->GetNodes()[0];
        auto c11 = c1->GetNodes().back();

        auto offset = glm::vec3(2, 0, 0);

        auto c2 = std::make_shared<BezierCurve>();
        c2->AddNode(std::make_shared<BezierNode>(c10->GetCenterHandle()->GetPosition() + offset, c10->GetLeftHandle()->GetPosition() + offset, c10->GetRightHandle()->GetPosition() + offset, c10->GetMode()));
        c2->AddNode(std::make_shared<BezierNode>(c11->GetCenterHandle()->GetPosition() + offset, c11->GetLeftHandle()->GetPosition() + offset, c11->GetRightHandle()->GetPosition() + offset, c11->GetMode()));

        auto c10Pos = c10->GetCenterHandle()->GetPosition();
        auto c11Pos = c11->GetCenterHandle()->GetPosition();
        auto c20Pos = c2->GetNodes()[0]->GetCenterHandle()->GetPosition();
        auto c21Pos = c2->GetNodes()[1]->GetCenterHandle()->GetPosition();


        auto d1 = std::make_shared<BezierCurve>();
        d1->AddNode(std::make_shared<BezierNode>(c10Pos, c10Pos + offset * -0.25f, c10Pos + offset * 0.25f, HandleMode::Aligned));
        d1->GetNodes()[0]->SetCenterHandle(c1->GetNodes()[0]->GetCenterHandle());
        d1->AddNode(std::make_shared<BezierNode>(c20Pos, c20Pos + offset * -0.25f, c20Pos + offset * 0.25f, HandleMode::Aligned));
        d1->GetNodes()[1]->SetCenterHandle(c2->GetNodes()[0]->GetCenterHandle());

        auto d2 = std::make_shared<BezierCurve>();
        d2->AddNode(std::make_shared<BezierNode>(c11Pos, c11Pos + offset * -0.25f, c11Pos + offset * 0.25f, HandleMode::Aligned));
        d2->GetNodes()[0]->SetCenterHandle(c1->GetNodes()[1]->GetCenterHandle());
        d2->AddNode(std::make_shared<BezierNode>(c21Pos, c21Pos + offset * -0.25f, c21Pos + offset * 0.25f, HandleMode::Aligned));
        d2->GetNodes()[1]->SetCenterHandle(c2->GetNodes()[1]->GetCenterHandle());

        auto newSurface = std::make_shared<CoonsSurface>(c1, c2, d1, d2);
        curveMesh->AddSurface(newSurface);

        //curveMesh->AddEdge(c1);
        curveMesh->AddPoint(c2->GetNodes()[0]->GetCenterHandle());
        curveMesh->AddPoint(c2->GetNodes()[0]->GetLeftHandle());
        curveMesh->AddPoint(c2->GetNodes()[0]->GetRightHandle());
        curveMesh->AddPoint(c2->GetNodes()[1]->GetCenterHandle());
        curveMesh->AddPoint(c2->GetNodes()[1]->GetLeftHandle());
        curveMesh->AddPoint(c2->GetNodes()[1]->GetRightHandle());
        curveMesh->AddEdge(c2);

        curveMesh->AddPoint(d1->GetNodes()[0]->GetCenterHandle());
        curveMesh->AddPoint(d1->GetNodes()[0]->GetLeftHandle());
        curveMesh->AddPoint(d1->GetNodes()[0]->GetRightHandle());
        curveMesh->AddPoint(d1->GetNodes()[1]->GetCenterHandle());
        curveMesh->AddPoint(d1->GetNodes()[1]->GetLeftHandle());
        curveMesh->AddPoint(d1->GetNodes()[1]->GetRightHandle());
        curveMesh->AddEdge(d1);

        curveMesh->AddPoint(d2->GetNodes()[0]->GetCenterHandle());
        curveMesh->AddPoint(d2->GetNodes()[0]->GetLeftHandle());
        curveMesh->AddPoint(d2->GetNodes()[0]->GetRightHandle());
        curveMesh->AddPoint(d2->GetNodes()[1]->GetCenterHandle());
        curveMesh->AddPoint(d2->GetNodes()[1]->GetLeftHandle());
        curveMesh->AddPoint(d2->GetNodes()[1]->GetRightHandle());
        curveMesh->AddEdge(d2);

        //TODO Command
    }

    void MergeNodes() {
        auto active = GetActiveNode().lock();
        if (!active) return;

        auto cmd = CompositeCommand::Create();
        for (auto o : selectedNodes) {
            auto other = o.lock();
            if (!other) continue;

            cmd->ExecuteAdd<MergeBezierNodesCommand>(other, active);
            cmd->ExecuteAdd<RemovePointToCurveMeshCommand>(curveMesh, other->GetCenterHandle());
        }

        CommandHistory::Add(cmd);
    }

    void SplitNodes() {
        auto cmd = CompositeCommand::Create();

        for (auto n : selectedNodes) {
            if (auto ns = n.lock()) {
                cmd->ExecuteAdd<SplitBezierNodesCommand>(curveMesh, ns);
            }
        }

        CommandHistory::Add(cmd);
    }

    void FillCoons() {
        if (selectedEdges.size() != 4) return;

        auto c1 = selectedEdges[0].lock();
        auto c2 = selectedEdges[1].lock();
        auto d1 = selectedEdges[2].lock();
        auto d2 = selectedEdges[3].lock();

        if (!c1 || !c2 || !d1 || !d2) return;

        curveMesh->AddSurface(std::make_shared<CoonsSurface>(c1, c2, d1, d2));
        //TODO Command
    }

    void Present() {
        // Node views
        for (const auto& nodeView : nodeViews) {
            nodeView->Draw();
        }

        // Curve views
        for (const auto& curveView : curveViews) {
            curveView->Draw();
        }

        // Surface views
        for (const auto& surfaceView : surfaceViews) {
            surfaceView->Draw();
        }

        //TODO Hacky
        // Bezier surfaces
        for (const auto& surfaceView : beziersurfaceViews) {
            surfaceView->Draw();
        }

        //DEBUG
        for (const auto& v : curveCurvatureCombViews) {
            v->Draw();
        }

        for (const auto& v : curveRotationMinimizingFrameViews) {
            v->Draw();
        }
        //DEBUG
    }

private:
    void SyncViews() {
        // CREATE
        // Node views
        int nodeCount = 0;
        for (const auto& c : curveMesh->GetEdges())
            for (const auto& n : c->GetNodes())
                nodeCount++;

        if (nodeViews.size() != nodeCount) {
            nodeViews.clear();
            for (const auto& modelCurve : curveMesh->GetEdges()) {
                for (const auto& node : modelCurve->GetNodes()) {
                    auto nodeView = std::make_unique<BezierNodeView>();
                    nodeView->Update(*node);
                    nodeViews.push_back(std::move(nodeView));
                }
            }
        }

        // Curve views
        if (curveViews.size() != curveMesh->GetEdges().size()) {
            curveViews.clear();
            for (const auto& curve : curveMesh->GetEdges()) {
                auto nodeView = std::make_unique<CurveView>();
                nodeView->Update(*curve);
                curveViews.push_back(std::move(nodeView));
            }
        }

        // Surface views
        if (surfaceViews.size() != curveMesh->GetSurfaces().size()) {
            surfaceViews.clear();
            for (const auto& surface : curveMesh->GetSurfaces()) {
                auto surfaceView = std::make_unique<SurfaceView>();
                surfaceView->Update(*surface);
                surfaceViews.push_back(std::move(surfaceView));
            }
        }

        //TODO Hacky hacky, ez ne így legyen, hanem csináljuk már meg a model-view registry-t pls
        // Bezier surface
        int bezierSurfaceCount = 0;
        for (const auto& surface : curveMesh->GetSurfaces()) {
            const std::shared_ptr<BezierSurface> bs = std::dynamic_pointer_cast<BezierSurface>(surface);
            if (bs) bezierSurfaceCount++;
        }

        if (beziersurfaceViews.size() != bezierSurfaceCount) {
            beziersurfaceViews.clear();
            for (const auto& surface : curveMesh->GetSurfaces()) {
                const std::shared_ptr<BezierSurface> bs = std::dynamic_pointer_cast<BezierSurface>(surface);
                if (bs) {
                    auto surfaceView = std::make_unique<BezierSurfaceView>();
                    surfaceView->Update(*bs);
                    beziersurfaceViews.push_back(std::move(surfaceView));
                }
            }
        }

        // UPDATE ELEMENTS
        // Node views
        int n_node = 0;
        for (const auto& modelCurve : curveMesh->GetEdges()) {
            for (const auto& node : modelCurve->GetNodes()) {
                nodeViews[n_node]->Update(*node);
                n_node++;
            }
        }

        // Curve views
        auto& edges = curveMesh->GetEdges();
        for (int i = 0; i < edges.size(); i++) {
            curveViews[i]->Update(*edges[i], 10);
        }

        // Surface views
        auto& surfaces = curveMesh->GetSurfaces();
        for (int i = 0; i < surfaces.size(); i++) {
            surfaceViews[i]->Update(*surfaces[i], 10);
        }

        //TODO Hacky
        // Bezier surfaces
        int n_bs = 0;
        for (int i = 0; i < surfaces.size(); i++) {
            const std::shared_ptr<BezierSurface> bs = std::dynamic_pointer_cast<BezierSurface>(surfaces[i]);
            if (bs) {
                beziersurfaceViews[i]->Update(*bs);
                n_bs++;
            }
        }
    }

private:
    void OnClick(const glm::vec2& position, ImGuiMouseButton_ button) {
        if (button != ImGuiMouseButton_Left) return;

        float closestDistance = std::numeric_limits<float>::max();

        std::weak_ptr<Point> cp;
        std::weak_ptr<BezierNode> cn;
        std::weak_ptr<BezierCurve> ce;

        for (const auto p : curveMesh->GetPoints()) {
            float d = Camera::activeCamera->DistanceToRay(p->GetPosition(), position);
            if (d < closestDistance) {
                cp = p;
                closestDistance = d;
            }
        }

        //TODO Edge egy surface selection mode, meg a bezier node az egyfajta constraint legyen,
        //     és így majd az oldalak csúcsánál is a tanget plane-ek is ilyen constraint legyen
        for (const auto c : curveMesh->GetEdges()) {
            for (const auto n : c->GetNodes()) {
                for (const auto p : n->GetPoints()) {
                    if (p == cp.lock()) {
                        ce = c;
                        cn = n;
                    }
                }
            }
        }


        if (closestDistance < 0.1f) {
            auto cps = cp.lock();
            auto cns = cn.lock();
            auto ces = ce.lock();

            if (!ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
                selectedPoints.clear();
                selectedNodes.clear();
                selectedEdges.clear();
                selectedSurfaces.clear();
            }

            if (cps && std::find_if(selectedPoints.begin(), selectedPoints.end(), [&](const std::weak_ptr<Point>& p) { return p.lock() == cps; }) == selectedPoints.end())
                selectedPoints.push_back(cp);
            if (cns && std::find_if(selectedNodes.begin(), selectedNodes.end(), [&](const std::weak_ptr<BezierNode>& n) { return n.lock() == cns; }) == selectedNodes.end())
                selectedNodes.push_back(cn);
            if (ces && std::find_if(selectedEdges.begin(), selectedEdges.end(), [&](const std::weak_ptr<BezierCurve>& e) { return e.lock() == ces; }) == selectedEdges.end())
                selectedEdges.push_back(ce);
        }
        else /* too far away */ {
            selectedPoints.clear();
            selectedNodes.clear();
            selectedEdges.clear();
            selectedSurfaces.clear();
        }
    }

    void OnDrag(const glm::vec2& totalDelta, const glm::vec2& delta, const glm::vec2& position, ImGuiMouseButton_ button) {
        auto p = GetActivePoint().lock();
        if (button != ImGuiMouseButton_Left || !p) return;

        // 1. Lekérjük a jelenleg kiválasztott pont/handle 3D-s pozícióját
        std::weak_ptr<Point> currentPos3D = p;

        // --- EZEKET A VÁLTOZÓKAT A KAMERÁBÓL/VIEWPORTBÓL KELL LEKÉRNED ---
        glm::vec3 cameraPos = Camera::activeCamera->position;
        glm::vec3 cameraFront = Camera::activeCamera->direction; // A kamera előre mutató vektora
        glm::mat4 view = Camera::activeCamera->matView;
        glm::mat4 proj = Camera::activeCamera->matProjection;
        // -----------------------------------------------------------------

        // 2. A "Munkasík" definiálása
        // A sík normálvektora a kamera iránya (így a sík párhuzamos a képernyővel)
        glm::vec3 planeNormal = cameraFront;

        // 3. A jelenlegi és az előző egérpozícióhoz tartozó 3D-s sugarak (Rays)
        glm::vec3 currentRayDir = Camera::activeCamera->GetRayDirectionFromScreen(position.x, position.y);
        glm::vec3 prevRayDir = Camera::activeCamera->GetRayDirectionFromScreen(position.x - delta.x, position.y - delta.y);

        // 4. Egy kis lambda függvény, ami kiszámolja a sugár és a munkasík metszéspontját
        auto intersectPlane = [&](glm::vec3 rayDir) -> glm::vec3 {
            float denom = glm::dot(planeNormal, rayDir);
            if (std::abs(denom) > 0.0001f) {
                // A sík távolsága a kamerától a pont irányában
                float t = glm::dot(currentPos3D.lock()->GetPosition() - cameraPos, planeNormal) / denom;
                return cameraPos + rayDir * t;
            }
            return currentPos3D.lock()->GetPosition(); // Fallback
        };

        // 5. A 3D-s elmozdulás kiszámítása
        glm::vec3 hitCurrent = intersectPlane(currentRayDir);
        glm::vec3 hitPrev = intersectPlane(prevRayDir);
        glm::vec3 translation3D = hitCurrent - hitPrev;

        // 6. Az új pozíció beállítása a modellben
        for (auto sp : selectedPoints) {
            glm::vec3 newPos3D = sp.lock()->GetPosition() + translation3D;
            if (auto sps = sp.lock()) {
                sps->SetPosition(newPos3D);
            }
        }
    }
};


// for (const auto& curve : curveMesh->GetEdges()) {
//     for (const auto& node : curve->GetNodes()) {
//         for (const auto p : node->GetPoints()) {
//             float d = Camera::activeCamera->DistanceToRay(p->GetPosition(), position);
//             if (d < closestDistance) {
//                 cp = p;
//                 cn = node;
//                 ce = curve;
//                 closestDistance = d;
//             }
//         }
//     }
// }
