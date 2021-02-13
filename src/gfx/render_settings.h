#ifndef CRENDERER_RENDER_SETTINGS_H
#define CRENDERER_RENDER_SETTINGS_H

struct KCR_RenderSettings {
    bool showVertices : 1;
    bool showSolidFaces : 1;
    bool showWireframe : 1;
    bool enableBackFaceCulling : 1;
    bool enableLighting : 1;
};

#endif //CRENDERER_RENDER_SETTINGS_H
