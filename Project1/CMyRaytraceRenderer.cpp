#include "pch.h"
#include "CMyRaytraceRenderer.h"
#include <cmath>

void CMyRaytraceRenderer::SetWindow(CWnd* p_window)
{
    m_window = p_window;
}

bool CMyRaytraceRenderer::RendererStart()
{
	m_intersection.Initialize();

	m_mstack.clear();


	// We have to do all of the matrix work ourselves.
	// Set up the matrix stack.
	CGrTransform t;
	t.SetLookAt(Eye().X(), Eye().Y(), Eye().Z(),
		Center().X(), Center().Y(), Center().Z(),
		Up().X(), Up().Y(), Up().Z());

	m_mstack.push_back(t);

	m_material = NULL;

	return true;
}

void CMyRaytraceRenderer::RendererMaterial(CGrMaterial* p_material)
{
	m_material = p_material;
}

void CMyRaytraceRenderer::RendererPushMatrix()
{
	m_mstack.push_back(m_mstack.back());
}

void CMyRaytraceRenderer::RendererPopMatrix()
{
	m_mstack.pop_back();
}

void CMyRaytraceRenderer::RendererRotate(double a, double x, double y, double z)
{
	CGrTransform r;
	r.SetRotate(a, CGrPoint(x, y, z));
	m_mstack.back() *= r;
}

void CMyRaytraceRenderer::RendererTranslate(double x, double y, double z)
{
	CGrTransform r;
	r.SetTranslate(x, y, z);
	m_mstack.back() *= r;
}

//
// Name : CMyRaytraceRenderer::RendererEndPolygon()
// Description : End definition of a polygon. The superclass has
// already collected the polygon information
//

void CMyRaytraceRenderer::RendererEndPolygon()
{
    const std::list<CGrPoint>& vertices = PolyVertices();
    const std::list<CGrPoint>& normals = PolyNormals();
    const std::list<CGrPoint>& tvertices = PolyTexVertices();

    // Allocate a new polygon in the ray intersection system
    m_intersection.PolygonBegin();
    m_intersection.Material(m_material);

    if (PolyTexture())
    {
        m_intersection.Texture(PolyTexture());
    }

    std::list<CGrPoint>::const_iterator normal = normals.begin();
    std::list<CGrPoint>::const_iterator tvertex = tvertices.begin();

    for (std::list<CGrPoint>::const_iterator i = vertices.begin(); i != vertices.end(); i++)
    {
        if (normal != normals.end())
        {
            m_intersection.Normal(m_mstack.back() * *normal);
            normal++;
        }

        if (tvertex != tvertices.end())
        {
            m_intersection.TexVertex(*tvertex);
            tvertex++;
        }

        m_intersection.Vertex(m_mstack.back() * *i);
    }

    m_intersection.PolygonEnd();
}

bool CMyRaytraceRenderer::RendererEnd()
{
    m_intersection.LoadingComplete();

    double ymin = -tan(ProjectionAngle() / 2 * GR_DTOR);
    double yhit = -ymin * 2;

    double xmin = ymin * ProjectionAspect();
    double xwid = -xmin * 2;

    int numSamples = 2; 

    for (int r = 0; r < m_rayimageheight; r++)
    {
        for (int c = 0; c < m_rayimagewidth; c++)
        {
            double colorTotal[3] = { 0, 0, 0 };

            for (int sx = 0; sx < numSamples; sx++)
            {
                for (int sy = 0; sy < numSamples; sy++)
                {
                    double xOffset = (sx + 0.5) / numSamples;
                    double yOffset = (sy + 0.5) / numSamples;

                    double x = xmin + (c + xOffset) / m_rayimagewidth * xwid;
                    double y = ymin + (r + yOffset) / m_rayimageheight * yhit;

                    CRay ray(CGrPoint(0, 0, 0), Normalize3(CGrPoint(x, y, -1)));

                    double t;
                    CGrPoint intersect;
                    const CRayIntersection::Object* nearest;
                    if (m_intersection.Intersect(ray, 1e20, NULL, nearest, t, intersect))
                    {
                        CGrPoint N;
                        CGrMaterial* material;
                        CGrTexture* texture;
                        CGrPoint texcoord;

                        m_intersection.IntersectInfo(ray, nearest, t, N, material, texture, texcoord);

                        double surfaceColor[3] = { 0, 0, 0 };
                        if (material != NULL)
                        {
                            for (int i = 0; i < LightCnt(); i++)
                            {
                                const Light& light = GetLight(i);

                                for (int a = 0; a < 3; a++) {
                                    surfaceColor[a] += material->Ambient(a) * light.m_ambient[a];
                                }

                                CGrPoint lightDir = Normalize3(light.m_pos - intersect);
                                CRay shadowRay(intersect + N * 0.0001, lightDir);
                                const CRayIntersection::Object* shadowNearest;
                                double shadowT;
                                CGrPoint shadowIntersect;

                                if (!m_intersection.Intersect(shadowRay, 1e20, nearest, shadowNearest, shadowT, shadowIntersect))
                                {
                                    double dotNormalLight = Dot3(N, lightDir);
                                    if (dotNormalLight > 0)
                                    {
                                        for (int d = 0; d < 3; d++) {
                                            surfaceColor[d] += material->Diffuse(d) * light.m_diffuse[d] * dotNormalLight;
                                        }

                                        CGrPoint viewDirection = Normalize3(Eye() - intersect);
                                        CGrPoint half = Normalize3(lightDir + viewDirection);
                                        double sif = pow(max(Dot3(N, half), 0.0), material->Shininess());
                                        for (int sp = 0; sp < 3; sp++) {
                                            surfaceColor[sp] += material->Specular(sp) * light.m_specular[sp] * sif;
                                        }
                                    }
                                }
                            }

                            if (texture != NULL)
                            {
                                float texColor[3];
                                texture->Pixel(texcoord.X(), texcoord.Y(), texColor);
                                for (int j = 0; j < 3; j++) {
                                    surfaceColor[j] *= texColor[j];
                                }
                            }

                            if (m_fogEnabled)
                            {
                                double distance = intersect.Length3();
                                double fogFactor = exp(-m_fogDensity * distance);
                                for (int i = 0; i < 3; ++i) {
                                    surfaceColor[i] = fogFactor * surfaceColor[i] + (1 - fogFactor) * m_fogColor[i];
                                }
                            }
                        }

                        colorTotal[0] += surfaceColor[0];
                        colorTotal[1] += surfaceColor[1];
                        colorTotal[2] += surfaceColor[2];
                    }
                    else
                    {
                        if (m_fogEnabled)
                        {
                            colorTotal[0] += m_fogColor[0];
                            colorTotal[1] += m_fogColor[1];
                            colorTotal[2] += m_fogColor[2];
                        }
                    }
                }
            }

            colorTotal[0] /= (numSamples * numSamples);
            colorTotal[1] /= (numSamples * numSamples);
            colorTotal[2] /= (numSamples * numSamples);

            for (int i = 0; i < 3; i++) {
                colorTotal[i] = max(0.0, min(colorTotal[i], 1.0));
                m_rayimage[r][c * 3 + i] = BYTE(colorTotal[i] * 255);
            }
        }

        if ((r % 50) == 0)
        {
            m_window->Invalidate();
            MSG msg;
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                DispatchMessage(&msg);
        }
    }

    return true;
}

