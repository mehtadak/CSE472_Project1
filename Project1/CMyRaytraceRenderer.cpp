#include "pch.h"
#include "CMyRaytraceRenderer.h"

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

	for (int r = 0; r < m_rayimageheight; r++)
	{
		for (int c = 0; c < m_rayimagewidth; c++)
		{
			double colorTotal[3] = { 0, 0, 0 };

			double x = xmin + (c + 0.5) / m_rayimagewidth * xwid;
			double y = ymin + (r + 0.5) / m_rayimageheight * yhit;


			// Construct a Ray
			CRay ray(CGrPoint(0, 0, 0), Normalize3(CGrPoint(x, y, -1, 0)));

			double t;                                   // Will be distance to intersection
			CGrPoint intersect;                         // Will by x,y,z location of intersection
			const CRayIntersection::Object* nearest;    // Pointer to intersecting object
			if (m_intersection.Intersect(ray, 1e20, NULL, nearest, t, intersect))
			{
				// We hit something...
				// Determine information about the intersection
				CGrPoint N;
				CGrMaterial* material;
				CGrTexture* texture;
				CGrPoint texcoord;

				m_intersection.IntersectInfo(ray, nearest, t,
					N, material, texture, texcoord);

				if (material != NULL)
				{
					double color[] = { 0, 0, 0 };
					for (int i = 0; i < LightCnt(); i++)
					{
						const Light& light = GetLight(i);

						//Ambient Component
						for (int a = 0; a < 3; a++) {
							color[a] += material->Ambient(a) * light.m_ambient[a];
						}

						CGrPoint lightDir = Normalize3(light.m_pos - intersect);
						CRay shadowRay(intersect, lightDir);
						CGrPoint shadowIntersect;
						const CRayIntersection::Object* shadowNearest;
						double shadowT;

						//No Shadow!!! Add Diffuse and specular components
						if (!m_intersection.Intersect(shadowRay, 1e20, nearest, shadowNearest, shadowT, shadowIntersect))
						{
							double dotNormalLight = Dot3(N, lightDir);
							if (dotNormalLight > 0)
							{
								//Diffuse Component
								for (int d = 0; d < 3; d++) {
									color[d] += material->Diffuse(d) * light.m_diffuse[d] * dotNormalLight;
								}

								// specular component
								CGrPoint viewDirection = Normalize3(Eye() - intersect);
								CGrPoint half = Normalize3(lightDir + viewDirection);
								double sif = pow(max(Dot3(N, half), 0.0), material->Shininess());
								for (int c = 0; c < 3; c++) {
									color[c] += material->Specular(c) * light.m_specular[c] * sif;
								}

								//Specular Component
								//CGrPoint viewDir = Normalize3(Eye() - intersect);
								//CGrPoint reflectDir = lightDir - N * 2 * dotNormalLight;
								//float spec = pow(max(Dot3(viewDir, reflectDir), 0.0), material->Shininess());

								//for (int s = 0; s < 3; s++) {
								//	color[s] += material->Specular(s) * light.m_specular[s] * spec;
								//}

							}
						}
					}

					if (texture != NULL)
					{
						float texColor[3];
						texture->Pixel(texcoord.X(), texcoord.Y(), texColor);
						for (int j = 0; j < 3; j++)
						{
							color[j] *= texColor[j];
						}
					}

					// Clamp values to [0,1] before scaling to [0,255]
					for (int i = 0; i < 3; i++) {
						color[i] = max(0.0, min(color[i], 1.0));
					}

					m_rayimage[r][c * 3] = BYTE(color[0] * 255);
					m_rayimage[r][c * 3 + 1] = BYTE(color[1] * 255);
					m_rayimage[r][c * 3 + 2] = BYTE(color[2] * 255);
				}

			}
			else
			{
				// We hit nothing...
				m_rayimage[r][c * 3] = 0;
				m_rayimage[r][c * 3 + 1] = 0;
				m_rayimage[r][c * 3 + 2] = 0;
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
