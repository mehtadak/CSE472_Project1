#pragma once
#include "graphics/GrRenderer.h"
#include "graphics/RayIntersection.h"
#include "graphics/GrTexture.h"
#include <random>

class CMyRaytraceRenderer :
	public CGrRenderer
{
public:
    CMyRaytraceRenderer() { m_window = NULL; }
    int     m_rayimagewidth;
    int     m_rayimageheight;
    BYTE** m_rayimage;
    void SetImage(BYTE** image, int w, int h) { m_rayimage = image; m_rayimagewidth = w;  m_rayimageheight = h; }

    CWnd* m_window;

    CRayIntersection m_intersection;

    std::list<CGrTransform> m_mstack;
    CGrMaterial* m_material;

    // Fog parameters
    double m_fogDensity;
    double m_fogColor[3];

    bool m_fogEnabled;

    void SetWindow(CWnd* p_window);
    bool RendererStart();
    bool RendererEnd();
    void RendererMaterial(CGrMaterial* p_material);

    virtual void RendererPushMatrix();
    virtual void RendererPopMatrix();
    virtual void RendererRotate(double a, double x, double y, double z);
    virtual void RendererTranslate(double x, double y, double z);
    void RendererEndPolygon();

    // Method to set fog properties
    void SetFog(double density, double r, double g, double b) {
        m_fogDensity = density;
        m_fogColor[0] = r;
        m_fogColor[1] = g;
        m_fogColor[2] = b;
    }

    void SetFogTransform(const CGrTransform& transform) {
        m_fogTransform = transform;
    }

    CGrTransform m_fogTransform; // To store the fog volume's transform

    void EnableFog(bool enable) { m_fogEnabled = enable; }

private:
    std::default_random_engine m_randomEngine;
    std::uniform_real_distribution<> m_uniformDistribution;

    // Helper function to calculate fog factor using Beer-Lambert law
    double CalculateFogFactor(double distance) const {
        return exp(-m_fogDensity * distance);
    }

};

