#pragma once
#include "graphics/OpenGLWnd.h"
#include "graphics/GrCamera.h"
#include "graphics/GrObject.h"

// CChildView window

class CChildView : public COpenGLWnd
{
    // Construction
public:
    CChildView();

    // Attributes
public:
    CGrCamera m_camera;

    CGrPtr<CGrObject> m_scene;

    bool m_raytrace;
    bool m_fogEnabled;

    double m_fogDensity;
    double m_fogColor[3];

    CGrPtr<CGrComposite> m_fogVolume;
    CGrTransform m_fogTransform;

private:
    BYTE** m_rayimage;
    int         m_rayimagewidth;
    int         m_rayimageheight;

    // Operations
public:
    void OnGLDraw(CDC* pDC);
    void ConfigureRenderer(CGrRenderer* p_renderer);

    // Overrides
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    // Implementation
public:
    virtual ~CChildView();

    // Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnRenderRaytrace();
    afx_msg void OnUpdateRenderRaytrace(CCmdUI* pCmdUI);
    afx_msg void OnRenderFog();
    afx_msg void OnUpdateRenderFog(CCmdUI* pCmdUI);
};