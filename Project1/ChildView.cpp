
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "Project1.h"
#include "ChildView.h"
#include "graphics/OpenGLRenderer.h"
#include "CMyRaytraceRenderer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	m_camera.Set(20., 10., 50., 0., 0., 0., 0., 1., 0.);

	m_raytrace = false;
	m_fogEnabled = false;
	m_rayimage = NULL;

	// Initialize fog properties here
	m_fogDensity = 0.02; // Example density
	m_fogColor[0] = 0.7f; // Red component (light gray)
	m_fogColor[1] = 0.7f; // Green component
	m_fogColor[2] = 0.7f; // Blue component

	CGrPtr<CGrComposite> scene = new CGrComposite;
	m_scene = scene;

	//// A red box
	//CGrPtr<CGrMaterial> redpaint = new CGrMaterial;
	//redpaint->AmbientAndDiffuse(0.8f, 0.0f, 0.0f);
	//scene->Child(redpaint);

	//CGrPtr<CGrComposite> redbox = new CGrComposite;
	//redpaint->Child(redbox);
	//redbox->Box(1, 1, 1, 5, 5, 5);

	//// A white box
	//CGrPtr<CGrMaterial> whitepaint = new CGrMaterial;
	//whitepaint->AmbientAndDiffuse(0.8f, 0.8f, 0.8f);
	//scene->Child(whitepaint);

	//CGrPtr<CGrComposite> whitebox = new CGrComposite;
	//whitepaint->Child(whitebox);
	//whitebox->Box(-10, -10, -10, 5, 5, 5);
	//
	//// Floor
	//CGrPtr<CGrMaterial> greenpaint = new CGrMaterial;
	//greenpaint->AmbientAndDiffuse(0.1f, 0.8f, 0.1f);
	//scene->Child(greenpaint);

	//CGrPtr<CGrComposite> floor = new CGrComposite;
	//greenpaint->Child(floor);
	//floor->Box(-15, -18, -15, 30, 1, 30);

	//// blue pyramid
	//CGrPtr<CGrMaterial> bluepaint = new CGrMaterial;
	//bluepaint->AmbientAndDiffuse(0.0f, 0.0f, 0.8f);
	//scene->Child(bluepaint);
	//CGrPtr<CGrComposite> bluepyramid = new CGrComposite;
	//bluepaint->Child(bluepyramid);
	////verts
	CGrPoint top(0.0f, 2.5f, 0.0f);
	//CGrPoint base1(-3.0f, -3.0f, 3.0f);
	//CGrPoint base2(3.0f, -3.0f, 3.0f);
	//CGrPoint base3(3.0f, -3.0f, -3.0f);
	//CGrPoint base4(-3.0f, -3.0f, -3.0f);
	//// sides
	//bluepyramid->Poly3(top, base1, base2, NULL);
	//bluepyramid->Poly3(top, base2, base3, NULL);
	//bluepyramid->Poly3(top, base3, base4, NULL);
	//bluepyramid->Poly3(top, base4, base1, NULL);
	////base
	//bluepyramid->Poly4(base1, base4, base3, base2, NULL);

	//// tetrahedron
	//CGrPtr<CGrMaterial> tetPaint = new CGrMaterial;
	//tetPaint->AmbientAndDiffuse(1.0f, 0.5f, 0.0f); // orange
	//tetPaint->Specular(0.5f, 0.5f, 0.5f);
	//tetPaint->Shininess(100.0);
	//scene->Child(tetPaint);

	//CGrPtr<CGrComposite> tetrahedron = new CGrComposite;
	//tetPaint->Child(tetrahedron);

	//// tetrahedron vertices 
	//float edge = 3.0f;
	//float h = sqrt(2.0f / 3.0f) * edge;

	//CGrPoint v0(-5.0f - edge / 2, 0.0f, 0.0f);
	//CGrPoint v1(-5.0f + edge / 2, 0.0f, 0.0f);
	//CGrPoint v2(-5.0f, 0.0f, edge * sqrt(3.0f) / 2.0f);
	//CGrPoint v3(-5.0f, h, edge * sqrt(3.0f) / 6.0f);

	//tetrahedron->Poly3(v0, v1, v2, NULL);
	//tetrahedron->Poly3(v0, v2, v3, NULL);
	//tetrahedron->Poly3(v0, v3, v1, NULL);
	//tetrahedron->Poly3(v1, v3, v2, NULL);

	// sphere
	CGrPtr<CGrMaterial> spherePaint = new CGrMaterial;
	spherePaint->AmbientAndDiffuse(0.8f, 0.2f, 0.2f); // red
	spherePaint->Specular(0.5f, 0.5f, 0.5f);
	spherePaint->Shininess(100.0);
	scene->Child(spherePaint);

	CGrPtr<CGrComposite> sphere = new CGrComposite;
	spherePaint->Child(sphere);

	const float radius = 1.0f;
	const CGrPoint center(-10.0f, -3.0f, 0.0f);
	const int stacks = 20;
	const int slices = 20;

	//sphere vertices
	std::vector<CGrPoint> vertices;
	for (int i = 0; i <= stacks; ++i) {
		double phi = GR_PI * (-0.5 + (double)i / stacks);
		double y = radius * sin(phi);
		double r = radius * cos(phi);

		for (int j = 0; j <= slices; ++j) {
			double theta = 2 * GR_PI * j / slices;
			double x = r * cos(theta);
			double z = r * sin(theta);
			vertices.push_back(center + CGrPoint(x, y, z));
		}
	}

	//sphere triangles
	for (int i = 0; i < stacks; ++i) {
		for (int j = 0; j < slices; ++j) {
			int i0 = i * (slices + 1) + j;
			int i1 = i0 + 1;
			int i2 = (i + 1) * (slices + 1) + j;
			int i3 = i2 + 1;

			sphere->Poly3(vertices[i0], vertices[i2], vertices[i1], NULL);
			sphere->Poly3(vertices[i1], vertices[i2], vertices[i3], NULL);
		}
	}

	//// cylinder
	//CGrPtr<CGrMaterial> cylinderPaint = new CGrMaterial;
	//cylinderPaint->AmbientAndDiffuse(0.2f, 0.8f, 0.2f); // green
	//cylinderPaint->Specular(0.5f, 0.5f, 0.5f);
	//cylinderPaint->Shininess(50.0);
	//scene->Child(cylinderPaint);

	//CGrPtr<CGrComposite> cylinder = new CGrComposite;
	//cylinderPaint->Child(cylinder);

	//const float cylRadius = 1.5f;
	//const float cylHeight = 5.0f;
	//const CGrPoint cylBase(-10.0f, 0.0f, 0.0f);
	//const int segments = 20;

	//std::vector<CGrPoint> bottomPoints, topPoints;

	////cylinder vertices
	//for (int i = 0; i < segments; ++i) {
	//	double theta = 2 * GR_PI * i / segments;
	//	double x = cylRadius * cos(theta);
	//	double z = cylRadius * sin(theta);
	//	bottomPoints.push_back(cylBase + CGrPoint(x, 0.0f, z));
	//	topPoints.push_back(cylBase + CGrPoint(x, cylHeight, z));
	//}

	////cylinder
	//for (int i = 0; i < segments; ++i) {
	//	int next = (i + 1) % segments;

	//	// sides
	//	cylinder->Poly4(bottomPoints[i], topPoints[i], topPoints[next], bottomPoints[next], NULL);

	//	// bottom 
	//	cylinder->Poly3(cylBase, bottomPoints[i], bottomPoints[next], NULL);

	//	// top cap
	//	CGrPoint topCenter = cylBase + CGrPoint(0.0f, cylHeight, 0.0f);
	//	cylinder->Poly3(topCenter, topPoints[next], topPoints[i], NULL);
	//}

	// Floor
	CGrPtr<CGrMaterial> greenpaint = new CGrMaterial;
	greenpaint->AmbientAndDiffuse(0.1f, 0.8f, 0.1f);
	scene->Child(greenpaint);

	CGrPtr<CGrComposite> floor = new CGrComposite;
	greenpaint->Child(floor);
	floor->Box(-15, -5, -15, 30, 1, 30);

	// House Walls
	CGrPtr<CGrMaterial> housePaint = new CGrMaterial;
	housePaint->AmbientAndDiffuse(0.9f, 0.7f, 0.5f); // light brown
	scene->Child(housePaint);

	CGrPtr<CGrComposite> house = new CGrComposite;
	housePaint->Child(house);
	house->Box(-5, -4, -5, 10, 6, 10);

	// Roof
	CGrPtr<CGrMaterial> roofPaint = new CGrMaterial;
	roofPaint->AmbientAndDiffuse(0.5f, 0.1f, 0.1f); // dark red
	scene->Child(roofPaint);

	CGrPtr<CGrComposite> roof = new CGrComposite;
	roofPaint->Child(roof);

	CGrPoint roofFrontLeft(-5, 2, -5);
	CGrPoint roofFrontRight(5, 2, -5);
	CGrPoint roofBackLeft(-5, 2, 5);
	CGrPoint roofBackRight(5, 2, 5);
	CGrPoint roofPeak(0, 6, 0);

	// Front triangle
	roof->Poly3(roofFrontLeft, roofPeak, roofFrontRight);

	// Right triangle
	roof->Poly3(roofFrontRight, roofPeak, roofBackRight);

	// Back triangle
	roof->Poly3(roofBackRight, roofPeak, roofBackLeft);

	// Left triangle
	roof->Poly3(roofBackLeft, roofPeak, roofFrontLeft);



	// Door
	CGrPtr<CGrMaterial> doorPaint = new CGrMaterial;
	doorPaint->AmbientAndDiffuse(0.4f, 0.2f, 0.1f); // dark brown
	scene->Child(doorPaint);

	CGrPtr<CGrComposite> door = new CGrComposite;
	doorPaint->Child(door);
	// Positioned on the front wall
	door->Box(-1, -4, -5.01f, 2, 4, 0.02f);

	// Window
	CGrPtr<CGrMaterial> framePaint = new CGrMaterial;
	framePaint->AmbientAndDiffuse(0.2f, 0.2f, 0.2f); // gray frame
	scene->Child(framePaint);

	CGrPtr<CGrComposite> windowFrame = new CGrComposite;
	framePaint->Child(windowFrame);
	// Frame around window at left side wall
	// Top
	windowFrame->Box(-5.01f, -0.5f, -1.0f, 0.02f, 0.2f, 2.0f);
	// Bottom
	windowFrame->Box(-5.01f, -2.5f, -1.0f, 0.02f, 0.2f, 2.0f);
	// Left side
	windowFrame->Box(-5.01f, -2.5f, -1.0f, 0.02f, 2.2f, 0.2f);
	// Right side
	windowFrame->Box(-5.01f, -2.5f, 0.8f, 0.02f, 2.2f, 0.2f);

	//tree stump
	CGrPtr<CGrTexture> woodPaint = new CGrTexture;
	woodPaint->LoadFile(L"textures/plank01.bmp");
	scene->Child(woodPaint);
	glBindTexture(GL_TEXTURE_2D, woodPaint->TexName());
	CGrPtr<CGrComposite> cylinder = new CGrComposite;

	CGrPtr<CGrMaterial> brownPaint = new CGrMaterial;
	brownPaint->AmbientAndDiffuse(0.3f, 0.1f, 0.1f); // dark red
	scene->Child(brownPaint);

	brownPaint->Child(cylinder);	

	const float cylRadius = 1.5f;
	const float cylHeight = 5.0f;
	const CGrPoint cylBase(-10.0f, -4.0f, -10.0f);
	const int segments = 20;

	std::vector<CGrPoint> bottomPoints, topPoints;

	// cylinder vertices
	for (int i = 0; i < segments; ++i) {
		double theta = 2 * GR_PI * i / segments;
		double x = cylRadius * cos(theta);
		double z = cylRadius * sin(theta);
		bottomPoints.push_back(cylBase + CGrPoint(x, 0.0f, z));
		topPoints.push_back(cylBase + CGrPoint(x, cylHeight, z));
	}

	//cylinder
	for (int i = 0; i < segments; ++i) {
		int next = (i + 1) % segments;

		// sides
		cylinder->Poly4(bottomPoints[i], topPoints[i], topPoints[next], bottomPoints[next], NULL);

		// bottom 
		cylinder->Poly3(cylBase, bottomPoints[i], bottomPoints[next], NULL);

		// top cap
		CGrPoint topCenter = cylBase + CGrPoint(0.0f, cylHeight, 0.0f);
		cylinder->Poly3(topCenter, topPoints[next], topPoints[i], NULL);
	}
	glDisable(GL_TEXTURE_2D);

	// tetrahedron
	CGrPtr<CGrMaterial> tetPaint = new CGrMaterial;
	tetPaint->AmbientAndDiffuse(0.019f, 0.284f, 0.164f); // dark green
	tetPaint->Specular(0.5f, 0.5f, 0.5f);
	tetPaint->Shininess(100.0);
	scene->Child(tetPaint);

	CGrPtr<CGrComposite> tetrahedron = new CGrComposite;
	tetPaint->Child(tetrahedron);

	// tetrahedron vertices 
	float edge = 6.0f;
	float h = sqrt(2.0f / 3.0f) * edge;

	CGrPoint v0(-5.0f - edge / 2 - 5.0f, 1.0f, -12.0f);
	CGrPoint v1(-5.0f + edge / 2 - 5.0f, 1.0f, -12.0f);
	CGrPoint v2(-5.0f - 5.0f, 1.0f, edge * sqrt(3.0f) / 2.0f - 12.0f);
	CGrPoint v3(-5.0f - 5.0f, h + 1.0f, edge * sqrt(3.0f) / 6.0f - 12.0f);

	tetrahedron->Poly3(v0, v1, v2, NULL);
	tetrahedron->Poly3(v0, v2, v3, NULL);
	tetrahedron->Poly3(v0, v3, v1, NULL);
	tetrahedron->Poly3(v1, v3, v2, NULL);
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, COpenGLWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_RENDER_RAYTRACE, &CChildView::OnRenderRaytrace)
	ON_UPDATE_COMMAND_UI(ID_RENDER_RAYTRACE, &CChildView::OnUpdateRenderRaytrace)
	ON_COMMAND(ID_RENDER_FOG, &CChildView::OnRenderFog)
	ON_UPDATE_COMMAND_UI(ID_RENDER_FOG, &CChildView::OnUpdateRenderFog)
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!COpenGLWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

void CChildView::OnGLDraw(CDC* pDC)
{
	if (m_raytrace)
	{
		// Clear the color buffer
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set up for parallel projection
		int width, height;
		GetSize(width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, 0, height, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// If we got it, draw it
		if (m_rayimage)
		{
			glRasterPos3i(0, 0, 0);
			glDrawPixels(m_rayimagewidth, m_rayimageheight,
				GL_RGB, GL_UNSIGNED_BYTE, m_rayimage[0]);
		}

		glFlush();
	}
	else
	{
		//
		// Instantiate a renderer
		//

		COpenGLRenderer renderer;

		// Configure the renderer
		ConfigureRenderer(&renderer);

		//
		// Render the scene
		//

		renderer.Render(m_scene);
	}
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_camera.MouseDown(point.x, point.y);

	COpenGLWnd::OnLButtonDown(nFlags, point);
}


void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_camera.MouseMove(point.x, point.y, nFlags))
		Invalidate();

	COpenGLWnd::OnMouseMove(nFlags, point);
}


void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_camera.MouseDown(point.x, point.y, 2);

	COpenGLWnd::OnRButtonDown(nFlags, point);
}

//
// Name :         CChildView::ConfigureRenderer()
// Description :  Configures our renderer so it is able to render the scene.
//                Indicates how we'll do our projection, where the camera is,
//                and where any lights are located.
//

void CChildView::ConfigureRenderer(CGrRenderer* p_renderer)
{
	// Determine the screen size so we can determine the aspect ratio
	int width, height;
	GetSize(width, height);
	double aspectratio = double(width) / double(height);

	//
	// Set up the camera in the renderer
	//

	p_renderer->Perspective(m_camera.FieldOfView(),
		aspectratio, // The aspect ratio.
		20., // Near clipping
		1000.); // Far clipping

	// m_camera.FieldOfView is the vertical field of view in degrees.

	//
	// Set the camera location
	//

	p_renderer->LookAt(m_camera.Eye()[0], m_camera.Eye()[1], m_camera.Eye()[2],
		m_camera.Center()[0], m_camera.Center()[1], m_camera.Center()[2],
		m_camera.Up()[0], m_camera.Up()[1], m_camera.Up()[2]);

	//
	// Set the light locations and colors
	//

	float dimd = 0.5f;
	GLfloat dim[] = { dimd, dimd, dimd, 1.0f };
	GLfloat brightwhite[] = { 1.f, 1.f, 1.f, 1.0f };

	p_renderer->AddLight(CGrPoint(0, 20, 0),
		dim, brightwhite, brightwhite);

	// If it's a raytracer, set the fog properties
	CMyRaytraceRenderer* rayRenderer = dynamic_cast<CMyRaytraceRenderer*>(p_renderer);
	if (rayRenderer) {
		rayRenderer->SetFog(m_fogDensity, m_fogColor[0], m_fogColor[1], m_fogColor[2]);
		rayRenderer->EnableFog(m_fogEnabled);
	}
}


void CChildView::OnRenderRaytrace()
{
	m_raytrace = !m_raytrace;
	Invalidate();
	if (!m_raytrace)
		return;

	GetSize(m_rayimagewidth, m_rayimageheight);

	m_rayimage = new BYTE * [m_rayimageheight];

	int rowwid = m_rayimagewidth * 3;
	while (rowwid % 4)
		rowwid++;

	m_rayimage[0] = new BYTE[m_rayimageheight * rowwid];
	for (int i = 1; i < m_rayimageheight; i++)
	{
		m_rayimage[i] = m_rayimage[0] + i * rowwid;
	}

	for (int i = 0; i < m_rayimageheight; i++)
	{
		// Fill the image with blue
		for (int j = 0; j < m_rayimagewidth; j++)
		{
			m_rayimage[i][j * 3] = 0;               // red
			m_rayimage[i][j * 3 + 1] = 0;           // green
			m_rayimage[i][j * 3 + 2] = BYTE(255);   // blue
		}
	}
	
	// Instantiate a raytrace object
	CMyRaytraceRenderer raytrace;

	// Generic configurations for all renderers
	ConfigureRenderer(&raytrace);

	//
	// Render the Scene
	//
	raytrace.SetImage(m_rayimage, m_rayimagewidth, m_rayimageheight);
	raytrace.SetWindow(this);
	raytrace.Render(m_scene);
	Invalidate();
}


void CChildView::OnUpdateRenderRaytrace(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_raytrace);
}


void CChildView::OnRenderFog()
{
	// TODO: Add your command handler code here
	m_fogEnabled = !m_fogEnabled;
	Invalidate();
	//if (m_fogEnabled) {
	OnRenderRaytrace();
	//}
	Invalidate();
}


void CChildView::OnUpdateRenderFog(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_fogEnabled);
}
