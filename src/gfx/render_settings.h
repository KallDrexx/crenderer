#ifndef CRENDERER_RENDER_SETTINGS_H
#define CRENDERER_RENDER_SETTINGS_H

enum KCR_TriangleFillMode {FILL_NONE, FILL_MESH_TRI_COLORS, FILL_WHITE, FILL_TEXTURED};
enum KCR_LightingMode {LIGHTING_NONE, LIGHTING_FLAT, LIGHTING_SMOOTH};

struct KCR_RenderSettings {
    bool showVertices : 1;
    bool showWireframe : 1;
    bool enableBackFaceCulling : 1;
    enum KCR_LightingMode lightingMode;
    enum KCR_TriangleFillMode triangleFillMode;
};

#endif //CRENDERER_RENDER_SETTINGS_H
