#ifndef CRENDERER_RENDER_SETTINGS_H
#define CRENDERER_RENDER_SETTINGS_H

enum KCR_LightingMode {LIGHTING_NONE, LIGHTING_FLAT};

struct KCR_RenderSettings {
    bool showVertices : 1;
    bool showSolidFaces : 1;
    bool showWireframe : 1;
    bool enableBackFaceCulling : 1;
    enum KCR_LightingMode lightingMode;
};

#endif //CRENDERER_RENDER_SETTINGS_H
